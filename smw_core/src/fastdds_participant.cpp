//
// Created by youxinxin on 2021/12/2.
//
#include "dds/fastdds_participant.h"
#include "runtime.h"

namespace smw::core
{
FastDDSParticipant& FastDDSParticipant::getInstance() noexcept
{
    static FastDDSParticipant instance;
    return instance;
}

FastDDSParticipant::FastDDSParticipant() noexcept
    : m_participant(nullptr)
{
    auto* factory = eprosima::fastdds::dds::DomainParticipantFactory::get_instance();
    m_participant = factory->create_participant(Runtime::getInstance().getOption().domain_id,
                                                eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);
}

FastDDSParticipant::~FastDDSParticipant() noexcept
{
    auto* factory = eprosima::fastdds::dds::DomainParticipantFactory::get_instance();
    if (m_participant != nullptr)
    {
        m_participant->delete_contained_entities();
        factory->delete_participant(m_participant);
    }
}

eprosima::fastdds::dds::DomainParticipant* FastDDSParticipant::getParticipant() noexcept
{
    return m_participant;
}


} // namespace smw::core
