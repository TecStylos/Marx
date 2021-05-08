#pragma once

#include <unordered_map>

#include "KeyCodes.h"

namespace Marx
{
	class KeyCodeConverter
	{
	public:
		static void init();
	public:
		static inline Key fromWin32(uint16_t key) { return s_fromWin32[key]; }
		static inline uint16_t toWin32(Key key) { return s_toWin32[key]; }
	
		static inline Key fromGlfw(uint16_t key) { return s_fromGLFW[key]; }
		static inline uint16_t toGlfw(Key key) { return s_toGLFW[key]; }
	private:
		typedef std::unordered_map<Key, uint16_t> ToPlatform;
		typedef std::unordered_map<uint16_t, Key> FromPlatform;

		static ToPlatform s_toWin32;
		static FromPlatform s_fromWin32;
		static ToPlatform s_toGLFW;
		static FromPlatform s_fromGLFW;
	};
}