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
    std::size_t getMaxSize() noexcept
    {
        return m_delegate.m_typeSize;
    }

    std::string getTypeName() noexcept
    {
        return m_delegate.getName();
    }

    std::size_t getSize(const void* data) noexcept
    {
        return m_delegate.getSerializedSizeProvider(const_cast<void*>(data))();
    }

    bool serialize(const void* from_data, void* dest_buffer, std::size_t* dest_len) noexcept
    {
        /// IMPROVE : more copy here
        eprosima::fastrtps::rtps::SerializedPayload_t payload{static_cast<uint32_t>(getMaxSize())};

        bool result = m_delegate.serialize(const_cast<void*>(from_data), &payload);
        std::memcpy(dest_buffer, payload.data, payload.length);
        *dest_len = payload.length;
        return result;
    }

    bool deserialize(const void* from_buffer, std::size_t from_len, void* dest_data) noexcept
    {
        /// IMPROVE : more copy here
        eprosima::fastrtps::rtps::SerializedPayload_t payload{static_cast<uint32_t>(from_len)};
        payload.length = from_len;
        std::memcpy(payload.data, from_buffer, from_len);

        bool result = m_delegate.deserialize(&payload, dest_data);

        return result;
    }

    void* createData() noexcept
    {
        return m_delegate.createData();
    }

    void deleteData(void* data) noexcept
    {
        m_delegate.deleteData(data);
    }

  private:
    inline static PubSubType m_delegate;
};
} // namespace smw::core
#endif // SMW_SERIALIZER_IDL_H
