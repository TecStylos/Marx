#include "Flanger.h"

EffectFlanger::EffectFlanger()
{
	m_desc.fWetDryMix = 50.0f;
	m_desc.fDepth = 100.0f;
	m_desc.fFeedback = -50.0f;
	m_desc.fFrequency = 0.25f;
	m_desc.lWaveform = DSFXFLANGER_WAVE_SIN;
	m_desc.fDelay = 2.0f;
	m_desc.lPhase = DSFXFLANGER_PHASE_ZERO;
}

void EffectFlanger::updateOnBuffer(SoundBuffer* pBuffer, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	auto pfx = (IDirectSoundFXFlanger*)getFX(pBuffer, index, IID_IDirectSoundFXFlanger);

	DH_ASSERT_HR(
		pfx->SetAllParameters(&m_desc)
	);
}

bool EffectFlanger::getNextParam(uint32_t paramIndex, EffectParamDesc* desc)
{
	switch (paramIndex)
	{
	case 0:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXFLANGER_WETDRYMIX_MIN;
		*(FLOAT*)&desc->maxVal = DSFXFLANGER_WETDRYMIX_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fWetDryMix;
		desc->name = "Wet Dry Mix";
		desc->internalName = "mix";
		break;
	case 1:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXFLANGER_DEPTH_MIN;
		*(FLOAT*)&desc->maxVal = DSFXFLANGER_DEPTH_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fDepth;
		desc->name = "Depth";
		desc->internalName = "depth";
		break;
	case 2:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXFLANGER_FEEDBACK_MIN;
		*(FLOAT*)&desc->maxVal = DSFXFLANGER_FEEDBACK_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fFeedback;
		desc->name = "Feedback";
		desc->internalName = "feedback";
		break;
	case 3:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXFLANGER_FREQUENCY_MIN;
		*(FLOAT*)&desc->maxVal = DSFXFLANGER_FREQUENCY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fFrequency;
		desc->name = "Frequency";
		desc->internalName = "frequency";
		break;
	case 4:
		desc->type = EffectParamType::LONG;
		*(LONG*)&desc->minVal = DSFXFLANGER_WAVE_TRIANGLE;
		*(LONG*)&desc->maxVal = DSFXFLANGER_WAVE_SIN;
		desc->pValue = (EffectParam_t*)&m_desc.lWaveform;
		desc->name = "Waveform";
		desc->internalName = "waveform";
		break;
	case 5:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXFLANGER_DELAY_MIN;
		*(FLOAT*)&desc->maxVal = DSFXFLANGER_DELAY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fDelay;
		desc->name = "Delay";
		desc->internalName = "delay";
		break;
	case 6:
		desc->type = EffectParamType::LONG;
		*(LONG*)&desc->minVal = DSFXFLANGER_PHASE_MIN;
		*(LONG*)&desc->maxVal = DSFXFLANGER_PHASE_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.lPhase;
		desc->name = "Phase";
		desc->internalName = "phase";
		break;
	default:
		return false;
	}
	return true;
}
