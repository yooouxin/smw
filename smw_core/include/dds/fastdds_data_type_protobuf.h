//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_FASTDDS_DATA_TYPE_PROTOBUF_H
#define SMW_FASTDDS_DATA_TYPE_PROTOBUF_H
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <google/protobuf/message.h>
#include <type_traits>

namespace smw::core
{
template <typename T>
class FastDDSDataTypeProtobuf : public eprosima::fastdds::dds::TopicDataType
{
    /// ensure a protobuf type
    static_assert(std::is_base_of_v<google::protobuf::Message, T>);

  public:
    FastDDSDataTypeProtobuf() noexcept
    {
        /// TODO find a static method??
        std::string type_name = T().GetTypeName();
        setName(type_name.c_str());

        m_typeSize = PB_DEFAULT_MAX_SIZE;
        m_isGetKeyDefined = false;
    }


    bool serialize(void* data, eprosima::fastrtps::rtps::SerializedPayload_t* payload) override
    {
        bool result = reinterpret_cast<T*>(data)->SerializeToArray(payload->data, payload->max_size);

        payload->length = getSerializedSizeProvider(data)();

        return result;
    }

    bool deserialize(eprosima::fastrtps::rtps::SerializedPayload_t* payload, void* data) override
    {
        bool result = reinterpret_cast<T*>(data)->ParseFromArray(payload->data, payload->length);

        return result;
    }


    std::function<uint32_t()> getSerializedSizeProvider(void* data) override
    {
        return [data]() -> uint32_t { return static_cast<uint32_t>((reinterpret_cast<T*>(data))->ByteSizeLong()); };
    }

    void* createData() override
    {
        return reinterpret_cast<void*>(new T());
    }

    void deleteData(void* data) override
    {
        delete (reinterpret_cast<T*>(data));
    }

    bool getKey(void*, eprosima::fastrtps::rtps::InstanceHandle_t*, bool) override
    {
        return false;
    }

  private:
    static constexpr std::uint32_t PB_DEFAULT_MAX_SIZE = 256;
};
} // namespace smw::core
#endif // SMW_FASTDDS_DATA_TYPE_PROTOBUF_H
