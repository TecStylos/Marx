#pragma once

#include <chrono>
#include <mutex>
#include <fstream>
#include <algorithm>

#ifdef MX_ENABLE_PROFILING

#define MX_PROFILER_BEGIN_SESSION(filename) ::Marx::Profiler::get().beginSession(filename)
#define MX_PROFILER_END_SESSION() ::Marx::Profiler::get().endSession()
#define MX_PROFILE_SCOPE(name) ::Marx::ProfileTimer profileTimer(name)
#define MX_PROFILE_FUNCTION() MX_PROFILE_SCOPE(__FUNCSIG__)

#else

#define MX_PROFILER_BEGIN_SESSION(filename)
#define MX_PROFILER_END_SESSION()
#define MX_PROFILE_SCOPE(name)
#define MX_PROFILE_FUNCTION()

#endif

namespace Marx
{
	struct Profile
	{
		std::string name;
		long long begin;
		long long end;
		uint32_t tID;
	};

	class Profiler
	{
	public:
		inline void beginSession(const std::string& filename)
		{
			m_profileCount = 0;
			m_file.open(filename, std::ios::out | std::ios::trunc);
			writeHeader();
		}

		inline void endSession()
		{
			writeFooter();
			m_file.close();
		}

		inline void writeHeader()
		{
			m_file << "{ ";
			m_file << "\"displayTimeUnit\": \"ms\", ";
			m_file << "\"systemTraceEvents\": \"SystemTraceData\", ";
			m_file << "\"otherData\": {}, ";
			m_file << "\"traceEvents\": [";
		}

		inline void writeFooter()
		{
			m_file << "]}";
		}

		inline void writeProfile(Profile profile)
		{
			std::lock_guard lock(m_mtxFile);

			if (m_profileCount++ > 0)
				m_file << ",";

			std::replace(profile.name.begin(), profile.name.end(), '"', '\'');

			m_file << "{ ";
			m_file << "\"cat\": \"function\", ";
			m_file << "\"name\": \"" << profile.name << "\", ";
			m_file << "\"ts\": " << profile.begin << ", ";
			m_file << "\"dur\": " << (profile.end - profile.begin) << ", ";
			m_file << "\"ph\": \"X\", ";
			m_file << "\"pid\": 0, ";
			m_file << "\"tid\": " << profile.tID;
			m_file << "} ";

			m_file.flush();
		}
	public:
		static inline Profiler& get()
		{
			static Profiler s_profiler;
			return s_profiler;
		}
	private:
		uint32_t m_profileCount;
		std::mutex m_mtxFile;
		std::fstream m_file;
	};

	class ProfileTimer
	{
	public:
		ProfileTimer(const std::string& name = "ProfileTimer")
			: m_name(name), m_begin(std::chrono::high_resolution_clock::now())
		{}
		ProfileTimer(const ProfileTimer&) = delete;
		ProfileTimer(ProfileTimer&&) = delete;
		~ProfileTimer()
		{
			if (!m_stopped)
				stop();
		}
	public:
		void stop()
		{
			auto end = std::chrono::high_resolution_clock::now();

			Profile profile;
			profile.name = m_name;
			profile.tID = (uint32_t)std::hash<std::thread::id>{}(std::this_thread::get_id());
			profile.begin = std::chrono::time_point_cast<std::chrono::microseconds>(m_begin).time_since_epoch().count();
			profile.end = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();
			Profiler::get().writeProfile(profile);

			m_stopped = true;
		}
	private:
		bool m_stopped = false;
		std::string m_name;
		std::chrono::high_resolution_clock::time_point m_begin;
	};
}