#pragma once

#include "Effect.h"

class EffectGargle : public Effect
{
public:
	EffectGargle();
	~EffectGargle() = default;
public:
	virtual EffectType getType() const override { return EffectType_Gargle; }
	virtual GUID getGUID() const override { return GUID_DSFX_STANDARD_GARGLE; }
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) override;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) override;
private:
	DSFXGargle m_desc;
};