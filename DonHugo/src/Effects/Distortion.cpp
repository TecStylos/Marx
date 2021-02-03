#include "Distortion.h"

EffectDistortion::EffectDistortion()
{
	m_desc.fGain = -18.0f;
	m_desc.fEdge = 15.0f;
	m_desc.fPostEQCenterFrequency = 2400.0f;
	m_desc.fPostEQBandwidth = 2400.0f;
	m_desc.fPreLowpassCutoff = 8000.0f;
}

void EffectDistortion::updateOnBuffer(SoundBuffer* pBuffer, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	auto pfx = (IDirectSoundFXDistortion*)getFX(pBuffer, index, IID_IDirectSoundFXDistortion);

	DH_ASSERT_HR(
		pfx->SetAllParameters(&m_desc)
	);
}

bool EffectDistortion::getNextParam(uint32_t paramIndex, EffectParamDesc* desc)
{
	switch (paramIndex)
	{
	case 0:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXDISTORTION_GAIN_MIN;
		*(FLOAT*)&desc->maxVal = DSFXDISTORTION_GAIN_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fGain;
		desc->name = "Gain";
		break;
	case 1:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXDISTORTION_EDGE_MIN;
		*(FLOAT*)&desc->maxVal = DSFXDISTORTION_EDGE_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fEdge;
		desc->name = "Edge";
		break;
	case 2:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXDISTORTION_POSTEQCENTERFREQUENCY_MIN;
		*(FLOAT*)&desc->maxVal = DSFXDISTORTION_POSTEQCENTERFREQUENCY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fPostEQCenterFrequency;
		desc->name = "Post EQ Center Frequency";
		break;
	case 3:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXDISTORTION_POSTEQBANDWIDTH_MIN;
		*(FLOAT*)&desc->maxVal = DSFXDISTORTION_POSTEQBANDWIDTH_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fPostEQBandwidth;
		desc->name = "Post EQ Bandwidth";
		break;
	case 4:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXDISTORTION_PRELOWPASSCUTOFF_MIN;
		*(FLOAT*)&desc->maxVal = DSFXDISTORTION_PRELOWPASSCUTOFF_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fPreLowpassCutoff;
		desc->name = "Pre Lowpass Cutoff";
		break;
	default:
		return false;
	}
	return true;
}
