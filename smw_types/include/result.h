//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_RESULT_H
#define SMW_RESULT_H

#include <functional>
#include <variant>

namespace smw::types
{
template <typename T = void>
struct Ok
{
    /// @brief constructor which creates a Ok helper class by copying
    ///         the value of t
    /// @param[in] t value which should be later stored in an expected
    Ok(const T& t) noexcept
        : value(t)
    {
    }

    Ok(T&& t) noexcept
        : value(std::forward<T>(t))
    {
    }

    template <typename... Targs>
    Ok(Targs... args) noexcept
        : value(std::forward<Targs>(args)...)
    {
    }
    T value;
};


/// @brief helper struct to create an error only expected which is signalling Ok more easily
template <>
struct Ok<void>
{
};

template <typename T>
struct Err
{
    /// @brief constructor which creates a error helper class by copying
    ///         the value of t
    /// @param[in] t value which should be later stored in an expected
    Err(const T& t) noexcept
        : value(t)
    {
    }

    /// @brief constructor which creates a error helper class by moving
    ///         the value of t
    /// @param[in] t value which should be later moved into an expected
    Err(T&& t) noexcept
        : value(std::forward<T>(t))
    {
    }

    /// @brief constructor which creates a error helper class by forwarding
    ///         arguments to the constructor of T
    /// @param[in] args... arguments which will be perfectly forwarded to the
    ///                     constructor
    template <typename... Targs>
    Err(Targs&&... args) noexcept
        : value(std::forward<Targs>(args)...)
    {
    }

    T value;
};


template <typename... T>
class [[nodiscard]] Result;


/// @brief result contained only error
/// @tparam ErrorType
template <typename ErrorType>
class [[nodiscard]] Result<ErrorType>
{
  public:
    /// @brief default ctor is deleted since you have to clearly state if the
    ///         expected contains a success value or an error value
    Result() = delete;

    ~Result() noexcept = default;

    /// @brief copy ctor is default since just need copying contained Ok or Err
    ///         value
    Result(const Result&) noexcept = default;

    /// @brief copy assignment is default since just need copy contained Ok or Err
    ///         value
    /// @return reference of result
    Result& operator=(const Result&) noexcept = default;

    /// @brief move ctor default
    Result(Result&& rsh) noexcept = default;

    /// @brief move assignment
    /// @return
    Result& operator=(Result&&) noexcept = default;


    /// @brief construct result instance with Ok value
    /// @param value
    Result(const Ok<void>& value) noexcept
        : m_has_error(false)
    {
    }

    /// @brief construct result instance with Ok value
    /// @param value
    Result(Ok<void>&& value) noexcept
        : m_has_error(false)
    {
    }

    /// @brief  constructs an Result which is signaling an error and stores the
    ///         error value provided by error
    Result(const Err<ErrorType>& error) noexcept
        : m_store(std::in_place_index<ERROR_INDEX>, error.value)
        , m_has_error(true)
    {
    }

    /// @brief  constructs an Result which is signaling an error and stores the
    ///         error value provided by error
    Result(Err<ErrorType>&& error) noexcept
        : m_store(std::in_place_index<ERROR_INDEX>, std::move(error.value))
        , m_has_error(true)
    {
    }

    /// @brief return if Result has error
    bool hasError() const noexcept
    {
        return m_has_error;
    }

    /// @brief  returns a reference to the contained error value, if the Result
    ///         does not contain an error ,program will exit
    /// @return reference to the internally contained error
    ErrorType& getError() & noexcept
    {
        return std::get<ERROR_INDEX>(m_store);
    }

    /// @brief  returns a const reference to the contained error value, if the Result
    ///         does not contain an error ,program will exit
    /// @return const reference to the internally contained error
    const ErrorType& getError() const& noexcept
    {
        return std::get<ERROR_INDEX>(m_store);
    }

    /// @brief  returns a rvalue reference to the contained error value, if the Result
    ///         does not contain an error ,program will exit
    /// @return rvalue reference to the internally contained error
    ErrorType&& getError() && noexcept
    {
        return std::move(std::get<ERROR_INDEX>(m_store));
    }


    /// @brief call user's callable if contained Ok value
    /// @param callable user's callable
    /// @return reference of the same Result
    Result& andThen(const std::function<void()>& callable) noexcept
    {
        if (!hasError() && callable)
        {
            callable();
        }

        return *this;
    }

    /// @brief call user's callable if contained Err value
    /// @param callable
    /// @return reference of the same Result
    Result& orElse(const std::function<void(ErrorType&)>& callable) noexcept
    {
        if (hasError() && callable)
        {
            callable(getError());
        }

        return *this;
    }

