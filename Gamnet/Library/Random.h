#ifndef GAMNET_RANDOM_H
#define GAMNET_RANDOM_H

#include <random>
#include <thread>
#include <stdexcept>

#undef max
namespace Gamnet {
	class MT19937Wrapper {
	public :
		MT19937Wrapper();
		~MT19937Wrapper();

		std::mt19937     engine;
		std::mt19937_64  engine64;
	};

	class Random
	{
	public:
		// get random number from min(include) to max(include)
		static int32_t Range(int32_t min, int32_t max);
		static uint32_t Range(uint32_t min, uint32_t max);
		static int64_t Range(int64_t min, int64_t max);
		static uint64_t Range(uint64_t min, uint64_t max);
		static float Range(float min, float max);
		static double Range(double min, double max);
	};
}

#endif
