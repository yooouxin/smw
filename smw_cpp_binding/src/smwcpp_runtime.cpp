//
// Created by youxinxin on 2021/12/1.
//
#include "smwcpp_runtime.h"
#include <iostream>

namespace smw::cpp
{
Runtime& Runtime::createInstance(const RuntimeOption& option) noexcept
{
    static Runtime instance{option};
    return instance;
}

Runtime::Runtime(const RuntimeOption& option) noexcept
    : m_option(option)
{
}

Runtime::~Runtime() noexcept
{
}

std::string Runtime::getName() const noexcept
{
    return m_option.name;
}

} // namespace smw::cpp