//
// Created by youxinxin on 2021/12/2.
//
#include "dds/fastdds_participant.h"
#include "runtime.h"

namespace smw::core
{
FastDDSParticipant& FastDDSParticipant::getInstance() noexcept
{
    static FastDDSParticipant instance{};
    return instance;
}

FastDDSParticipant::FastDDSParticipant() noexcept
    : m_participant(nullptr)
{
    auto* factory = eprosima::fastdds::dds::DomainParticipantFactory::get_instance();
    eprosima::fastdds::dds::DomainParticipantQos pqos;
    pqos.name(Runtime::getInstance().getOption().name);
    /// use udp as default transport
    auto udp_transport = std::make_shared<eprosima::fastrtps::rtps::UDPv4TransportDescriptor>();
    udp_transport->sendBufferSize = DEFAULT_UDP_BUFFER_SIZE;
    udp_transport->receiveBufferSize = DEFAULT_UDP_BUFFER_SIZE;
    udp_transport->non_blocking_send = true;

    // Link the Transport Layer to the Participant.
    pqos.transport().user_transports.push_back(udp_transport);

    // Avoid using the default transport
    pqos.transport().use_builtin_transports = false;

    m_participant = factory->create_participant(Runtime::getInstance().getOption().domain_id,
                                                eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);
}

FastDDSParticipant::~FastDDSParticipant() noexcept
{
    auto* factory = eprosima::fastdds::dds::DomainParticipantFactory::get_instance();
    if (m_participant != nullptr)
    {
        eprosima::fastrtps::types::ReturnCode_t return_code = eprosima::fastrtps::types::ReturnCode_t::RETCODE_ERROR;
        return_code = m_participant->delete_contained_entities();
        assert(return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK);

        return_code = m_participant->set_listener(nullptr);
        assert(return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK);

        m_participant->close();

        factory->delete_participant(m_participant);
        assert(return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK);
    }
}

eprosima::fastdds::dds::DomainParticipant* FastDDSParticipant::getParticipant() noexcept
{
    return m_participant;
}

void FastDDSParticipant::deleteTopic(const std::string& topic_name) noexcept
{
    std::unique_lock<std::mutex> lock(m_topics_mutex);
    if (m_topics.find(topic_name) != m_topics.end())
    {
        m_topics[topic_name].refCount--;
        if (m_topics[topic_name].refCount <= 0)
        {
            eprosima::fastrtps::types::ReturnCode_t return_code;

            return_code = m_participant->delete_topic(m_topics[topic_name].topic);
            assert(return_code == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK);

            m_topics.erase(topic_name);
        }
    }
}


} // namespace smw::core
