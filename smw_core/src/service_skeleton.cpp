//
// Created by youxinxin on 2021/12/2.
//
#include "service_skeleton.h"


namespace smw::core
{
ServiceSkeleton::ServiceSkeleton(std::uint16_t service_id, std::uint16_t instance_id) noexcept
    : ServiceBase(service_id, instance_id)
    , m_is_offered(false)
{
}

void ServiceSkeleton::offerService() noexcept
{
    /// use DDS to do service discovery
}

bool ServiceSkeleton::isOffered() const noexcept
{
    return m_is_offered;
}

template <typename T>
Result<Publisher<T>> ServiceSkeleton::createPublisher(std::uint16_t event_id) noexcept
{
    return Result<Publisher<T>>();
}

void ServiceSkeleton::stopOfferService() noexcept
{
}

ServiceSkeleton::~ServiceSkeleton() noexcept
{
    if (isOffered())
    {
        stopOfferService();
    }
}
} // namespace smw::core