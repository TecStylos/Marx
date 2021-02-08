#include "Reverb.h"

EffectReverb::EffectReverb()
{
	m_desc.fInGain = 0.0f;
	m_desc.fReverbMix = 0.0f;
	m_desc.fReverbTime = 1000.0f;
	m_desc.fHighFreqRTRatio = 0.001f;
}

void EffectReverb::updateOnBuffer(SoundBuffer* pBuffer, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	auto pfx = (IDirectSoundFXWavesReverb*)getFX(pBuffer, index, IID_IDirectSoundFXWavesReverb);

	DH_ASSERT_HR(
		pfx->SetAllParameters(&m_desc)
	);
}

bool EffectReverb::getNextParam(uint32_t paramIndex, EffectParamDesc* desc)
{
	switch (paramIndex)
	{
	case 0:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFX_WAVESREVERB_INGAIN_MIN;
		*(FLOAT*)&desc->maxVal = DSFX_WAVESREVERB_INGAIN_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fInGain;
		desc->name = "In Gain";
		desc->internalName = "gain";
		break;
	case 1:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFX_WAVESREVERB_REVERBMIX_MIN;
		*(FLOAT*)&desc->maxVal = DSFX_WAVESREVERB_REVERBMIX_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fReverbMix;
		desc->name = "Reverb Mix";
		desc->internalName = "mix";
		break;
	case 2:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFX_WAVESREVERB_REVERBTIME_MIN;
		*(FLOAT*)&desc->maxVal = DSFX_WAVESREVERB_REVERBTIME_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fReverbTime;
		desc->name = "Reverb Time";
		desc->internalName = "time";
		break;
	case 3:
		desc->type = EffectParamType::FLOAT;
		*(FLOAT*)&desc->minVal = DSFX_WAVESREVERB_HIGHFREQRTRATIO_MIN;
		*(FLOAT*)&desc->maxVal = DSFX_WAVESREVERB_HIGHFREQRTRATIO_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.fHighFreqRTRatio;
		desc->name = "High Frequency reverb time ratio";
		desc->internalName = "ratio";
		break;
	default:
		return false;
	}
	return true;
}
