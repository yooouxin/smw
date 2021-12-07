//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_FASTDDS_READER_H
#define SMW_FASTDDS_READER_H
#include "fastdds_participant.h"
#include "spdlog/spdlog.h"
#include "transport/transport_reader.h"
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <future>

namespace smw::core
{
class FastDDSReaderListener : public eprosima::fastdds::dds::DataReaderListener
{
  public:
    inline void on_data_available(eprosima::fastdds::dds::DataReader* reader) noexcept override
    {
        std::unique_lock<std::mutex> lock(m_notify_callback_mutex);
        if (m_notify_callback)
        {
            /// Just notify
            m_notify_callback();
        }
    }

    inline void setNotifyCallback(const std::function<void()>& callback) noexcept
    {
        std::unique_lock<std::mutex> lock(m_notify_callback_mutex);
        m_notify_callback = callback;
    }

  private:
    std::function<void()> m_notify_callback;
    std::mutex m_notify_callback_mutex;
};

template <typename T, template <typename> typename Serializer>
class FastDDSReader : public TransportReader<T>
{
  public:
    using typename TransportReader<T>::data_callback_t;

    FastDDSReader(const std::string& topic_name, int32_t queue_size = DEFAULT_QUEUE_SIZE) noexcept
        : m_subscriber(nullptr)
        , m_topic_name(topic_name)
        , m_topic(nullptr)
        , m_data_reader(nullptr)
        , m_queue_size(queue_size)
    {
        init();
    }

    ~FastDDSReader() noexcept
    {
        disable();
        eprosima::fastrtps::types::ReturnCode_t return_code = m_subscriber->delete_contained_entities();
        assert(return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK);

        FastDDSParticipant::getInstance().deleteTopic(m_topic_name);
        m_dds_operation_async_future.wait(); /// wait all operation finished
    }

    void setDataCallback(const data_callback_t& callback) noexcept override
    {
        std::unique_lock<std::mutex> lock(m_user_callback_mutex);
        m_user_callback = callback;
    }

    void enable() noexcept override
    {
        std::unique_lock<std::mutex> lock(m_data_reader_mutex);
        if (m_data_reader != nullptr)
        {
            return;
        }
        auto create_reader_async = [this]() {
            eprosima::fastdds::dds::DataReaderQos reader_qos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;
            reader_qos.endpoint().history_memory_policy =
                eprosima::fastrtps::rtps::MemoryManagementPolicy_t::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

            reader_qos.history().kind = eprosima::fastrtps::KEEP_LAST_HISTORY_QOS;
            reader_qos.history().depth = m_queue_size;

            std::unique_lock<std::mutex> lock(m_data_reader_mutex);
            m_data_reader = m_subscriber->create_datareader(
                m_topic, reader_qos, &m_listener, eprosima::fastdds::dds::StatusMask::data_available());
            assert(m_data_reader != nullptr);
        };

        m_dds_operation_async_future = std::async(create_reader_async);
    }

    void disable() noexcept override
    {
        if (m_data_reader == nullptr)
        {
            return;
        }
        
        std::unique_lock<std::mutex> lock(m_data_reader_mutex);
        eprosima::fastrtps::types::ReturnCode_t return_code = m_data_reader->set_listener(nullptr);

        assert(return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK);

        m_data_reader->close();

        auto delete_reader_async = [this]() {
            std::unique_lock<std::mutex> lock(m_data_reader_mutex);
            m_subscriber->delete_datareader(m_data_reader);
            m_data_reader = nullptr;
        };
        m_dds_operation_async_future = std::async(delete_reader_async);
    }

  private:
    eprosima::fastdds::dds::Subscriber* m_subscriber;
    eprosima::fastdds::dds::DataReader* m_data_reader;
    std::mutex m_data_reader_mutex;
    eprosima::fastdds::dds::Topic* m_topic;
    FastDDSReaderListener m_listener;
    std::string m_topic_name;
    int32_t m_queue_size;
    data_callback_t m_user_callback;
    std::mutex m_user_callback_mutex;

    std::future<void> m_dds_operation_async_future;

    static constexpr int32_t DEFAULT_QUEUE_SIZE = 10;


    void onDataAvailable()
    {
        SamplePtr<const T> dds_data_ptr = makeSamplePtr<const T>();
        eprosima::fastdds::dds::SampleInfo info;
        eprosima::fastrtps::types::ReturnCode_t return_code =
            m_data_reader->take_next_sample(const_cast<T*>(dds_data_ptr.get()), &info);

        if (return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK && info.valid_data)
        {
            std::unique_lock<std::mutex> lock(m_user_callback_mutex);
            if (m_user_callback)
            {
                m_user_callback(std::move(dds_data_ptr));
            }
        }
        else
        {
            spdlog::error("error");
        }
    }

    void init() noexcept
    {
        /// create a subscriber and data reader
        m_subscriber = FastDDSParticipant::getInstance().getParticipant()->create_subscriber(
            eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT);

        assert(m_subscriber != nullptr);

        m_topic = FastDDSParticipant::getInstance().template getTopic<T, Serializer>(m_topic_name);
        assert(m_topic != nullptr);

        m_listener.setNotifyCallback([this]() { onDataAvailable(); });

        enable();
    }
};
} // namespace smw::core
#endif // SMW_FASTDDS_READER_H
