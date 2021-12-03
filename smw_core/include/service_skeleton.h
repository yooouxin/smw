//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_SERVICE_SKELETON_H
#define SMW_SERVICE_SKELETON_H

#include "publisher.h"
#include "result.h"
#include "service_description.h"
#include "service_registry.h"
#include <atomic>

namespace smw::core
{
using namespace types;

class ServiceSkeleton
{
  public:
    /// @brief ctor of service skeleton
    /// @param service_id service id
    /// @param instance_id instance id
    ServiceSkeleton(const ServiceDescription& service_description) noexcept;

    /// @brief dtor of service skeleton
    ~ServiceSkeleton() noexcept;

    /// @brief disable copy because we could not deal with service state of another instance
    ServiceSkeleton(const ServiceSkeleton&) = delete;
    ServiceSkeleton& operator=(const ServiceSkeleton&) = delete;

    /// @brief disable move because we could not deal with service state of another instance
    ServiceSkeleton(ServiceSkeleton&&) = delete;
    ServiceSkeleton& operator=(ServiceSkeleton&&) = delete;

    /// @brief offer this service
    void offerService() noexcept;

    /// @brief stop offer this service
    void stopOfferService() noexcept;

    /// @brief return if this service has been offered
    bool isOffered() const noexcept;

    /// @brief create a event publisher
    /// @tparam T data type of event
    /// @param event_id id of event
    /// @return created publisher
    template <typename T>
    Result<Publisher<T>> createPublisher(std::uint16_t event_id) noexcept;

  private:
    std::atomic_bool m_is_offered;
    ServiceDescription m_service_description;
};

} // namespace smw::core
#endif // SMW_SERVICE_SKELETON_H