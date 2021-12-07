//
// Created by youxinxin on 2021/12/5.
//
#include "service_proxy.h"

namespace smw::core
{

ServiceProxy::ServiceProxy(const ServiceDescription& service_description) noexcept
    : m_service_description(service_description)
{
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::FIND, m_service_description);
}

ServiceProxy::~ServiceProxy() noexcept
{
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::STOP_FIND, m_service_description);
}
} // namespace smw::core
