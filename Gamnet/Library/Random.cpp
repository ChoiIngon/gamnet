#include "Random.h"
#include <iostream>

namespace Gamnet {
	MT19937Wrapper::MT19937Wrapper() 
	{
		std::random_device rd;
		engine.seed(rd());
		engine64.seed(rd());
	}

	MT19937Wrapper::~MT19937Wrapper() 
	{
	}

	static thread_local MT19937Wrapper mt;

	int32_t Random::Range(int32_t min, int32_t max)
	{
		std::uniform_int_distribution<int32_t> dist(min, max);
		return (int32_t)dist(mt.engine);
	}
	/*
	uint32_t Random::Range(uint32_t min, uint32_t max)
	{
		std::uniform_int_distribution<uint32_t> dist(min, max);
		return (uint32_t)dist(mt.engine);
	}
	*/
	int64_t Random::Range(int64_t min, int64_t max)
	{
		std::uniform_int_distribution<int64_t> dist(min, max);
		return (int64_t)dist(mt.engine64);
	}
	/*
	uint64_t Random::Range(uint64_t min, uint64_t max)
	{
		std::uniform_int_distribution<uint64_t> dist(min, max);
		return (uint64_t)dist(mt.engine64);
	}
	*/
	double Random::Range(double min, double max)
	{
		std::uniform_real_distribution<double> dist(min, max);
		return (double)dist(mt.engine);
	}
}