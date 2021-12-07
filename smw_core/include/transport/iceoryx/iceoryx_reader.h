//
// Created by youxinxin on 2021/12/5.
//

#ifndef SMW_ICEORYX_READER_H
#define SMW_ICEORYX_READER_H
#include "iceoryx_runtime.h"
#include "sample_ptr.h"
#include "service_description.h"
#include "transport_reader.h"
#include <iceoryx_posh/popo/listener.hpp>
#include <iceoryx_posh/popo/untyped_subscriber.hpp>

namespace smw::core
{

template <typename T, template <typename> typename Serializer>
class IceoryxReader : public TransportReader<T>
{
  public:
    using data_callback_t = std::function<void(SamplePtr<const T>)>;

    explicit IceoryxReader(const ServiceDescription& service_description, std::uint32_t event_id) noexcept
        : m_iox_subscriber{nullptr}
        , m_iox_listener{nullptr}
    {
        IceoryxRuntime::initRuntime();
        iox::capro::IdString_t service_id_string{iox::cxx::TruncateToCapacity,
                                                 std::to_string(service_description.service_id)};
        iox::capro::IdString_t instance_id_string{iox::cxx::TruncateToCapacity,
                                                  std::to_string(service_description.instance_id)};
        iox::capro::IdString_t event_id_string{iox::cxx::TruncateToCapacity, std::to_string(event_id)};
        iox::capro::ServiceDescription iox_service_desc{service_id_string, instance_id_string, event_id_string};

        m_iox_subscriber = std::make_unique<iox::popo::UntypedSubscriber>(iox_service_desc);
        assert(m_iox_subscriber != nullptr);
        m_iox_listener = std::make_unique<::iox::popo::Listener>();
        assert(m_iox_listener != nullptr);

        m_iox_listener
            ->template attachEvent(*m_iox_subscriber,
                                   iox::popo::SubscriberEvent::DATA_RECEIVED,
                                   iox::popo::createNotificationCallback(onDataAvailable, *this))
            .or_else([](auto& error) {
                assert(0); /// never happen
            });
    }

    ~IceoryxReader() noexcept
    {
        m_iox_listener->template detachEvent(*m_iox_subscriber, iox::popo::SubscriberEvent::DATA_RECEIVED);
        disable();
    }

    void setDataCallback(const data_callback_t& callback) noexcept
    {
        std::unique_lock<std::mutex> lock(m_user_callback_mutex);
        m_user_callback = callback;
    }

    void enable() noexcept override
    {
        m_iox_subscriber->subscribe();
    }

    void disable() noexcept override
    {
        m_iox_subscriber->unsubscribe();
    }

  private:
    std::unique_ptr<iox::popo::UntypedSubscriber> m_iox_subscriber;
    data_callback_t m_user_callback;
    std::mutex m_user_callback_mutex;
    std::unique_ptr<::iox::popo::Listener> m_iox_listener;

    constexpr static bool IS_FIXED_DATA_TYPE = std::is_trivial_v<T>;

    static void onDataAvailable(::iox::popo::UntypedSubscriber*, IceoryxReader* self)
    {
        self->m_iox_subscriber->take()
            .and_then([self](const void* user_payload) {
                SamplePtr<const T> sample_ptr{nullptr};

                // dynamic data type,need deserializer and pass to user
                if constexpr (!IS_FIXED_DATA_TYPE)
                {
                    auto chunkHeader = ::iox::mepoo::ChunkHeader::fromUserPayload(user_payload);
                    auto payloadSize = chunkHeader->userPayloadSize();
                    /// construct a new sample,and deserialize to it

                    sample_ptr = SamplePtr<const T>(new const T, [self, user_payload](const T* ptr) {
                        self->m_iox_subscriber->release(user_payload);
                        delete ptr;
                    });

                    Serializer<T>::deserialize(user_payload, payloadSize, const_cast<T*>(sample_ptr.get()));
                }
                else
                { // Just convert userPayload to data type pointer and pass to user
                    const T* sample = reinterpret_cast<const T*>(user_payload);
                    sample_ptr =
                        SamplePtr<const T>(sample, [self](const T* ptr) { self->m_iox_subscriber->release(ptr); });
                }

                std::unique_lock<std::mutex> lock(self->m_user_callback_mutex);
                self->m_user_callback(std::move(sample_ptr));
            })
            .or_else([](auto& error) {
                /// TODO log error
                (void)error;
            });
    }
};

} // namespace smw::core
#endif // SMW_ICEORYX_READER_H
