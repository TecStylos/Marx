#include "Controller.h"

namespace Marx
{
	float Controller::calcDZ(float delta, float min, float max)
	{
		delta = std::max(min, std::min(max, delta));
		return (delta - min) / (max - min);
	}
	float Controller::calcTriggerDZ(float delta) const
	{
		return calcDZ(delta, m_deadzones.triggerMin, m_deadzones.triggerMax);
	}

	ControllerStickState Controller::calcStickDZ(const ControllerStickState& stickState) const
	{
		return {
			(std::signbit(stickState.x) ? -1.0f : 1.0f) * calcDZ(std::abs(stickState.x), m_deadzones.stickMin, m_deadzones.stickMax),
			(std::signbit(stickState.y) ? -1.0f : 1.0f) * calcDZ(std::abs(stickState.y), m_deadzones.stickMin, m_deadzones.stickMax),
		};
	}
}