//
// Created by youxinxin on 2021/12/5.
//

#ifndef SMW_ICEORYX_RUNTIME_H
#define SMW_ICEORYX_RUNTIME_H
#include "iceoryx_posh/runtime/posh_runtime.hpp"

namespace smw::core
{
class IceoryxRuntime
{
  public:
    static IceoryxRuntime& getInstance() noexcept;
    static void initRuntime() noexcept;

    IceoryxRuntime(const IceoryxRuntime&) = delete;
    IceoryxRuntime& operator=(const IceoryxRuntime&) = delete;

    IceoryxRuntime(IceoryxRuntime&&) = delete;
    IceoryxRuntime& operator=(IceoryxRuntime&&) = delete;

    ~IceoryxRuntime() noexcept;

  private:
    IceoryxRuntime() noexcept;
};
} // namespace smw::core
#endif // SMW_ICEORYX_RUNTIME_H
