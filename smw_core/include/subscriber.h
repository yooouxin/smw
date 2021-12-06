//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_SUBSCRIBER_H
#define SMW_SUBSCRIBER_H

#include "dds/dds_factory.h"
#include "iceoryx/iceoryx_reader.h"
#include "result.h"
#include "sample_ptr.h"
#include "serializer_protobuf.h"
#include "service_registry.h"
#include "spdlog/spdlog.h"
#include <vector>

namespace smw::core
{
using namespace types;

enum class SubscriberError
{
    NO_IMPLEMENT,
    READ_ERROR
};

template <typename T, template <typename> typename Serializer = SerializerProtobuf>
class Subscriber
{
  public:
    using on_data_available_callback_t = std::function<void(SamplePtr<const T>)>;
    /// @brief ctor of Subscriber
    /// @param service_id
    /// @param instance_id
    /// @param event_id
    /// @param queue_size
    Subscriber(const ServiceDescription& service_description,
               std::uint32_t event_id,
               std::size_t queue_size = DEFAULT_QUEUE_SIZE) noexcept
        : m_topic_name(
            fmt::format("/{}/{}/{}", service_description.service_id, service_description.instance_id, event_id))
        , m_queue_size(queue_size)
        , m_service_description(service_description)
        , m_event_id(event_id)
        , m_dds_reader(nullptr)
        , m_iceoryx_reader(nullptr)
        , m_observer_id{ServiceRegistry::getInstance().startObserveServiceStatus(
              m_service_description, [this](const ServiceStatus& status) { onServiceStatusChange(status); })}
    {
        /// prefer read local message,if not have local provider,read dds message
        /// so we observe service status to check which transport should use
    }

    Subscriber(const Subscriber&) = delete;
    Subscriber& operator=(const Subscriber&) = delete;

    Subscriber(Subscriber&&) = delete;
    Subscriber& operator=(Subscriber&&) = delete;

    ~Subscriber() noexcept
    {
        ServiceRegistry::getInstance().stopObserveServiceStatus(m_service_description, m_observer_id);

        if (m_iceoryx_reader)
        {
            m_iceoryx_reader->setDataCallback(nullptr);
        }

        if (m_dds_reader)
        {
            m_dds_reader->setDataCallback(nullptr);
        }
    }
    /// @brief return topic name of publisher
    /// @return topic name
    std::string getTopicName() const noexcept
    {
        return m_topic_name;
    }

    void registerReceiveHandler(on_data_available_callback_t callback) noexcept
    {
        std::unique_lock<std::mutex> lock(m_user_data_callback_mutex);
        m_user_data_callback = callback;
    }


  private:
    std::string m_topic_name;
    std::size_t m_queue_size;
    ServiceDescription m_service_description;
    std::uint32_t m_event_id;

    std::unique_ptr<DdsReader<T>> m_dds_reader;
    std::unique_ptr<IceoryxReader<T, Serializer>> m_iceoryx_reader;

    std::uint32_t m_observer_id;

    on_data_available_callback_t m_user_data_callback;
    std::mutex m_user_data_callback_mutex;

    static constexpr std::size_t DEFAULT_QUEUE_SIZE = 10;

    void onServiceStatusChange(const ServiceStatus& status)
    {
        auto onDataAvailable = [this](SamplePtr<const T> sample_ptr) {
            std::unique_lock<std::mutex> lock(m_user_data_callback_mutex);
            if (m_user_data_callback)
            {
                m_user_data_callback(std::move(sample_ptr));
            }
        };

        /// prefer read local message,if not have local provider,read dds message
        if (status.hasLocalProvider())
        {
            if (m_iceoryx_reader == nullptr)
            {
                m_iceoryx_reader = std::make_unique<IceoryxReader<T, Serializer>>(m_service_description, m_event_id);
                assert(m_iceoryx_reader);

                m_iceoryx_reader->setDataCallback(onDataAvailable);
            }
        }
        else
        {
            /// we don't need iceoryx reader if no local provider
            m_iceoryx_reader.reset();

            /// check
            if (status.hasRemoteProvider())
            {
                if (m_dds_reader == nullptr)
                {
                    m_dds_reader = DDSFactory::createReader<T, Serializer>(m_topic_name);

                    m_dds_reader->setDataCallback(onDataAvailable);
                }
            }
            else
            {
                m_dds_reader.reset();
            }
        }
    }
};
} // namespace smw::core
#endif // SMW_SUBSCRIBER_H
