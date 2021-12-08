//
// Created by youxinxin on 2021/12/8.
//

#ifndef SMW_SERIALIZER_IDL_H
#define SMW_SERIALIZER_IDL_H
#include <fastcdr/Cdr.h>
#include <fastdds/dds/topic/TopicDataType.hpp>
namespace smw::core
{
template <typename PubSubType>
class SerializerIdl
{
    static_assert(std::is_base_of_v<eprosima::fastdds::dds::TopicDataType, PubSubType>);

  public:
    static PubSubType& getDelegateType() noexcept
    {
        static PubSubType delegate;
        return delegate;
    }

    static std::size_t getMaxSize() noexcept
    {
        return getDelegateType().m_typeSize;
    }

    static std::string getTypeName() noexcept
    {
        return getDelegateType().getName();
    }

    static std::size_t getSize(const void* data) noexcept
    {
        return getDelegateType().getSerializedSizeProvider(const_cast<void*>(data))();
    }

    static bool serialize(const void* from_data, void* dest_buffer, std::size_t* dest_len) noexcept
    {
        /// IMPROVE : more copy here
        eprosima::fastrtps::rtps::SerializedPayload_t payload{static_cast<uint32_t>(getMaxSize())};

        bool result = getDelegateType().serialize(const_cast<void*>(from_data), &payload);
        std::memcpy(dest_buffer, payload.data, payload.length);
        *dest_len = payload.length;
        return result;
    }

    static bool deserialize(const void* from_buffer, std::size_t from_len, void* dest_data) noexcept
    {
        /// IMPROVE : more copy here
        eprosima::fastrtps::rtps::SerializedPayload_t payload{static_cast<uint32_t>(from_len)};
        payload.length = from_len;
        std::memcpy(payload.data, from_buffer, from_len);

        bool result = getDelegateType().deserialize(&payload, dest_data);

        return result;
    }

    static void* createData() noexcept
    {
        return getDelegateType().createData();
    }

    static void deleteData(void* data) noexcept
    {
        getDelegateType().deleteData(data);
    }
};
} // namespace smw::core
#endif // SMW_SERIALIZER_IDL_H
