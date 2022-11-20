#include "Time.h"

Time* Time::m_Instance;

void Time::Update() {
	Time::GetInstance()->m_LastUpdateTime = Time::GetInstance()->m_CurrentUpdateTime;
	Time::GetInstance()->m_CurrentUpdateTime = std::chrono::steady_clock::now();
}

double Time::GetUnscaledDeltaTimeSeconds() {
	std::chrono::duration<double> elapsed = Time::GetInstance()->m_CurrentUpdateTime - Time::GetInstance()->m_LastUpdateTime;
	return elapsed.count();
}

Time* Time::GetInstance() {
	if (m_Instance == nullptr) {
		m_Instance = new Time();
	}
	return m_Instance;
}