  private:
    bool m_has_error;
    std::variant<ErrorType> m_store;
    static constexpr std::size_t ERROR_INDEX = 0;
};

/// @brief result contained value or error
/// @tparam ValueType
/// @tparam ErrorType
template <typename ValueType, typename ErrorType>
class [[nodiscard]] Result<ValueType, ErrorType>
{
  public:
    /// @brief default ctor is deleted since you have to clearly state if the
    ///         expected contains a success value or an error value
    Result() = delete;

    ~Result() noexcept = default;

    /// @brief copy ctor is default since just need copying contained Ok or Err
    ///         value
    Result(const Result&) noexcept = default;

    /// @brief copy assignment is default since just need copy contained Ok or Err
    ///         value
    /// @return reference of result
    Result& operator=(const Result&) noexcept = default;

    /// @brief move ctor default
    Result(Result&& rsh) noexcept = default;

    /// @brief move assignment
    /// @return
    Result& operator=(Result&&) noexcept = default;


    /// @brief construct result instance with Ok value
    /// @param value
    Result(const Ok<ValueType>& value) noexcept
        : m_store(std::in_place_index<VALUE_INDEX>, value.value)
        , m_has_error(false)
    {
    }

    /// @brief construct result instance with Ok value
    /// @param value
    Result(Ok<ValueType>&& value) noexcept
        : m_store(std::in_place_index<VALUE_INDEX>, std::move(value.value))
    {
    }

    /// @brief  constructs an Result which is signaling an error and stores the
    ///         error value provided by error
    Result(const Err<ErrorType>& error) noexcept
        : m_store(std::in_place_index<ERROR_INDEX>, error.value)
        , m_has_error(true)
    {
    }

    /// @brief  constructs an Result which is signaling an error and stores the
    ///         error value provided by error
    Result(Err<ErrorType>&& error) noexcept
        : m_store(std::in_place_index<ERROR_INDEX>, std::move(error.value))
        , m_has_error(true)
    {
    }

    /// @brief return if Result has error
    bool hasError() const noexcept
    {
        return m_has_error;
    }

    /// @brief  returns a reference to the contained error value, if the Result
    ///         does not contain an error this is undefined behavior
    /// @return reference to the internally contained error
    ErrorType& getError() & noexcept
    {
        return std::get<ERROR_INDEX>(m_store);
    }

    /// @brief  returns a const reference to the contained error value, if the Result
    ///         does not contain an error this is undefined behavior
    /// @return const reference to the internally contained error
    const ErrorType& getError() const& noexcept
    {
        return std::get<ERROR_INDEX>(m_store);
    }

    /// @brief  returns a rvalue reference to the contained error value, if the Result
    ///         does not contain an error this is undefined behavior
    /// @return rvalue reference to the internally contained error
    ErrorType&& getError() && noexcept
    {
        return std::move(std::get<ERROR_INDEX>(m_store));
    }

    /// @brief  returns a reference to the contained success value, if the Result
    ///         does not contain a Ok value this is undefined behavior
    /// @return reference to the internally contained value
    ValueType& getValue() & noexcept
    {
        return std::get<VALUE_INDEX>(m_store);
    }

    /// @brief  returns a const reference to the contained success value, if the Result
    ///         does not contain a Ok value this is undefined behavior
    /// @return const reference to the internally contained value
    const ValueType& getValue() const& noexcept
    {
        return std::get<VALUE_INDEX>(m_store);
    }

    /// @brief  returns a reference to the contained success value, if the Result
    ///         does not contain a success value this is undefined behavior
    /// @return rvalue reference to the internally contained value
    ValueType&& getValue() && noexcept
    {
        return std::move(std::get<VALUE_INDEX>(m_store));
    }

    /// @brief call user's callable if contained Ok value
    /// @param callable
    /// @return reference of the same Result
    Result& andThen(const std::function<void(ValueType&)>& callable) noexcept
    {
        if (!hasError() && callable)
        {
            callable(getValue());
        }

        return *this;
    }

    /// @brief call user's callable if contained Err value
    /// @param callable
    /// @return reference of the same Result
    Result& orElse(const std::function<void(ErrorType&)>& callable) noexcept
    {
        if (hasError() && callable)
        {
            callable(getError());
        }

        return *this;
    }

  private:
    bool m_has_error;
    std::variant<ValueType, ErrorType> m_store;
    static constexpr std::size_t VALUE_INDEX = 0;
    static constexpr std::size_t ERROR_INDEX = 1;
};
} // namespace smw::types
#endif // SMW_RESULT_H
