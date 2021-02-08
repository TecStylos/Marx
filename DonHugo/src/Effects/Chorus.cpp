#include "Chorus.h"

EffectChorus::EffectChorus()
{
	m_desc.fWetDryMix = 50.0f;
	m_desc.fDepth = 10.0f;
	m_desc.fFeedback = 25.0f;
	m_desc.fFrequency = 1.1f;
	m_desc.lWaveform = DSFXCHORUS_WAVE_SIN;
	m_desc.fDelay = 16.0f;
	m_desc.lPhase = DSFXCHORUS_PHASE_ZERO;
}

void EffectChorus::updateOnBuffer(SoundBuffer* pBuffer, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	auto pfx = (IDirectSoundFXChorus*)getFX(pBuffer, index, IID_IDirectSoundFXChorus);

	DH_ASSERT_HR(
		pfx->SetAllParameters(&m_desc)
	);
}

bool EffectChorus::getNextParam(uint32_t paramIndex, EffectParamDesc* desc)
{
	switch (paramIndex)
	{
	case 0:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCHORUS_WETDRYMIX_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCHORUS_WETDRYMIX_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fWetDryMix;
		desc->name = "Wet Dry Mix";
		desc->internalName = "mix";
		break;
	case 1:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCHORUS_DEPTH_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCHORUS_DEPTH_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fDepth;
		desc->name = "Depth";
		desc->internalName = "depth";
		break;
	case 2:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCHORUS_FEEDBACK_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCHORUS_FEEDBACK_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fFeedback;
		desc->name = "Feedback";
		desc->internalName = "feedback";
		break;
	case 3:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCHORUS_FREQUENCY_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCHORUS_FREQUENCY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fFrequency;
		desc->name = "Frequency";
		desc->internalName = "frequency";
		break;
	case 4:
		desc->type = EffectParamType::LONG;
		*(LONG*)&desc->minVal = DSFXCHORUS_WAVE_TRIANGLE;
		*(LONG*)&desc->maxVal = DSFXCHORUS_WAVE_SIN;
		desc->pValue = (EffectParam_t*)&m_desc.lWaveform;
		desc->name = "Waveform";
		desc->internalName = "waveform";
		break;
	case 5:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCHORUS_DELAY_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCHORUS_DELAY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fDelay;
		desc->name = "Delay";
		desc->internalName = "delay";
		break;
	case 6:
		desc->type = EffectParamType::LONG;
		*(LONG*)&desc->minVal = DSFXCHORUS_PHASE_MIN;
		*(LONG*)&desc->maxVal = DSFXCHORUS_PHASE_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.lPhase;
		desc->name = "Phase";
		desc->internalName = "phase";
		break;
	default:
		return false;
	}
	return true;
}
