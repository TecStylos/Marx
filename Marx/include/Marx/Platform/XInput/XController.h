#pragma once

#include <Xinput.h>
#include <set>

#include "Marx/Input/Controller.h"

namespace Marx
{
	class MARX_API XController : public Controller
	{
	public:
		XController(ControllerID id)
			: Controller(id), m_pStateNew(&m_states[0]), m_pStateOld(&m_states[1]), m_states()
		{
			m_uid = *s_availUIDs.begin();
			s_availUIDs.erase(s_availUIDs.begin());
		}
	public:
		bool onUpdate() override;
	public:
		ControllerType getType() const override { return ControllerType::XInput; }
	private:
		DWORD m_uid;
		XINPUT_STATE* m_pStateNew;
		XINPUT_STATE* m_pStateOld;
		XINPUT_STATE m_states[2];
	private:
		static std::set<DWORD> s_availUIDs;
	};
}