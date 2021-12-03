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
}

void ServiceSkeleton::offerService() noexcept
{
    /// notify service registry
    ServiceRegistry::getInstance().offerService(m_service_description);
    m_is_offered.store(true);
}

bool ServiceSkeleton::isOffered() const noexcept
{
    return m_is_offered.load();
}

template <typename T>
Result<Publisher<T>> ServiceSkeleton::createPublisher(std::uint16_t event_id) noexcept
{
    return Result<Publisher<T>>();
}

void ServiceSkeleton::stopOfferService() noexcept
{
    /// notify service registry
    ServiceRegistry::getInstance().stopOfferService(m_service_description);
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