#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <coroutine>

template <class ReturnType, class SUSPEND = std::suspend_always>
class Coroutine 
{
    struct promise_type_base
    {
        SUSPEND initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() 
        {
            throw std::exception("unhandled_exception");
        }
    };

    template <class R>
    struct promise_type_t : public promise_type_base
    {
        R value;

        Coroutine get_return_object()
        {
            return Coroutine{ std::coroutine_handle<promise_type_t<R>::from_promise(*this) };
        }

        std::suspend_always yield_value(R&& value)
        {
            this->value = std::move(value);
            return {};
        }

        std::suspend_always yield_value(const R& value)
        {
            this->value = value;
            return {};
        }

        void return_value(R&& value)
        {
            this->value = std::move(value);
        }

        void return_value(const R& value)
        {
            this->value = value;
        }
    };

    template <>
    struct promise_type_t<void> : public promise_type_base
    {
        Coroutine get_return_object()
        {
            return Coroutine{ std::coroutine_handle<promise_type_t<void>>::from_promise(*this) };
        }

        void return_void()
        {
        }
    };
    
public :
    typedef promise_type_t<typename ReturnType> promise_type;

    Coroutine()
        : handle(nullptr)
    {
    }

    Coroutine(std::coroutine_handle<promise_type> handle)
        : handle(handle)
    {
    }

    Coroutine(const Coroutine& other) = delete;
    Coroutine& operator=(const Coroutine& other) = delete;

    bool operator()() const
    {
        return resume();
    }

    bool resume() const
    {
        if (true == done())
        {
            return false;
        }

        handle.resume();

        return true;
    }

    promise_type& promise()
    {
        return handle.promise();
    }

    bool done() const
    {
        return handle.done();
    }

    std::coroutine_handle<promise_type> get_handle() const
    {
        return handle;
    }

    struct iterator
    {
        explicit iterator(Coroutine* coroutine)
            : coroutine(coroutine)
            , done(true)
        {
        }

        const ReturnType& operator*() const
        {
            return coroutine->promise().value;
        }

        iterator& operator++()
        {
            done = coroutine->resume();
            return *this;
        }

        bool operator == (std::default_sentinel_t) const
        {
            if (true == done && true == coroutine->done())
            {
                return true;
            }
            return false;
        }
    private:
        Coroutine* coroutine;
        bool done;
    };

    iterator begin()
    {
        if(nullptr == handle)
        {
            return iterator(nullptr);
        }

        if(nullptr != handle)
        {
            handle.resume();
        }

        return iterator(this);
    }

    std::default_sentinel_t end()
    {
        return {};
    }

private :
    std::coroutine_handle<promise_type> handle;
};

#endif

