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

    template <typename T>
    eprosima::fastdds::dds::Topic* getTopic(const std::string& topic_name) noexcept
    {
        eprosima::fastdds::dds::Topic* result = nullptr;
        if (m_topics.find(topic_name) != m_topics.end())
        {
            result = m_topics[topic_name];
        }
        else
        {
            eprosima::fastdds::dds::TypeSupport type_support{new FastDDSDataTypeProtobuf<T>()};
            type_support.register_type(m_participant);
            result = FastDDSParticipant::getInstance().getParticipant()->create_topic(
                topic_name, type_support.get_type_name(), eprosima::fastdds::dds::TOPIC_QOS_DEFAULT);
            m_topics[topic_name] = result;
        }

        return result;
    }

  private:
    FastDDSParticipant() noexcept;

    eprosima::fastdds::dds::DomainParticipant* m_participant;
    std::map<std::string, eprosima::fastdds::dds::Topic*> m_topics;
};
} // namespace smw::core
#endif // SMW_FASTDDS_PARTICIPANT_H
