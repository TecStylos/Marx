#include "Equalizer.h"

EffectEqualizer::EffectEqualizer()
{
	m_desc.fCenter = 8000.0f;
	m_desc.fBandwidth = 12.0f;
	m_desc.fGain = 0.0f;
}

void EffectEqualizer::updateOnBuffer(SoundBuffer* pBuffer, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	auto pfx = (IDirectSoundFXParamEq*)getFX(pBuffer, index, IID_IDirectSoundFXParamEq);

	DH_ASSERT_HR(
		pfx->SetAllParameters(&m_desc)
	);
}

bool EffectEqualizer::getNextParam(uint32_t paramIndex, EffectParamDesc* desc)
{
	switch (paramIndex)
	{
	case 0:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXPARAMEQ_CENTER_MIN;
		*(FLOAT*)&desc->maxVal = DSFXPARAMEQ_CENTER_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fCenter;
		desc->name = "Center";
		desc->internalName = "center";
		break;
	case 1:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXPARAMEQ_BANDWIDTH_MIN;
		*(FLOAT*)&desc->maxVal = DSFXPARAMEQ_BANDWIDTH_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fBandwidth;
		desc->name = "Bandwidth";
		desc->internalName = "bandwidth";
		break;
	case 2:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXPARAMEQ_GAIN_MIN;
		*(FLOAT*)&desc->maxVal = DSFXPARAMEQ_GAIN_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fGain;
		desc->name = "Gain";
		desc->internalName = "gain";
		break;
	default:
		return false;
	}
	return true;
}
