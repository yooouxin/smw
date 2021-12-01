//
// Created by youxinxin on 2021/12/1.
//

#ifndef SMW_SMWCPP_RUNTIME_H
#define SMW_SMWCPP_RUNTIME_H

#include "smw_cpp_runtime_option.h"

namespace smw::cpp
{
class Runtime
{
  public:
    /// @brief return runtime instance
    /// @param option options for initialize runtime
    /// @return runtime instance
    static Runtime& createInstance(const RuntimeOption& option) noexcept;

    Runtime(Runtime&&) = delete;
    Runtime& operator=(Runtime&&) = delete;

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;


    ~Runtime() noexcept;

    /// @brief return runtime name
    /// @return runtime name
    std::string getName() const noexcept;

  private:
    /// @brief Construct of runtime
    /// @param[in] option option for initialize runtime
    explicit Runtime(const RuntimeOption& option) noexcept;
    RuntimeOption m_option;
};
} // namespace smw::cpp
#endif // SMW_SMWCPP_RUNTIME_H
