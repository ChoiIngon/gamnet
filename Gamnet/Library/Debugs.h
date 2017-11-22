#ifndef __GAMNET_LIB_DEBUGS_H_
#define __GAMNET_LIB_DEBUGS_H_

namespace Gamnet {

	// derived 가 base 에서 상속된 클래스면 check_derived::assertion 이 true
	template<typename derived, typename base> struct check_derived
	{
		static struct No {} Check(...);
		static struct Yes { int c; } Check(base*);

	public:
		enum { assertion = sizeof(Check(static_cast<derived*>(0))) == sizeof(Yes) };
	};

#define GAMNET_WHERE(derived, base) static_assert(Gamnet::check_derived<derived, base>::assertion, #derived " was not derived " #base " class")

}
#endif 

