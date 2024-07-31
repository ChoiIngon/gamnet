#ifndef _GAMNET_LIB_RETURN_H_
#define _GAMNET_LIB_RETURN_H_

namespace Gamnet
{
template <class T>
struct Return
{
    Return() : error_code(0)
    {
    }
    Return(int error_code) : error_code(error_code)
    {
    }
    Return(int error_code, const T& value) : error_code(error_code), value(value)
    {
    }
    Return(const T& value) : error_code(0), value(value)
    {
    }
    Return(const Return<T>& rhs) : error_code(rhs.error_code), value(rhs.value)
    {
    }
    Return(Return<T>&& rhs) : error_code(rhs.error_code), value(std::move(rhs.value))
    {
    }
    Return<T>& operator=(const Return<T>& rhs)
    {
        error_code = rhs.error_code;
        value = rhs.value;
        return *this;
    }
    Return<T>& operator=(Return<T>&& rhs)
    {
        error_code = rhs.error_code;
        value = std::move(rhs.value);
        return *this;
    }

    operator bool() const
    {
        return 0 == error_code;
    }
    int error_code;
    T value;
};

template <>
struct Return<void>
{
    Return() : error_code(0)
    {
    }
    Return(int error_code) : error_code(error_code)
    {
    }
    Return(const Return<void>& rhs) : error_code(rhs.error_code)
    {
    }
    Return(Return<void>&& rhs) : error_code(rhs.error_code)
    {
    }
    Return<void>& operator=(const Return<void>& rhs)
    {
        error_code = rhs.error_code;
        return *this;
    }
    Return<void>& operator=(Return<void>&& rhs)
    {
        error_code = rhs.error_code;
        return *this;
    }

    operator bool() const
    {
        return 0 == error_code;
    }

    int error_code;
};

}
#endif
