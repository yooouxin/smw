//
// Created by youxinxin on 2021/12/3.
//

#ifndef SMW_UTILS_H
#define SMW_UTILS_H

#include <string>

namespace smw::utils
{
extern const char SMW_HOST_ID_FILE[];

const std::string& getHostId() noexcept;
const std::string& getProcessId() noexcept;
} // namespace smw::utils
#endif // SMW_UTILS_H
