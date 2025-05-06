#pragma once

#include <random>
#include <cstdint>

class Random {
private:
	std::mt19937 m_gen;

public:
	Random(): m_gen(std::random_device{}()) {}

	int32_t randInt(int32_t start, int32_t end) {
		std::uniform_int_distribution<uint32_t> int_dist(start, end);
		return int_dist(m_gen);
	}

	float randFloat(float start, float end) {
		std::uniform_real_distribution<float> float_dist(start, end);
		return float_dist(m_gen);
	}
};