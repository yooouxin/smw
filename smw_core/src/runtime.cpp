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
}


const RuntimeOption& Runtime::getOption() const noexcept
{
    return m_option;
}

Runtime& Runtime::getInstance() noexcept
{
    return initRuntime(std::nullopt);
}

std::unique_ptr<ServiceSkeleton> Runtime::createServiceSkeleton(const ServiceDescription& service_description) noexcept
{
    return std::make_unique<ServiceSkeleton>(service_description);
}
std::unique_ptr<ServiceProxy> Runtime::createServiceProxy(const ServiceDescription& service_description) noexcept
{
    return std::make_unique<ServiceProxy>(service_description);
}


} // namespace smw::core