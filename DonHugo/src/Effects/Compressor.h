#pragma once

#include "Effect.h"

class EffectCompressor : public Effect
{
public:
	EffectCompressor();
	~EffectCompressor() = default;
public:
	virtual EffectType getType() const override { return EffectType_Compressor; }
	virtual GUID getGUID() const override { return GUID_DSFX_STANDARD_COMPRESSOR; }
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) override;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) override;
private:
	DSFXCompressor m_desc;
};