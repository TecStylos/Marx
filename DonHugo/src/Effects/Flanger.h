#pragma once

#include "Effect.h"

class EffectFlanger : public Effect
{
public:
	EffectFlanger();
	~EffectFlanger() = default;
public:
	virtual EffectType getType() const override { return EffectType_Flanger; }
	virtual GUID getGUID() const override { return GUID_DSFX_STANDARD_FLANGER; }
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) override;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) override;
private:
	DSFXFlanger m_desc;
};