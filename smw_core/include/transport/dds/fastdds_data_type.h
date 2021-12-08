//
// Created by youxinxin on 2021/12/4.
//

#ifndef SMW_FASTDDS_DATA_TYPE_H
#define SMW_FASTDDS_DATA_TYPE_H

#include <fastcdr/Cdr.h>
#include <fastdds/dds/topic/TopicDataType.hpp>

namespace smw::core
{
template <typename T, typename Serializer>
class FastDDSDataType : public eprosima::fastdds::dds::TopicDataType
{
  public:
    FastDDSDataType() noexcept
    {
        std::string type_name = m_serializer.getTypeName();
        setName(type_name.c_str());

        m_typeSize = m_serializer.getMaxSize() + CDR_VECTOR_LEN_SIZE + CDR_ENCAPSULATION_SIZE;
        m_isGetKeyDefined = false;
    }


    bool serialize(void* data, eprosima::fastrtps::rtps::SerializedPayload_t* payload) noexcept override
    {
        std::size_t serialized_len = 0;
        eprosima::fastcdr::FastBuffer fast_buffer(reinterpret_cast<char*>(payload->data),
                                                  payload->max_size); // Object that manages the raw buffer.
        eprosima::fastcdr::Cdr ser(fast_buffer,
                                   eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                                   eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.

        // Serialize encapsulation
        ser.serialize_encapsulation();

        /// first serialize vector size
        ser << static_cast<uint32_t>(m_serializer.getSize(data));

        /// then serialize actual size
        bool result =
            m_serializer.serialize(data, payload->data + CDR_VECTOR_LEN_SIZE + CDR_ENCAPSULATION_SIZE, &serialized_len);

        payload->length = ser.getSerializedDataLength() + static_cast<uint32_t>(m_serializer.getSize(data));
        return result;
    }

    bool deserialize(eprosima::fastrtps::rtps::SerializedPayload_t* payload, void* data) noexcept override
    {
        /// first de-serialize vector size
        eprosima::fastcdr::FastBuffer fast_buffer(reinterpret_cast<char*>(payload->data),
                                                  payload->length); // Object that manages the raw buffer.
        eprosima::fastcdr::Cdr de_ser(fast_buffer,
                                      eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                                      eprosima::fastcdr::Cdr::DDS_CDR); // Object that deserializes the data.
        // Deserialize encapsulation.
        de_ser.read_encapsulation();


        payload->encapsulation = de_ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

        uint32_t actual_size = 0;
        /// first de-serialize vector size
        de_ser >> actual_size;

        return m_serializer.deserialize(
            payload->data + CDR_VECTOR_LEN_SIZE + CDR_ENCAPSULATION_SIZE, actual_size, data);
    }

    /// @brief get alignment size of data,we may return more bytes to allocate more bytes to fill alignment
    /// @param data
    /// @return
    std::function<uint32_t()> getSerializedSizeProvider(void* data) noexcept override
    {
        return [data]() -> uint32_t {
            uint32_t current_alignment = 0;
            current_alignment += static_cast<uint32_t>(m_serializer.getSize(data))
                                 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);
            current_alignment += CDR_VECTOR_LEN_SIZE + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);
            current_alignment += CDR_ENCAPSULATION_SIZE + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);
            return current_alignment;
        };
    }

    void* createData() override
    {
        return m_serializer.createData();
    }

    void deleteData(void* data) noexcept override
    {
        m_serializer.deleteData(data);
    }

    bool getKey(void*, eprosima::fastrtps::rtps::InstanceHandle_t*, bool) noexcept override
    {
        return false;
    }

  private:
    static inline Serializer m_serializer;
    
    static constexpr std::size_t CDR_VECTOR_LEN_SIZE = 4;
    static constexpr std::size_t CDR_ENCAPSULATION_SIZE = 4;
};
} // namespace smw::core
#endif // SMW_FASTDDS_DATA_TYPE_H
