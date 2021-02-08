#include "Echo.h"

EffectEcho::EffectEcho()
{
	m_desc.fWetDryMix = 50.0f;
	m_desc.fFeedback = 50.0f;
	m_desc.fLeftDelay = 500.0f;
	m_desc.fRightDelay = 500.0f;
	m_desc.lPanDelay = 0;
}

void EffectEcho::updateOnBuffer(SoundBuffer* pBuffer, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	auto pfx = (IDirectSoundFXEcho*)getFX(pBuffer, index, IID_IDirectSoundFXEcho);

	DH_ASSERT_HR(
		pfx->SetAllParameters(&m_desc)
	);
}

bool EffectEcho::getNextParam(uint32_t paramIndex, EffectParamDesc* desc)
{
	switch (paramIndex)
	{
	case 0:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXECHO_WETDRYMIX_MIN;
		*(FLOAT*)&desc->maxVal = DSFXECHO_WETDRYMIX_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fWetDryMix;
		desc->name = "Wet Dry Mix";
		desc->internalName = "mix";
		break;
	case 1:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXECHO_FEEDBACK_MIN;
		*(FLOAT*)&desc->maxVal = DSFXECHO_FEEDBACK_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fFeedback;
		desc->name = "Feedback";
		desc->internalName = "feedback";
		break;
	case 2:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXECHO_LEFTDELAY_MIN;
		*(FLOAT*)&desc->maxVal = DSFXECHO_LEFTDELAY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fLeftDelay;
		desc->name = "Left Delay";
		desc->internalName = "leftDelay";
		break;
	case 3:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXECHO_RIGHTDELAY_MIN;
		*(FLOAT*)&desc->maxVal = DSFXECHO_RIGHTDELAY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fRightDelay;
		desc->name = "Right Delay";
		desc->internalName = "rightDelay";
		break;
	case 4:
		desc->type = EffectParamType::LONG;
		*(LONG*)&desc->minVal = DSFXECHO_PANDELAY_MIN;
		*(LONG*)&desc->maxVal = DSFXECHO_PANDELAY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.lPanDelay;
		desc->name = "Pan Delay";
		desc->internalName = "panDelay";
		break;
	default:
		return false;
	}
	return true;
}
