#pragma once

#include <chrono>

class Time {
public:
	static double GetUnscaledDeltaTimeSeconds();
	static double GetCurrentTimeSeconds();
	static void Update();

private:
	static Time* GetInstance();
	static Time* m_Instance;

	std::chrono::steady_clock::time_point m_LastUpdateTime;
	std::chrono::steady_clock::time_point m_CurrentUpdateTime;
};
