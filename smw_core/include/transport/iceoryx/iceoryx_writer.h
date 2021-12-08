//
// Created by youxinxin on 2021/12/5.
//

#ifndef SMW_ICEORYX_WRITER_H
#define SMW_ICEORYX_WRITER_H
#include "iceoryx_runtime.h"
#include "service_description.h"
#include "transport_writer.h"
#include <iceoryx_posh/popo/untyped_publisher.hpp>

namespace smw::core
{
template <typename T, typename Serializer>
class IceoryxWriter : public TransportWriter<T>
{
  public:
    explicit IceoryxWriter(const ServiceDescription& service_description, std::uint32_t event_id) noexcept
        : m_iox_publisher{nullptr}
    {
        IceoryxRuntime::initRuntime();
        iox::capro::IdString_t service_id_string{iox::cxx::TruncateToCapacity,
                                                 std::to_string(service_description.service_id)};
        iox::capro::IdString_t instance_id_string{iox::cxx::TruncateToCapacity,
                                                  std::to_string(service_description.instance_id)};
        iox::capro::IdString_t event_id_string{iox::cxx::TruncateToCapacity, std::to_string(event_id)};
        iox::capro::ServiceDescription iox_service_desc{service_id_string, instance_id_string, event_id_string};

        m_iox_publisher = std::make_unique<iox::popo::UntypedPublisher>(iox_service_desc);
        assert(m_iox_publisher != nullptr);
    }

    SamplePtr<T> loan() noexcept override
    {
        // dynamic data types -- using heap,not shared memory here
        if constexpr (!IS_FIXED_DATA_TYPE)
        {
            return makeSamplePtr<T>();
        }

        void* user_payload = nullptr;

        m_iox_publisher->loan(sizeof(T))
            .and_then([&user_payload](auto& loaned_ptr) { user_payload = loaned_ptr; })
            .or_else([&user_payload](auto& error) { user_payload = nullptr; });

        // Don't crash when loan failed
        if (user_payload == nullptr)
        {
            return nullptr;
        }
        auto sample_ptr = SamplePtr<T>(new (user_payload) T{});
        return sample_ptr;
    }


    bool write(SamplePtr<T>&& data) noexcept override
    {
        if (!data)
        {
            return false;
        }
        // dynamic types --> allocate buffer from shared memory and publish by iceoryx
        if constexpr (!IS_FIXED_DATA_TYPE)
        {
            std::size_t data_size = m_serializer.getSize(data.get());
            void* user_payload = nullptr;

            m_iox_publisher->loan(data_size)
                .and_then([&user_payload](auto& loaned_ptr) { user_payload = loaned_ptr; })
                .or_else([&user_payload](auto& error) { user_payload = nullptr; });

            // Don't crash if loan failed
            if (user_payload == nullptr)
            {
                return false;
            }

            std::size_t serialized_len = 0;
            /// serialize to loaned memory
            m_serializer.serialize(data.get(), user_payload, &serialized_len);
            assert(serialized_len == data_size);

            m_iox_publisher->publish(user_payload);
        }
        else
        { // fixed size data types --> just publish chunk
            m_iox_publisher->publish(data.get());

            // DON'T REMOVE,data pointer must NOT be deleted by free(),because it's not allocated by heap,iceoryx will
            // delete it automatically
            data.release();
        }

        return true;
    }

    bool write(const T& data) noexcept override
    {
        return write(loan());
    }

    void enable() noexcept override
    {
        m_iox_publisher->offer();
    }

    void disable() noexcept override
    {
        m_iox_publisher->stopOffer();
    }

  private:
    std::unique_ptr<iox::popo::UntypedPublisher> m_iox_publisher;
    static inline Serializer m_serializer;
    constexpr static bool IS_FIXED_DATA_TYPE = std::is_trivial_v<T>;
};
} // namespace smw::core
#endif // SMW_ICEORYX_WRITER_H
