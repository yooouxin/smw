//
// Created by youxinxin on 2021/12/8.
//

#ifndef SMW_SERIALIZER_HELLO_WORLD_H
#define SMW_SERIALIZER_HELLO_WORLD_H
#include "hello_world.h"
#include <cstdlib>
#include <cstring>
#include <string>

namespace smw::examples::zero_copy
{
class SerializerHelloworld
{
  public:
    inline std::size_t getMaxSize() noexcept
    {
        return sizeof(HelloWorld);
    }

    inline std::string getTypeName() noexcept
    {
        return "smw::examples::zero_copy::Helloworld";
    }

    inline std::size_t getSize(const void* data) noexcept
    {
        const HelloWorld* real_type = reinterpret_cast<const HelloWorld*>(data);
        return sizeof(real_type->index) + sizeof(real_type->message);
    }

    inline bool serialize(const void* from_data, void* dest_buffer, std::size_t* dest_len) noexcept
    {
        const HelloWorld* real_type = reinterpret_cast<const HelloWorld*>(from_data);
        std::byte* byte_buffer_pointer = reinterpret_cast<std::byte*>(dest_buffer);

        *dest_len = getSize(from_data);

        /// Just a sample
        std::memcpy(byte_buffer_pointer, &real_type->index, sizeof(real_type->index));
        std::memcpy(byte_buffer_pointer + sizeof(real_type->index), &real_type->message, sizeof(real_type->message));
        return true;
    }

    inline bool deserialize(const void* from_buffer, std::size_t from_len, void* dest_data) noexcept
    {
        HelloWorld* real_type = reinterpret_cast<HelloWorld*>(dest_data);
        const std::byte* byte_buffer_pointer = reinterpret_cast<const std::byte*>(from_buffer);

        std::memcpy(&real_type->index, byte_buffer_pointer, sizeof(real_type->index));
        std::memcpy(&real_type->message, byte_buffer_pointer + sizeof(real_type->index), sizeof(real_type->message));
        return true;
    }

    inline void* createData() noexcept
    {
        return reinterpret_cast<void*>(new HelloWorld());
    }

    inline void deleteData(void* data) noexcept
    {
        delete (reinterpret_cast<HelloWorld*>(data));
    }
};
} // namespace smw::examples::zero_copy
#endif // SMW_SERIALIZER_HELLO_WORLD_H
