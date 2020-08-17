#ifndef GAMNET_RANDOM_H
#define GAMNET_RANDOM_H

#include <random>
#include <thread>
#include <vector>
#include <list>
#include <limits>

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
		//static uint32_t Range(uint32_t min, uint32_t max);
		static int64_t Range(int64_t min, int64_t max);
		//static uint64_t Range(uint64_t min, uint64_t max);
		static double Range(double min, double max);
	};

	template <class T>
	class WeightRandom
	{
		const uint32_t MAX_WEIGHT_CONSTRAINT = std::numeric_limits<uint32_t>::max();
		uint32_t totalWeight_;
		std::vector<std::pair<uint32_t, T>> valWeight_;
	public :
		WeightRandom() : totalWeight_(0) {};
		~WeightRandom() {};

		void SetWeight(const T&& value, uint32_t weight)
		{
			if (0 == weight)
			{
				return;
			}
			totalWeight_ += weight;
			if (MAX_WEIGHT_CONSTRAINT < totalWeight_)
			{
				throw std::overflow_error("total weight was over limit");
			}
			valWeight_.push_back(std::make_pair(totalWeight_, value));
		}

		void SetWeight(const T& value, uint32_t weight)
		{
			if (0 == weight)
			{
				return;
			}
			totalWeight_ += weight;
			if (MAX_WEIGHT_CONSTRAINT < totalWeight_)
			{
				throw std::overflow_error("total weight was over limit");
			}
			valWeight_.push_back(std::make_pair(totalWeight_, value));
		}

		const T& Random() const
		{
			if (0 == valWeight_.size())
			{
				throw std::underflow_error("no weight info");
			}

			uint32_t r = Random::Range(0, totalWeight_ - 1);
			for (const auto& itr : valWeight_)
			{
				if (r < itr.first)
				{
					return itr.second;
				}
			}
			return valWeight_.back().second;
		}

		uint32_t GetTotalWeight() const
		{
			return totalWeight_;
		}

		std::list<T> GetAllRandomEntity() const
		{
			std::list<T> entities;
			for(const auto& itr : valWeight_)
			{
				entities.push_back(itr.second);
			}
			return entities;
		}
	};
}

#endif
