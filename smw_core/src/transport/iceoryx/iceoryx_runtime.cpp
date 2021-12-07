//
// Created by youxinxin on 2021/12/5.
//
#include "iceoryx/iceoryx_runtime.h"
#include "runtime.h"

namespace smw::core
{

IceoryxRuntime& IceoryxRuntime::getInstance() noexcept
{
    static IceoryxRuntime instance{};
    return instance;
}

IceoryxRuntime::IceoryxRuntime() noexcept
{
    /// init  iceoryx runtime
    iox::RuntimeName_t posh_runtime_name{iox::cxx::TruncateToCapacity, Runtime::getInstance().getOption().name};
    iox::runtime::PoshRuntime::initRuntime(posh_runtime_name);
}

IceoryxRuntime::~IceoryxRuntime() noexcept
{
    iox::runtime::PoshRuntime::getInstance().shutdown();
}

void IceoryxRuntime::initRuntime() noexcept
{
    (void)getInstance();
}

} // namespace smw::core