#pragma once

namespace Marx
{
	enum class ControllerButton
	{
		A = 0,
		B,
		X,
		Y,
		Start,
		Back,
		DPAD_Up,
		DPAD_Down,
		DPAD_Left,
		DPAD_Right,
		Thumb_Left,
		Thumb_Right,
		Shoulder_Left,
		Shoulder_Right,
		Count
	};

	inline std::string ControllerButtonString(ControllerButton btn)
	{
		switch (btn)
		{
		case ControllerButton::A: return "A";
		case ControllerButton::B: return "B";
		case ControllerButton::X: return "X";
		case ControllerButton::Y: return "Y";
		case ControllerButton::Start: return "Start";
		case ControllerButton::Back: return "Back";
		case ControllerButton::DPAD_Up: return "DPAD_Up";
		case ControllerButton::DPAD_Down: return "DPAD_Down";
		case ControllerButton::DPAD_Left: return "DPAD_Left";
		case ControllerButton::DPAD_Right: return "DPAD_Right";
		case ControllerButton::Thumb_Left: return "Thumb_Left";
		case ControllerButton::Thumb_Right: return "Thumb_Right";
		case ControllerButton::Shoulder_Left: return "Shoulder_Left";
		case ControllerButton::Shoulder_Right: return "Shoulder_Right";
		}
		return "UNKNOWN";
	}

	enum class ControllerStick
	{
		Left = 0,
		Right
	};

	inline std::string ControllerStickString(ControllerStick stick)
	{
		switch (stick)
		{
		case ControllerStick::Left: return "Left";
		case ControllerStick::Right: return "Right";
		}
		return "UNKNOWN";
	}

	enum class ControllerTrigger
	{
		Left = 0,
		Right
	};

	inline std::string ControllerTriggerString(ControllerTrigger stick)
	{
		switch (stick)
		{
		case ControllerTrigger::Left: return "Left";
		case ControllerTrigger::Right: return "Right";
		}
		return "UNKNOWN";
	}

	typedef char ControllerButtonState;
	#define ControllerButtonState_Up FLAG(0)
	#define ControllerButtonState_Down FLAG(1)
	#define ControllerButtonState_Repeat FLAG(2)

	struct ControllerStickState
	{
		float x, y;
	};

	struct ControllerState
	{
		ControllerButtonState button[(uint32_t)ControllerButton::Count];
		float trigger[2];
		ControllerStickState stick[2];
	};

	struct ControllerDeadzones
	{
		float triggerMin = 0.0f;
		float triggerMax = 1.0f;
		float stickMin = 0.15f;
		float stickMax = 1.0f;
	};

	inline bool operator!=(const ControllerStickState& left, const ControllerStickState& right)
	{
		return (left.x != right.x) || (left.y != right.y);
	}
}