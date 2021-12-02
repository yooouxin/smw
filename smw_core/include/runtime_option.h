//
// Created by youxinxin on 2021/12/1.
//

#ifndef SMW_RUNTIME_OPTION_H
#define SMW_RUNTIME_OPTION_H

#include <string>

namespace smw::core
{
struct RuntimeOption
{
    std::string name{"no-name"};
    std::uint32_t domain_id{0};
};
} // namespace smw::core
#endif // SMW_RUNTIME_OPTION_H
