#ifndef __GAMNET_LIB_COROUTINE_H_
#define __GAMNET_LIB_COROUTINE_H_

#include <boost/coroutine2/all.hpp>
namespace Gamnet {

	template <class T>
	class Coroutine {
	public:
		typedef typename boost::coroutines2::coroutine<T>::pull_type Enumerator; // return
		typedef typename boost::coroutines2::coroutine<T>::push_type Yield; //  

		Coroutine(std::function<void(Yield&)> f)
			: enumerator(std::make_shared<Enumerator>(f))
		{
		}

		Coroutine(Coroutine const& other)
			: enumerator(other.enumerator)
		{
		}

		Coroutine(Coroutine&& other) noexcept
		{
			std::swap(enumerator, other.enumerator);
		}

		Enumerator& GetEnumerator()
		{
			return *enumerator;
		}

		Coroutine& operator=(Coroutine const& other)
		{
			enumerator = other.enumerator;
			return *this;
		}

		Coroutine& operator=(Coroutine&& other) noexcept
		{
			std::swap(enumerator, other.enumerator);
			return *this;
		}

		T operator()()
		{
			T t = enumerator->get();
			(*enumerator)();
			return t;
		}

		operator bool() const noexcept
		{
			return (bool)(*enumerator);
		}

		bool operator!() const noexcept
		{
			return !(bool)(*enumerator);
		}
	private:
		std::shared_ptr<Enumerator> enumerator;
	};

	template <>
	class Coroutine<void> {
	public:
		typedef typename boost::coroutines2::coroutine<void>::pull_type Enumerator; // return
		typedef typename boost::coroutines2::coroutine<void>::push_type Yield; //  

		Coroutine(std::function<void(Yield&)> f)
			: enumerator(std::make_shared<Enumerator>(f))
		{
		}

		Coroutine(Coroutine const& other)
			: enumerator(other.enumerator)
		{
		}

		Coroutine(Coroutine&& other) noexcept
		{
			std::swap(enumerator, other.enumerator);
		}

		Enumerator& GetEnumerator()
		{
			return *enumerator;
		}

		Coroutine& operator=(Coroutine const& other)
		{
			enumerator = other.enumerator;
			return *this;
		}

		Coroutine& operator=(Coroutine&& other) noexcept
		{
			std::swap(enumerator, other.enumerator);
			return *this;
		}

		void operator()()
		{
			(*enumerator)();
		}

		operator bool() const noexcept
		{
			return (bool)(*enumerator);
		}

		bool operator!() const noexcept
		{
			return !(bool)(*enumerator);
		}
	private:
		std::shared_ptr<Enumerator> enumerator;
	};

}

#endif