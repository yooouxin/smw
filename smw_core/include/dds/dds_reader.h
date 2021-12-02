//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_DDS_READER_H
#define SMW_DDS_READER_H
#include <functional>
namespace smw::core
{
template <typename T>
class DdsReader
{
  public:
    virtual void setDataCallback(const std::function<void(const T&)>& callback) noexcept = 0;
};
} // namespace smw::core
#endif // SMW_DDS_READER_H
