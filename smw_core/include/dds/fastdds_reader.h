//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_FASTDDS_READER_H
#define SMW_FASTDDS_READER_H
#include "../../test/test_data.pb.h"
#include "dds_reader.h"
#include "fastdds_participant.h"
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>

namespace smw::core
{
class FastDDSReaderListener : public eprosima::fastdds::dds::DataReaderListener
{
  public:
    inline void on_data_available(eprosima::fastdds::dds::DataReader* reader) noexcept override
    {
        if (m_void_callback)
        {
            /// Just notify
            m_void_callback();
        }
    }

    inline void setNotifyCallback(const std::function<void()>& callback) noexcept
    {
        m_void_callback = callback;
    }

  private:
    std::function<void()> m_void_callback;
};

template <typename T>
class FastDDSReader : public DdsReader<T>
{
  public:
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
        eprosima::fastrtps::types::ReturnCode_t return_code = eprosima::fastrtps::types::ReturnCode_t::RETCODE_ERROR;
        return_code = m_data_reader->set_listener(nullptr);

        assert(return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK);

        m_data_reader->close();

        return_code = m_subscriber->delete_contained_entities();
        assert(return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK);

        FastDDSParticipant::getInstance().deleteTopic(m_topic_name);
    }

    void setDataCallback(const std::function<void(const T&)>& callback) noexcept override
    {
        std::unique_lock<std::mutex> lock(m_user_callback_mutex);
        m_user_callback = callback;
    }

  private:
    eprosima::fastdds::dds::Subscriber* m_subscriber;
    eprosima::fastdds::dds::DataReader* m_data_reader;
    eprosima::fastdds::dds::Topic* m_topic;
    FastDDSReaderListener m_listener;
    std::string m_topic_name;
    int32_t m_queue_size;
    std::function<void(const T&)> m_user_callback;
    std::mutex m_user_callback_mutex;

    static constexpr int32_t DEFAULT_QUEUE_SIZE = 10;


    void onDataAvailable()
    {
        T dds_data{};
        eprosima::fastdds::dds::SampleInfo info;
        m_data_reader->take_next_sample(&dds_data, &info);

        if (info.valid_data)
        {
            std::unique_lock<std::mutex> lock(m_user_callback_mutex);
            if (m_user_callback)
            {
                m_user_callback(dds_data);
            }
        }
    }

    void init() noexcept
    {
        /// create a subscriber and data reader
        m_subscriber = FastDDSParticipant::getInstance().getParticipant()->create_subscriber(
            eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT);

        assert(m_subscriber != nullptr);

        m_topic = FastDDSParticipant::getInstance().template getTopic<T>(m_topic_name);
        assert(m_topic != nullptr);

        eprosima::fastdds::dds::DataReaderQos reader_qos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;
        reader_qos.endpoint().history_memory_policy =
            eprosima::fastrtps::rtps::MemoryManagementPolicy_t::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

        reader_qos.history().kind = eprosima::fastrtps::KEEP_LAST_HISTORY_QOS;
        reader_qos.history().depth = m_queue_size;

        m_listener.setNotifyCallback([this]() { onDataAvailable(); });

        m_data_reader = m_subscriber->create_datareader(
            m_topic, reader_qos, &m_listener, eprosima::fastdds::dds::StatusMask::data_available());
        assert(m_data_reader != nullptr);
    }
};
} // namespace smw::core
#endif // SMW_FASTDDS_READER_H
