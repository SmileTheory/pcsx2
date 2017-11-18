#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <thread>

namespace shoryu
{

#define repeat(N) for(decltype (N) i = 0; i < N; i++)
#define MS_IN_SEC 1000;

	typedef int64_t msec;

	inline void sleep( uint32_t msec ) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(msec));
	}

	inline msec time_ms() {
		//return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		return GetCPUTicks() / (GetTickFrequency() / 1000);
	}
}
