//
// Created by youxinxin on 2021/12/4.
//

#ifndef SMW_SERVICE_STATUS_H
#define SMW_SERVICE_STATUS_H
#include "service_discovery.pb.h"
#include <set>
namespace smw::core
{

struct ServiceEntityInfo
{
    std::string hostId;
    std::string processId;

    inline bool operator<(const ServiceEntityInfo& rhs) const noexcept
    {
        bool result = false;
        if (hostId < rhs.hostId)
        {
            result = true;
        }
        else
        {
            result = ((hostId == rhs.hostId) && (processId < rhs.processId));
        }

        return result;
    }
};


struct ServiceStatus
{
    std::set<ServiceEntityInfo> localProviders{};
    std::set<ServiceEntityInfo> remoteProviders{};
    std::set<ServiceEntityInfo> localConsumers{};
    std::set<ServiceEntityInfo> remoteConsumers{};

    /// @brief a helper to get if there's exist local consumer
    inline bool hasLocalConsumer() const noexcept
    {
        return localConsumers.size() > 0;
    }

    /// @brief a helper to get if there's exist remote consumer
    inline bool hasRemoteConsumer() const noexcept
    {
        return remoteConsumers.size() > 0;
    }

    /// @brief a helper to get if there's exist local provider
    inline bool hasLocalProvider() const noexcept
    {
        return localProviders.size() > 0;
    }

    /// @brief a helper to get if there's exist remote provider
    inline bool hasRemoteProvider() const noexcept
    {
        return remoteProviders.size() > 0;
    }
};

} // namespace smw::core
#endif // SMW_SERVICE_STATUS_H
