//
// Created by youxinxin on 2021/12/2.
//
#include "service_skeleton.h"
#include "runtime.h"

namespace smw::core
{

ServiceSkeleton::ServiceSkeleton(const ServiceDescription& service_description) noexcept
    : m_is_offered(false)
    , m_service_description(service_description)
{
    offerService();
}

void ServiceSkeleton::offerService() noexcept
{
    /// notify service registry
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, m_service_description);
    m_is_offered.store(true);
}

bool ServiceSkeleton::isOffered() const noexcept
{
    return m_is_offered.load();
}


void ServiceSkeleton::stopOfferService() noexcept
{
    /// notify service registry
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::STOP_OFFER,
                                                             m_service_description);
    m_is_offered.store(false);
}

ServiceSkeleton::~ServiceSkeleton() noexcept
{
    if (isOffered())
    {
        stopOfferService();
    }
}
} // namespace smw::core