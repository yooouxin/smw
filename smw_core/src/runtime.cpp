//
// Created by youxinxin on 2021/12/1.
//
#include "runtime.h"


namespace smw::core
{
Runtime& Runtime::initRuntime(const std::optional<RuntimeOption>& option) noexcept
{
    static Runtime instance{option.value()};
    return instance;
}

Runtime::Runtime(const RuntimeOption& option) noexcept
    : m_option(option)
{
    /// use a dds m_participant exchange information
}


const RuntimeOption& Runtime::getOption() const noexcept
{
    return m_option;
}

Runtime& Runtime::getInstance() noexcept
{
    return initRuntime(std::nullopt);
}

} // namespace smw::core