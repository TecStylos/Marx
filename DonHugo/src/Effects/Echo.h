#pragma once

#include "Effect.h"

class EffectEcho : public Effect
{
public:
	EffectEcho();
	~EffectEcho() = default;
public:
	virtual EffectType getType() const override { return EffectType_Echo; }
	virtual GUID getGUID() const override { return GUID_DSFX_STANDARD_ECHO; }
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) override;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) override;
private:
	DSFXEcho m_desc;
};