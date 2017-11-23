#ifndef __GAMNET_LIB_DEBUGS_H_
#define __GAMNET_LIB_DEBUGS_H_

namespace Gamnet {

	template<class DERIVED_T, class BASE_T>
	class CheckDerived
	{
		struct No {};
		struct Yes { int c; };

		static No Check(...) { return No(); }
		static Yes Check(BASE_T*) { return Yes(); }

	public:
		enum { ASSERTION = sizeof(Check(static_cast<DERIVED_T*>(0))) == sizeof(Yes) };
	};
}

#define GAMNET_WHERE(derived, base) static_assert(Gamnet::CheckDerived<derived, base>::ASSERTION, #derived " was not derived " #base " class")

#endif 

