//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_TRANSPORT_WRITER_H
#define SMW_TRANSPORT_WRITER_H

#include "sample_ptr.h"

namespace smw::core
{
template <typename T>
class TransportWriter
{
  public:
    virtual SamplePtr<T> loan() noexcept = 0;
    virtual bool write(const T&) noexcept = 0;
    virtual bool write(SamplePtr<T>&&) noexcept = 0;
    virtual void enable() noexcept = 0;
    virtual void disable() noexcept = 0;
};
} // namespace smw::core
#endif // SMW_TRANSPORT_WRITER_H
