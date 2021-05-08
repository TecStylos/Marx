#pragma once

#include "Marx/Input/KeyCodes.h"

namespace Marx
{
	namespace Internal
	{
		const KeyPair win32Keys[] = {
			{ Key::None, 0x00 },
			{ Key::None, 0x01 },
			{ Key::None, 0x02 },
			{ Key::Cancel, 0x03},
			{ Key::None, 0x04 },
			{ Key::None, 0x05 },
			{ Key::None, 0x06 },
			{ Key::None, 0x07 },
			{ Key::Backspace, 0x08 },
			{ Key::Tab, 0x09 },
			{ Key::None, 0x0A },
			{ Key::None, 0x0B },
			{ Key::Clear, 0x0C },
			{ Key::Return, 0x0D },
			{ Key::None, 0x0E },
			{ Key::None, 0x0F },
			{ Key::Shift, 0x10 },
			{ Key::Control, 0x11 },
			{ Key::Menu, 0x12 },
			{ Key::Pause, 0x13 },
			{ Key::Capital, 0x14},
			{ Key::None, 0x15 },
			{ Key::None, 0x16 },
			{ Key::None, 0x17 },
			{ Key::None, 0x18 },
			{ Key::None, 0x19 },
			{ Key::None, 0x1A },
			{ Key::Escape, 0x1B },
			{ Key::Convert, 0x1C},
			{ Key::NonConvert, 0x1D },
			{ Key::Accept, 0x1E },
			{ Key::ModeChange, 0x1F },
			{ Key::Space, 0x20 },
			{ Key::Prior, 0x21 },
			{ Key::Next, 0x22 },
			{ Key::End, 0x23 },
			{ Key::Home, 0x24 },
			{ Key::Left, 0x25 },
			{ Key::Up, 0x26 },
			{ Key::Right, 0x27 },
			{ Key::Down, 0x28 },
			{ Key::Select, 0x29 },
			{ Key::Print, 0x2A },
			{ Key::Execute, 0x2B },
			{ Key::Snapshot, 0x2C },
			{ Key::Insert, 0x2D },
			{ Key::Delete, 0x2E },
			{ Key::Help, 0x2F },
			{ Key::_0, 0x30 },
			{ Key::_1, 0x31 },
			{ Key::_2, 0x32 },
			{ Key::_3, 0x33 },
			{ Key::_4, 0x34 },
			{ Key::_5, 0x35 },
			{ Key::_6, 0x36 },
			{ Key::_7, 0x37 },
			{ Key::_8, 0x38 },
			{ Key::_9, 0x39 },
			{ Key::None, 0x3A },
			{ Key::None, 0x3B },
			{ Key::None, 0x3C },
			{ Key::None, 0x3D },
			{ Key::None, 0x3E },
			{ Key::None, 0x3F },
			{ Key::None, 0x40 },
			{ Key::A, 0x41 },
			{ Key::B, 0x42 },
			{ Key::C, 0x43 },
			{ Key::D, 0x44 },
			{ Key::E, 0x45 },
			{ Key::F, 0x46 },
			{ Key::G, 0x47 },
			{ Key::H, 0x48 },
			{ Key::I, 0x49 },
			{ Key::J, 0x4A },
			{ Key::K, 0x4B },
			{ Key::L, 0x4C },
			{ Key::M, 0x4D },
			{ Key::N, 0x4E },
			{ Key::O, 0x4F },
			{ Key::P, 0x50 },
			{ Key::Q, 0x51 },
			{ Key::R, 0x52 },
			{ Key::S, 0x53 },
			{ Key::T, 0x54 },
			{ Key::U, 0x55 },
			{ Key::V, 0x56 },
			{ Key::W, 0x57 },
			{ Key::X, 0x58 },
			{ Key::Y, 0x59 },
			{ Key::Z, 0x5A },
			{ Key::LWin, 0x5B },
			{ Key::RWin, 0x5C },
			{ Key::Apps, 0x5D },
			{ Key::None, 0x5E },
			{ Key::Sleep, 0x5F },
			{ Key::Num_0, 0x60 },
			{ Key::Num_1, 0x61 },
			{ Key::Num_2, 0x62 },
			{ Key::Num_3, 0x63 },
			{ Key::Num_4, 0x64 },
			{ Key::Num_5, 0x65 },
			{ Key::Num_6, 0x66 },
			{ Key::Num_7, 0x67 },
			{ Key::Num_8, 0x68 },
			{ Key::Num_9, 0x69 },
			{ Key::Multiply, 0x6A },
			{ Key::Add, 0x6B },
			{ Key::Separator, 0x6C },
			{ Key::Subtract, 0x6D },
			{ Key::Decimal, 0x6E },
			{ Key::Divide, 0x6F },
			{ Key::F1, 0x70 },
			{ Key::F2, 0x71 },
			{ Key::F3, 0x72 },
			{ Key::F4, 0x73 },
			{ Key::F5, 0x74 },
			{ Key::F6, 0x75 },
			{ Key::F7, 0x76 },
			{ Key::F8, 0x77 },
			{ Key::F9, 0x78 },
			{ Key::F10, 0x79 },
			{ Key::F11, 0x7A },
			{ Key::F12, 0x7B },
			{ Key::F13, 0x7C },
			{ Key::F14, 0x7D },
			{ Key::F15, 0x7E },
			{ Key::F16, 0x7F },
			{ Key::F17, 0x80 },
			{ Key::F18, 0x81 },
			{ Key::F19, 0x82 },
			{ Key::F20, 0x83 },
			{ Key::F21, 0x84 },
			{ Key::F22, 0x85 },
			{ Key::F23, 0x86 },
			{ Key::F24, 0x87 },
			{ Key::None, 0x88 },
			{ Key::None, 0x89 },
			{ Key::None, 0x8A },
			{ Key::None, 0x8B },
			{ Key::None, 0x8C },
			{ Key::None, 0x8D },
			{ Key::None, 0x8E },
			{ Key::None, 0x8F },
			{ Key::Numlock, 0x90 },
			{ Key::Scroll, 0x91 },
			{ Key::None, 0x92 },
			{ Key::None, 0x93 },
			{ Key::None, 0x94 },
			{ Key::None, 0x95 },
			{ Key::None, 0x96 },
			{ Key::None, 0x97 },
			{ Key::None, 0x98 },
			{ Key::None, 0x99 },
			{ Key::None, 0x9A },
			{ Key::None, 0x9B },
			{ Key::None, 0x9C },
			{ Key::None, 0x9D },
			{ Key::None, 0x9E },
			{ Key::None, 0x9F },
			{ Key::LShift, 0xA0 },
			{ Key::RShift, 0xA1 },
			{ Key::LControl, 0xA2 },
			{ Key::RControl, 0xA3 },
			{ Key::LMenu, 0xA4 },
			{ Key::RMenu, 0xA5 },
			{ Key::Browser_Back, 0xA6 },
			{ Key::Browser_Forward, 0xA7 },
			{ Key::Browser_Refresh, 0xA8 },
			{ Key::Browser_Stop, 0xA9 },
			{ Key::Browser_Search, 0xAA },
			{ Key::Browser_Favorites, 0xAB },
			{ Key::Browser_Home, 0xAC },
			{ Key::Volume_Mute, 0xAD },
			{ Key::Volume_Down, 0xAE },
			{ Key::Volume_Up, 0xAF },
			{ Key::Media_Next_Track, 0xB0 },
			{ Key::Media_Prev_Track, 0xB1 },
			{ Key::Media_Stop, 0xB2 },
			{ Key::Media_Play_Pause, 0xB3 },
			{ Key::Launch_Mail, 0xB4 },
			{ Key::Launch_Media_Select, 0xB5 },
			{ Key::Launch_App_1, 0xB6 },
			{ Key::Launch_App_2, 0xB7 },
			{ Key::None, 0xB8 },
			{ Key::None, 0xB9 },
			{ Key::International_1, 0xBA },
			{ Key::Plus, 0xBB },
			{ Key::Comma, 0xBC },
			{ Key::Minus, 0xBD },
			{ Key::Period, 0xBE },
			{ Key::International_2, 0xBF },
			{ Key::International_3, 0xC0 },
			{ Key::None, 0xC1 },
			{ Key::None, 0xC2 },
			{ Key::None, 0xC3 },
			{ Key::None, 0xC4 },
			{ Key::None, 0xC5 },
			{ Key::None, 0xC6 },
			{ Key::None, 0xC7 },
			{ Key::None, 0xC8 },
			{ Key::None, 0xC9 },
			{ Key::None, 0xCA },
			{ Key::None, 0xCB },
			{ Key::None, 0xCC },
			{ Key::None, 0xCD },
			{ Key::None, 0xCE },
			{ Key::None, 0xCF },
			{ Key::None, 0xD0 },
			{ Key::None, 0xD1 },
			{ Key::None, 0xD2 },
			{ Key::None, 0xD3 },
			{ Key::None, 0xD4 },
			{ Key::None, 0xD5 },
			{ Key::None, 0xD6 },
			{ Key::None, 0xD7 },
			{ Key::None, 0xD8 },
			{ Key::None, 0xD9 },
			{ Key::None, 0xDA },
			{ Key::International_4, 0xDB },
			{ Key::International_5, 0xDC },
			{ Key::International_6, 0xDD },
			{ Key::International_7, 0xDE },
			{ Key::International_8, 0xDF },
			{ Key::None, 0xE0 },
			{ Key::None, 0xE1 },
			{ Key::None, 0xE2 },
			{ Key::None, 0xE3 },
			{ Key::None, 0xE4 },
			{ Key::ProcessKey, 0xE5 },
			{ Key::None, 0xE6 },
			{ Key::None, 0xE7 },
			{ Key::None, 0xE8 },
			{ Key::None, 0xE9 },
			{ Key::None, 0xEA },
			{ Key::None, 0xEB },
			{ Key::None, 0xEC },
			{ Key::None, 0xED },
			{ Key::None, 0xEE },
			{ Key::None, 0xEF },
			{ Key::None, 0xF0 },
			{ Key::None, 0xF1 },
			{ Key::None, 0xF2 },
			{ Key::None, 0xF3 },
			{ Key::None, 0xF4 },
			{ Key::None, 0xF5 },
			{ Key::Attn, 0xF6 },
			{ Key::CrSel, 0xF7 },
			{ Key::ExSel, 0xF8 },
			{ Key::ErEOF, 0xF9 },
			{ Key::Play, 0xFA },
			{ Key::Zoom, 0xFB },
			{ Key::None, 0xFC },
			{ Key::None, 0xFD },
			{ Key::None, 0xFE },
			{ Key::None, 0xFF },
		};
	}
}