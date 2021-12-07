//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_TRANSPORT_READER_H
#define SMW_TRANSPORT_READER_H
#include "sample_ptr.h"
#include <functional>

namespace smw::core
{
template <typename T>
class TransportReader
{
  public:
    using data_callback_t = std::function<void(SamplePtr<const T>)>;
    virtual void setDataCallback(const data_callback_t& callback) noexcept = 0;
    virtual void enable() noexcept = 0;
    virtual void disable() noexcept = 0;
};
} // namespace smw::core
#endif // SMW_TRANSPORT_READER_H
