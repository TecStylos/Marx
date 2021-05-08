#include "KeyCodeConverter.h"

#include "Marx/Platform/Win32/Win32KeyCodes.h"
#include "Marx/Platform/GLFW/CrossKeyCodes.h"

namespace Marx
{
	KeyCodeConverter::ToPlatform KeyCodeConverter::s_toWin32;
	KeyCodeConverter::FromPlatform KeyCodeConverter::s_fromWin32;
	KeyCodeConverter::ToPlatform KeyCodeConverter::s_toGLFW;
	KeyCodeConverter::FromPlatform KeyCodeConverter::s_fromGLFW;

	static bool keyCodeConverterInitialized = false;

	void KeyCodeConverter::init()
	{
		MX_CORE_ASSERT(!keyCodeConverterInitialized, "KeyCodeConverter already initialized!");

		for (const KeyPair& pair : Internal::win32Keys)
		{
			if (pair.universal != Key::None && s_toWin32.find(pair.universal) != s_toWin32.end())
				MX_CORE_WARN("Universal key {0} already exists in Win32 scope!", pair.universal);
			s_toWin32.insert(std::make_pair(pair.universal, pair.platformSpecific));
			
			if (pair.platformSpecific != 0x0 && s_fromWin32.find(pair.platformSpecific) != s_fromWin32.end())
				MX_CORE_WARN("Platform key {0} already exists in Win32 scope!", pair.platformSpecific);
			s_fromWin32.insert(std::make_pair(pair.platformSpecific, pair.universal));
		}

		for (const KeyPair& pair : Internal::crossKeys)
		{
			if (pair.universal != Key::None && s_toGLFW.find(pair.universal) != s_toGLFW.end())
				MX_CORE_WARN("Universal key {0} already exists in GLFW scope!", pair.universal);
			s_toGLFW.insert(std::make_pair(pair.universal, pair.platformSpecific));

			if (pair.platformSpecific != 0x0 && s_fromGLFW.find(pair.platformSpecific) != s_fromGLFW.end())
				MX_CORE_WARN("Platform key {0} already exists in GLFW scope!", pair.platformSpecific);
			s_fromGLFW.insert(std::make_pair(pair.platformSpecific, pair.universal));
		}
	}
}