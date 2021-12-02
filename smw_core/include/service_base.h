//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_SERVICE_BASE_H
#define SMW_SERVICE_BASE_H

#include <cstdint>

namespace smw::core
{
class ServiceBase
{
  public:
    ServiceBase(std::uint16_t service_id, std::uint16_t instance_id) noexcept;

  private:
    std::uint16_t m_service_id{0};
    std::uint16_t m_instance_id{0};
};
} // namespace smw::core
#endif // SMW_SERVICE_BASE_H
