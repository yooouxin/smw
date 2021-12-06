//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_DDS_READER_H
#define SMW_DDS_READER_H
#include "sample_ptr.h"
#include <functional>

namespace smw::core
{
template <typename T>
class DdsReader
{
  public:
    using data_callback_t = std::function<void(SamplePtr<const T>)>;
    virtual void setDataCallback(const data_callback_t& callback) noexcept = 0;
};
} // namespace smw::core
#endif // SMW_DDS_READER_H
