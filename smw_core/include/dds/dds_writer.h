//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_DDS_WRITER_H
#define SMW_DDS_WRITER_H
namespace smw::core
{
template <typename T>
class DdsWriter
{
  public:
    virtual bool write(const T&) noexcept = 0;
};
} // namespace smw::core
#endif // SMW_DDS_WRITER_H
