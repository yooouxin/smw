//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_SAMPLE_PTR_H
#define SMW_SAMPLE_PTR_H
#include <memory>

namespace smw::core
{
template <typename T>
class SamplePtr
{
    class Deleter
    {
      public:
        using delete_function_t = std::function<void(T*)>;

        Deleter()
            : m_delete_function([](T* ptr) { delete ptr; })
        {
        }

        explicit Deleter(const delete_function_t& f)
            : m_delete_function(f)
        {
        }

        void operator()(T* ptr) const
        {
            m_delete_function(ptr);
        }

      private:
        delete_function_t m_delete_function;
    };

    std::unique_ptr<T, Deleter> m_data_ptr;
    bool m_is_heap_allocated{true};

  public:
    SamplePtr() noexcept
        : m_data_ptr(nullptr)
    {
    }

    /* constexpr unique_ptr::unique_ptr(std::nullptr_t) noexcept; */
    constexpr SamplePtr(std::nullptr_t) noexcept
        : SamplePtr()
    {
    }

    /* template<class Y>shared_ptr(Y* ptr) ctor can throw */
    explicit SamplePtr(T* ptr)
        : m_data_ptr(ptr)
    {
    }

    explicit SamplePtr(T* ptr, const typename Deleter::delete_function_t& deleteFunction)
        : m_data_ptr(ptr, Deleter(deleteFunction))
    {
    }

    /* unique_ptr::unique_ptr(const unique_ptr& r) noexcept; */
    SamplePtr(const SamplePtr<T>&) noexcept = delete;

    /* shared_ptr( const shared_ptr&& r ) noexcept; */
    SamplePtr(SamplePtr<T>&& other) noexcept
        : m_data_ptr(std::move(other.m_data_ptr))
        , m_is_heap_allocated(std::move(other.m_is_heap_allocated))
    {
    }

    /* unique_ptr& unique_ptr::operator=(std::nullptr_t) noexcept; */
    SamplePtr& operator=(std::nullptr_t) noexcept
    {
        m_data_ptr = nullptr;
        return *this;
    }

    /* unique_ptr& unique_ptr::operator=(const unique_ptr& r) noexcept; */
    SamplePtr& operator=(const SamplePtr<T>&) noexcept = delete;

    /* shared_ptr& operator=( shared_ptr&& r ) noexcept; */
    SamplePtr& operator=(SamplePtr<T>&& other) noexcept
    {
        m_data_ptr = std::move(other.m_data_ptr);
        m_is_heap_allocated = std::move(other.m_is_heap_allocated);
        return *this;
    }

    ~SamplePtr() noexcept = default;

    /* void unique_ptr::swap(unique_ptr&) noexcept; */
    void swap(SamplePtr& other) noexcept
    {
        using std::swap;
        swap(m_data_ptr, other.m_data_ptr);
        swap(m_is_heap_allocated, other.m_is_heap_allocated);
    }

    /* void unique_ptr::reset(std::nullptr_t) noexcept; */
    void reset(std::nullptr_t) noexcept
    {
        m_data_ptr.reset();
    }

    T* release() noexcept
    {
        return m_data_ptr.release();
    }

    /* explicit unique_ptr::operator bool() const noexcept; */
    explicit operator bool() const noexcept
    {
        return bool(m_data_ptr);
    }

    /* T& shared_ptr::operator*() const noexcept; */
    T& operator*() const noexcept
    {
        return m_data_ptr.operator*();
    }

    /* T* shared_ptr::operator->() const noexcept; */
    T* operator->() const noexcept
    {
        return m_data_ptr.operator->();
    }

    /* T* unique_ptr::get() const noexcept; */
    T* get() const noexcept
    {
        return m_data_ptr.get();
    }

    void markAsNoHeapAllocated() noexcept
    {
        m_is_heap_allocated = false;
    }

    bool isHeapAllocated() const noexcept
    {
        return m_is_heap_allocated;
    }
};

template <typename T, typename... Args>
SamplePtr<T> makeSamplePtr(Args&&... args)
{
    return SamplePtr<T>(new T(std::forward<Args>(args)...));
}

} // namespace smw::core
#endif // SMW_SAMPLE_PTR_H
