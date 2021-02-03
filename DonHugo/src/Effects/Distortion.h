#pragma once

#include "Effect.h"

class EffectDistortion : public Effect
{
public:
	EffectDistortion();
	~EffectDistortion() = default;
public:
	virtual EffectType getType() const override { return EffectType_Distortion; }
	virtual GUID getGUID() const override { return GUID_DSFX_STANDARD_DISTORTION; }
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) override;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) override;
private:
	DSFXDistortion m_desc;
};