//
// Created by youxinxin on 2021/12/4.
//

#ifndef SMW_FASTDDS_DATA_TYPE_H
#define SMW_FASTDDS_DATA_TYPE_H

#include <fastdds/dds/topic/TopicDataType.hpp>

namespace smw::core
{
template <typename T, template <typename> typename Serializer>
class FastDDSDataType : public eprosima::fastdds::dds::TopicDataType
{
  public:
    FastDDSDataType() noexcept
    {
        std::string type_name = Serializer<T>::getTypeName();
        setName(type_name.c_str());

        m_typeSize = Serializer<T>::getMaxSize();
        m_isGetKeyDefined = false;
    }


    bool serialize(void* data, eprosima::fastrtps::rtps::SerializedPayload_t* payload) noexcept override
    {
        std::size_t serialized_len = 0;
        bool result = Serializer<T>::serialize(data, payload->data, &serialized_len);
        payload->length = serialized_len;
        return result;
    }

    bool deserialize(eprosima::fastrtps::rtps::SerializedPayload_t* payload, void* data) noexcept override
    {
        return Serializer<T>::deserialize(payload->data, payload->length, data);
    }


    std::function<uint32_t()> getSerializedSizeProvider(void* data) noexcept override
    {
        return [data]() -> uint32_t { return static_cast<uint32_t>(Serializer<T>::getSize(data)); };
    }

    void* createData() override
    {
        return Serializer<T>::createData();
    }

    void deleteData(void* data) noexcept override
    {
        Serializer<T>::deleteData(data);
    }

    bool getKey(void*, eprosima::fastrtps::rtps::InstanceHandle_t*, bool) noexcept override
    {
        return false;
    }
};
} // namespace smw::core
#endif // SMW_FASTDDS_DATA_TYPE_H
