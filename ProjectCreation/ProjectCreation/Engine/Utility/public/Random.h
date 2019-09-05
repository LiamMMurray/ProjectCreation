#pragma once
#include <random>
inline static std::random_device random_device_0;
inline static std::mt19937       default_random_engine(random_device_0());