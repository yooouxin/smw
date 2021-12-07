//
// Created by youxinxin on 2021/12/4.
//

#ifndef SMW_SERIALIZER_PROTOBUF_H
#define SMW_SERIALIZER_PROTOBUF_H
#include "google/protobuf/message.h"
#include "type_traits"

namespace smw::core
{

/// @brief
/// @tparam T
template <typename T>
class SerializerProtobuf
{
    /// ensure a protobuf type
    static_assert(std::is_base_of_v<google::protobuf::Message, T>);

  public:
    static std::size_t getMaxSize() noexcept
    {
        return PB_DEFAULT_MAX_SIZE; /// just a sample
    }

    static std::string getTypeName() noexcept
    {
        return T().GetTypeName();
    }

    static std::size_t getSize(const void* data) noexcept
    {
        return static_cast<std::size_t>((reinterpret_cast<const T*>(data))->ByteSizeLong());
    }

    static bool serialize(const void* from_data, void* dest_buffer, std::size_t* dest_len) noexcept
    {
        *dest_len = getSize(from_data);

        bool result = reinterpret_cast<const T*>(from_data)->SerializeToArray(dest_buffer, *dest_len);

        return result;
    }

    static bool deserialize(const void* from_buffer, std::size_t from_len, void* dest_data) noexcept
    {
        bool result = reinterpret_cast<T*>(dest_data)->ParseFromArray(from_buffer, from_len);

        return result;
    }

    static void* createData() noexcept
    {
        return reinterpret_cast<void*>(new T());
    }

    static void deleteData(void* data) noexcept
    {
        delete (reinterpret_cast<T*>(data));
    }

  private:
    static constexpr std::size_t PB_DEFAULT_MAX_SIZE = 256;
};
} // namespace smw::core
#endif // SMW_SERIALIZER_PROTOBUF_H
