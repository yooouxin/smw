//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_SERVICE_DESCRIPTION_H
#define SMW_SERVICE_DESCRIPTION_H

#include <cstdint>

namespace smw::core
{
struct ServiceDescription
{
    std::uint32_t service_id{0};
    std::uint32_t instance_id{0};

    /// @brief Uses the underlying service_id and instance_id compare method to provide an order.
    ///         This is needed to use ServiceDescription in sorted containers like map or set.
    inline bool operator<(const ServiceDescription& rhs) const noexcept
    {
        bool result = false;
        if (service_id < rhs.service_id)
        {
            result = true;
        }
        else
        {
            result = ((service_id == rhs.service_id) && (instance_id < rhs.instance_id));
        }
        return result;
    }
};
} // namespace smw::core
#endif // SMW_SERVICE_DESCRIPTION_H
