#pragma once

#include "Effect.h"

class EffectReverb : public Effect
{
public:
	EffectReverb();
	~EffectReverb() = default;
public:
	virtual EffectType getType() const override { return EffectType_Reverb; }
	virtual GUID getGUID() const override { return GUID_DSFX_WAVES_REVERB; }
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) override;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) override;
private:
	DSFXWavesReverb m_desc;
};