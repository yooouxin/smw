//
// Created by youxinxin on 2021/12/8.
//

#ifndef SMW_HELLO_WORLD_H
#define SMW_HELLO_WORLD_H
#include <array>
#include <cstdint>

namespace smw::examples::zero_copy
{
struct HelloWorld
{
    uint32_t index;
    std::array<std::uint8_t, 5> message;
};
} // namespace smw::examples::zero_copy
#endif // SMW_HELLO_WORLD_H
