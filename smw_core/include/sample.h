//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_SAMPLE_H
#define SMW_SAMPLE_H
#include <memory>

namespace smw::core
{
template <typename T, typename Deleter = std::default_delete<T>>
class Sample
{
  public:
    Sample(std::unique_ptr<T, Deleter>&& data) noexcept
        : m_store(std::move(data))
    {
    }

    ~Sample() noexcept = default;

    Sample(Sample&&) noexcept = default;
    Sample& operator=(Sample&&) noexcept = default;

    Sample(const Sample&) = delete;
    Sample& operator=(const Sample&) = delete;

    /// @brief Get the pointer of data
    /// @return pointer of data
    T* get() noexcept
    {
        return m_store.get();
    }

    ///
    /// @brief Transparent access to the encapsulated type.
    /// @return a pointer to the encapsulated type.
    ///
    T* operator->() noexcept
    {
        return get();
    }

    ///
    /// @brief Transparent access to the encapsulated type.
    /// @return a pointer to the encapsulated type.
    ///
    const T* operator->() const noexcept
    {
        return get();
    }

    T& operator*() noexcept
    {
        return *get();
    }

    const T& operator*() const noexcept
    {
        return *get();
    }

  private:
    std::unique_ptr<T, Deleter> m_store;
};
} // namespace smw::core
#endif // SMW_SAMPLE_H
