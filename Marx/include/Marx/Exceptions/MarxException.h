#pragma once

#include "Marx/Core.h"

#if !defined(MX_PLATFORM_WINDOWS)
typedef uint32_t DWORD;
#endif

namespace Marx
{
	#define MX_EXCEPT_AUTOLOG() { if (autoLog) MX_CORE_CRITICAL(what()); }

	inline std::string getErrString(DWORD errCode)
	{
		#ifdef MX_PLATFORM_WINDOWS
		LPSTR pBuffer = 0;
		size_t buffSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pBuffer, 0, NULL);

		std::string errString(pBuffer, buffSize);
		LocalFree(pBuffer);
		return errString;
		#else
		return "Not supported";
		#endif
	}

	class MarxException
	{
	public:
		MarxException(int line, const char* file, std::string msg = "No message", bool autoLog = true)
			: m_line(line), m_file(file), m_msg(msg)
		{
			MX_EXCEPT_AUTOLOG();
		}
	public:
		int getLine() const noexcept { return m_line; }
		const char* getFile() const noexcept { return m_file; }
		const char* getMsg() const noexcept { return m_msg.c_str(); }
		virtual const char* getName() const noexcept { return "MarxException"; }
		virtual std::string getInfo() const noexcept { return "No details"; }
		const char* what() const noexcept
		{
			std::stringstream ss;
			ss << getName() << "\n"
				<< "    [ LINE ]  " << getLine() << "\n"
				<< "    [ FILE ]  " << getFile() << "\n"
				<< "    [ MSG  ]  " << getMsg() << "\n"
				<< "    [ INFO ]  " << getInfo();
			m_whatBuff = ss.str();
			return m_whatBuff.c_str();
		}
	private:
		mutable std::string m_whatBuff;
		int m_line;
		const char* m_file;
		std::string m_msg;
	};

	class MarxExceptionInfo : public MarxException
	{
	public:
		MarxExceptionInfo(int line, const char* file, const char* msg, DWORD errCode, bool autoLog = true)
			: MarxException(line, file, msg, false), m_errCode(errCode)
		{
			MX_EXCEPT_AUTOLOG();
		}
	public:
		virtual const char* getName() const noexcept override { return "MarxExceptionInfo"; }
		virtual std::string getInfo() const noexcept override { return getErrString(m_errCode); }
	protected:
		DWORD m_errCode;
	};
}