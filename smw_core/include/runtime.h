//
// Created by youxinxin on 2021/12/1.
//

#ifndef SMW_RUNTIME_H
#define SMW_RUNTIME_H

#include "dds/dds_writer.h"
#include "runtime_option.h"
#include "service_proxy.h"
#include "service_skeleton.h"
#include <iceoryx_posh/runtime/posh_runtime.hpp>
#include <optional>

namespace smw::core
{
class Runtime
{
  public:
    /// @brief return runtime instance
    /// @param option options for initialize runtime
    /// @return runtime instance
    static Runtime& initRuntime(const std::optional<RuntimeOption>& option) noexcept;

    /// @brief get initialized instance
    /// @return runtime instance
    static Runtime& getInstance() noexcept;

    Runtime(Runtime&&) = delete;
    Runtime& operator=(Runtime&&) = delete;

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;


    ~Runtime() noexcept = default;


    /// @brief return runtime options
    /// @return runtime options
    const RuntimeOption& getOption() const noexcept;

    static std::unique_ptr<ServiceSkeleton>
    createServiceSkeleton(const ServiceDescription& service_description) noexcept;

    static std::unique_ptr<ServiceProxy> createServiceProxy(const ServiceDescription& service_description) noexcept;

  private:
    /// @brief Construct of runtime
    /// @param[in] option option for initialize runtime
    explicit Runtime(const RuntimeOption& option) noexcept;
    RuntimeOption m_option;
};
} // namespace smw::core
#endif // SMW_RUNTIME_H
