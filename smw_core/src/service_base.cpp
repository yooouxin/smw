//
// Created by youxinxin on 2021/12/2.
//
#include "service_base.h"

namespace smw::core
{
ServiceBase::ServiceBase(std::uint16_t service_id, std::uint16_t instance_id) noexcept
    : m_service_id(service_id)
    , m_instance_id(instance_id)
{
}

} // namespace smw::core
