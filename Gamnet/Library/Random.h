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
		static uint32_t Range(uint32_t min, uint32_t max);
		static int64_t Range(int64_t min, int64_t max);
		static uint64_t Range(uint64_t min, uint64_t max);
		static float Range(float min, float max);
		static double Range(double min, double max);
	};

	template <class T, int WEIGHT_LIMIT = std::numeric_limits<int>::max()>
	class WeightRandom
	{
		int total_weight;
		std::vector<std::pair<int, T>> weighted_values;
	public :
		WeightRandom() : total_weight(0) {};
		~WeightRandom() {};

		void SetWeight(const T&& value, uint32_t weight)
		{
			if (0 == weight)
			{
				return;
			}
			total_weight += weight;
			if (WEIGHT_LIMIT < total_weight)
			{
				throw std::overflow_error("total weight was over limit");
			}
			weighted_values.push_back(std::make_pair(total_weight, value));
		}

		void SetWeight(const T& value, uint32_t weight)
		{
			if (0 == weight)
			{
				return;
			}
			total_weight += weight;
			if (WEIGHT_LIMIT < total_weight)
			{
				throw std::overflow_error("total weight was over limit");
			}
			weighted_values.push_back(std::make_pair(total_weight, value));
		}

		const T& Random() const
		{
			if (0 == weighted_values.size())
			{
				throw std::underflow_error("no weight info");
			}

			int weight = Random::Range(0, total_weight - 1);
			for (const auto& itr : weighted_values)
			{
				if (weight < itr.first)
				{
					return itr.second;
				}
			}
			return weighted_values.back().second;
		}

		int GetTotalWeight() const
		{
			return total_weight;
		}

		std::list<T> GetAllRandomEntity() const
		{
			std::list<T> entities;
			for(const auto& itr : weighted_values)
			{
				entities.push_back(itr.second);
			}
			return entities;
		}
	};
}

#endif
