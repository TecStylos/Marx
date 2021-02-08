#include "Gargle.h"

EffectGargle::EffectGargle()
{
	m_desc.dwRateHz = 20;
	m_desc.dwWaveShape = DSFXGARGLE_WAVE_TRIANGLE;
}

void EffectGargle::updateOnBuffer(SoundBuffer* pBuffer, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	auto pfx = (IDirectSoundFXGargle*)getFX(pBuffer, index, IID_IDirectSoundFXGargle);

	DH_ASSERT_HR(
		pfx->SetAllParameters(&m_desc)
	);
}

bool EffectGargle::getNextParam(uint32_t paramIndex, EffectParamDesc* desc)
{
	switch (paramIndex)
	{
	case 0:
		desc->type = EffectParamType::DWORD;
		*(DWORD*)&desc->minVal = DSFXGARGLE_RATEHZ_MIN;
		*(DWORD*)&desc->maxVal = DSFXGARGLE_RATEHZ_MAX;
		desc->pValue = (EffectParam_t*)&m_desc.dwRateHz;
		desc->name = "Rate HZ";
		desc->internalName = "rate";
		break;
	case 1:
		desc->type = EffectParamType::DWORD;
		*(DWORD*)&desc->minVal = DSFXGARGLE_WAVE_TRIANGLE;
		*(DWORD*)&desc->maxVal = DSFXGARGLE_WAVE_SQUARE;
		desc->pValue = (EffectParam_t*)&m_desc.dwWaveShape;
		desc->name = "Waveshape";
		desc->internalName = "shape";
		break;
	default:
		return false;
	}
	return true;
}
