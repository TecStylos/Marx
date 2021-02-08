#include "Compressor.h"

EffectCompressor::EffectCompressor()
{
	m_desc.fGain = 0.0f;
	m_desc.fAttack = 10.0f;
	m_desc.fRelease = 200.0f;
	m_desc.fThreshold = -20.0f;
	m_desc.fRatio = 3.0f;
	m_desc.fPredelay = 4.0f;
}

void EffectCompressor::updateOnBuffer(SoundBuffer* pBuffer, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	auto pfx = (IDirectSoundFXCompressor*)getFX(pBuffer, index, IID_IDirectSoundFXCompressor);

	DH_ASSERT_HR(
		pfx->SetAllParameters(&m_desc)
	);
}

bool EffectCompressor::getNextParam(uint32_t paramIndex, EffectParamDesc* desc)
{
	switch (paramIndex)
	{
	case 0:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCOMPRESSOR_GAIN_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCOMPRESSOR_GAIN_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fGain;
		desc->name = "Gain";
		desc->internalName = "gain";
		break;
	case 1:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCOMPRESSOR_ATTACK_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCOMPRESSOR_ATTACK_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fAttack;
		desc->name = "Attack";
		desc->internalName = "attack";
		break;
	case 2:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCOMPRESSOR_RELEASE_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCOMPRESSOR_RELEASE_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fRelease;
		desc->name = "Release";
		desc->internalName = "release";
		break;
	case 3:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCOMPRESSOR_THRESHOLD_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCOMPRESSOR_THRESHOLD_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fThreshold;
		desc->name = "Threshold";
		desc->internalName = "threshold";
		break;
	case 4:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCOMPRESSOR_RATIO_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCOMPRESSOR_RATIO_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fRatio;
		desc->name = "Ratio";
		desc->internalName = "ratio";
		break;
	case 5:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFXCOMPRESSOR_PREDELAY_MIN;
		*(FLOAT*)&desc->maxVal = DSFXCOMPRESSOR_PREDELAY_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fPredelay;
		desc->name = "Predelay";
		desc->internalName = "predelay";
		break;
	default:
		return false;
	}
	return true;
}
