//
// Created by youxinxin on 2021/12/5.
//

#ifndef SMW_SERVICE_PROXY_H
#define SMW_SERVICE_PROXY_H
#include "result.h"
#include "service_description.h"
#include "service_registry.h"
#include "subscriber.h"
#include <atomic>

namespace smw::core
{
enum class ServiceProxyError
{
    NO_IMPLEMENT = -1
};

class ServiceProxy
{
  public:
    /// @brief ctor of service skeleton
    /// @param service_id service id
    /// @param instance_id instance id
    explicit ServiceProxy(const ServiceDescription& service_description) noexcept;

    /// @brief dtor of service skeleton
    ~ServiceProxy() noexcept;

    /// @brief disable copy because we could not deal with service state of another instance
    ServiceProxy(const ServiceProxy&) = delete;
    ServiceProxy& operator=(const ServiceProxy&) = delete;

    /// @brief disable move because we could not deal with service state of another instance
    ServiceProxy(ServiceProxy&&) = delete;
    ServiceProxy& operator=(ServiceProxy&&) = delete;

    /// @brief create a event publisher
    /// @tparam T data type of event
    /// @param event_id id of event
    /// @return created publisher
    template <typename T, template <typename> typename Serializer = SerializerProtobuf>
    Result<std::unique_ptr<Subscriber<T, Serializer>>, ServiceProxyError>
    createSubscriber(std::uint16_t event_id) noexcept
    {
        return Ok(std::make_unique<Subscriber<T, Serializer>>(m_service_description, event_id));
    }

  private:
    ServiceDescription m_service_description;
};
} // namespace smw::core
#endif // SMW_SERVICE_PROXY_H
