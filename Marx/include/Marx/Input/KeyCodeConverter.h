#pragma once

#include <unordered_map>

#include "KeyCodes.h"
#include "Marx/Platform/Win32/Win32KeyCodes.h"

namespace Marx
{
	class KeyCodeConverter
	{
	public:
		static void init();
	public:
		static inline Key fromWin32(uint16_t key) { return s_fromWin32[key]; }
		static inline uint16_t toWin32(Key key) { return s_toWin32[key]; }
	
		static inline Key fromGlfw(uint16_t key) { return Key::None; }
		static inline uint16_t toGlfw(Key key) { return 0; }
	private:
		typedef std::unordered_map<Key, uint16_t> ToPlatform;
		typedef std::unordered_map<uint16_t, Key> FromPlatform;

		static ToPlatform s_toWin32;
		static FromPlatform s_fromWin32;
	};
}