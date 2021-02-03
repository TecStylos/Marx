#pragma once

#include "Effect.h"

class EffectChorus : public Effect
{
public:
	EffectChorus();
	~EffectChorus() = default;
public:
	virtual EffectType getType() const override { return EffectType_Chorus; }
	virtual GUID getGUID() const override { return GUID_DSFX_STANDARD_CHORUS; }
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) override;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) override;
private:
	DSFXChorus m_desc;
};