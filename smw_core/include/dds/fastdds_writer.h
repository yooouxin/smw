//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_FASTDDS_WRITER_H
#define SMW_FASTDDS_WRITER_H
#include "dds_writer.h"
#include "fastdds_participant.h"
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/Topic.hpp>
namespace smw::core
{
template <typename T>
class FastDDSWriter : public DdsWriter<T>
{
  public:
    FastDDSWriter(const std::string& topic_name) noexcept
        : m_publisher(nullptr)
        , m_data_writer(nullptr)
        , m_topic_name(topic_name)
        , m_topic(nullptr)
    {
        init();
    }

    ~FastDDSWriter() noexcept
    {
        m_data_writer->close();
        m_publisher->delete_contained_entities();
    }
    
    bool write(const T& data) noexcept override
    {
        return m_data_writer->write(const_cast<T*>(&data));
    }

  private:
    eprosima::fastdds::dds::Publisher* m_publisher;
    eprosima::fastdds::dds::DataWriter* m_data_writer;
    eprosima::fastdds::dds::Topic* m_topic;
    std::string m_topic_name;

    static constexpr int32_t DEFAULT_HISTORY_DEPTH = 100;

    void init() noexcept
    {
        /// create a publisher and data writer
        m_publisher = FastDDSParticipant::getInstance().getParticipant()->create_publisher(
            eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT);

        assert(m_publisher != nullptr);

        eprosima::fastdds::dds::TypeSupport type_support{new FastDDSDataTypeProtobuf<T>};
        type_support.register_type(FastDDSParticipant::getInstance().getParticipant());

        m_topic = FastDDSParticipant::getInstance().template getTopic<T>(m_topic_name);
        assert(m_topic != nullptr);

        eprosima::fastdds::dds::DataWriterQos writer_qos = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;
        writer_qos.endpoint().history_memory_policy =
            eprosima::fastrtps::rtps::MemoryManagementPolicy_t::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

        writer_qos.history().kind = eprosima::fastrtps::KEEP_LAST_HISTORY_QOS;
        writer_qos.history().depth = DEFAULT_HISTORY_DEPTH;

        m_data_writer = m_publisher->create_datawriter(m_topic, writer_qos);
        assert(m_data_writer != nullptr);
    }
};


} // namespace smw::core
#endif // SMW_FASTDDS_WRITER_H
