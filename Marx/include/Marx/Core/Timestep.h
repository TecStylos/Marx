#pragma once

#include <chrono>

namespace Marx
{
	class Timestep
	{
	public:
		/*
		* Constructor of Timestep
		* 
		* @param time The current time in seconds
		*/
		Timestep(float time)
			: m_currTime(time), m_diffTime(0.0000001f)
		{}
	public:
		/*
		* Returns the time difference between the last two updates.
		* 
		* @returns Time difference in seconds
		*/
		operator float() const { return m_diffTime; }
	public:
		/*
		* Sets a new timepoint and recalculates the time difference.
		* 
		* @param newTime New time to set (Usually the current time)
		*/
		inline void update(float newTime) { m_diffTime = newTime - m_currTime; m_currTime = newTime; }
		/*
		* Returns the time set with the last call to Timestep::update.
		* 
		* @returns Timepoint in seconds
		*/
		inline float getSeconds() const { return m_currTime; }
		/*
		* Returns the time set with the last call to Timestep::update.
		* 
		* @returns Timepoint in milliseconds.
		*/
		inline float getMilliseconds() const { return getSeconds() * 1000.0f; }
		/*
		* Returns the time difference between the last two updates.
		* 
		* @returns Time difference in seconds
		*/
		inline float getPassedSeconds() const { return m_diffTime; }
		/*
		* Returns the time difference between the last two updates.
		*
		* @returns Time difference in milliseconds
		*/
		inline float getPassedMilliseconds() const { return getPassedSeconds() * 1000.0f; }
	public:
		/*
		* Returns the time passed since the program start.
		* 
		* @returns Passed time in seconds.
		*/
		inline static float secondsSinceProgramStartup() { return std::chrono::duration<float>(std::chrono::steady_clock::now() - s_startup).count(); }
	private:
		float m_currTime;
		float m_diffTime;
		inline static std::chrono::steady_clock::time_point s_startup = std::chrono::steady_clock::now();
	};
}