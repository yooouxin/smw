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
    template <typename T, typename Serializer>
    static std::unique_ptr<TransportWriter<T>> createWriter(const std::string& topic_name) noexcept
    {
        /// TODO just return Fast-DDS writer
        return std::make_unique<FastDDSWriter<T, Serializer>>(topic_name);
    }

    template <typename T, typename Serializer>
    static std::unique_ptr<TransportReader<T>> createReader(const std::string& topic_name) noexcept
    {
        /// TODO just return Fast-DDS writer
        return std::make_unique<FastDDSReader<T, Serializer>>(topic_name);
    }
};

} // namespace smw::core
#endif // SMW_DDS_FACTORY_H
