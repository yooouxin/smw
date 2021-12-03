//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_FASTDDS_PARTICIPANT_H
#define SMW_FASTDDS_PARTICIPANT_H

#include "fastdds_data_type_protobuf.h"
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

namespace smw::core
{

class FastDDSParticipant
{
  public:
    static FastDDSParticipant& getInstance() noexcept;
    eprosima::fastdds::dds::DomainParticipant* getParticipant() noexcept;

    FastDDSParticipant(const FastDDSParticipant&) = delete;
    FastDDSParticipant& operator=(const FastDDSParticipant&) = delete;

    FastDDSParticipant(FastDDSParticipant&&) = delete;
    FastDDSParticipant& operator=(FastDDSParticipant&&) = delete;

    ~FastDDSParticipant() noexcept;

    /// @brief get a Fast-DDS topic
    /// @tparam T data type of this topic
    /// @param topic_name
    template <typename T>
    eprosima::fastdds::dds::Topic* getTopic(const std::string& topic_name) noexcept
    {
        std::unique_lock<std::mutex> lock(m_topics_mutex);
        eprosima::fastdds::dds::Topic* result = nullptr;
        if (m_topics.find(topic_name) != m_topics.end())
        {
            result = m_topics[topic_name].topic;
        }
        else
        {
            eprosima::fastdds::dds::TypeSupport type_support{new FastDDSDataTypeProtobuf<T>()};
            type_support.register_type(m_participant);
            result = FastDDSParticipant::getInstance().getParticipant()->create_topic(
                topic_name, type_support.get_type_name(), eprosima::fastdds::dds::TOPIC_QOS_DEFAULT);
            TopicWithRefCount topic_with_ref_count{result, 0};

            m_topics[topic_name] = topic_with_ref_count;
        }

        m_topics[topic_name].refCount++;
        return result;
    }

    /// @brief delete topic created by getTopic,but not real delete,it depends on reference count
    /// @param topic_name
    void deleteTopic(const std::string& topic_name) noexcept;
    
  private:
    FastDDSParticipant() noexcept;

    eprosima::fastdds::dds::DomainParticipant* m_participant;

    /// @brief wrapper for Fast-DDS topic with reference count
    struct TopicWithRefCount
    {
        eprosima::fastdds::dds::Topic* topic{nullptr};
        std::int16_t refCount{0};
    };

    std::map<std::string, TopicWithRefCount> m_topics;
    std::mutex m_topics_mutex;
};
} // namespace smw::core
#endif // SMW_FASTDDS_PARTICIPANT_H
