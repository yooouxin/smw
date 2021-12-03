//
// Created by youxinxin on 2021/12/3.
//

#ifndef SMW_DDS_FACTORY_H
#define SMW_DDS_FACTORY_H
#include "fastdds_reader.h"
#include "fastdds_writer.h"
#include <memory>

namespace smw::core
{
class DDSFactory
{
  public:
    template <typename T>
    static std::unique_ptr<DdsWriter<T>> createWriter(const std::string& topic_name) noexcept
    {
        /// just return Fast-DDS writer for sample
        return std::make_unique<FastDDSWriter<T>>(topic_name);
    }

    template <typename T>
    static std::unique_ptr<DdsReader<T>> createReader(const std::string& topic_name) noexcept
    {
        /// just return Fast-DDS writer for sample
        return std::make_unique<FastDDSReader<T>>(topic_name);
    }
};

} // namespace smw::core
#endif // SMW_DDS_FACTORY_H
