#pragma once
#include <random>

namespace math
{
	// TODO: test this on different types like int
	template<typename T = float>
	float Random(T min = 0, T max = 1)
	{
		static std::random_device rd;  // seeds once
		static std::mt19937 gen(rd()); // Mersenne Twister engine
		std::uniform_real_distribution<T> dist(min, max);
		return dist(gen);
	}
}



