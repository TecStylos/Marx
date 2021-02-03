#pragma once

#include "Effect.h"

class EffectEqualizer : public Effect
{
public:
	EffectEqualizer();
	~EffectEqualizer() = default;
public:
	virtual EffectType getType() const override { return EffectType_Equalizer; }
	virtual GUID getGUID() const override { return GUID_DSFX_STANDARD_PARAMEQ; }
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) override;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) override;
private:
	DSFXParamEq m_desc;
};