#pragma once

#include "../future/SoundBuffer.h"

#include <vector>
#include <memory>
#include <unordered_map>

enum EffectType
{
	EffectType_None = 0,
	EffectType_Chorus,
	EffectType_Compressor,
	EffectType_Distortion,
	EffectType_Echo,
	EffectType_Flanger,
	EffectType_Gargle,
	EffectType_Equalizer,
	EffectType_Reverb
};

enum class EffectParamType
{
	NONE = 0,
	FLOAT,
	LONG,
	DWORD
};

typedef LONG EffectParam_t;

struct EffectParamDesc
{
	EffectParamType type;
	EffectParam_t minVal;
	EffectParam_t maxVal;
	EffectParam_t* pValue;
	const char* name;
};

inline const char* EffectTypeString(EffectType type)
{
	switch (type)
	{
	case EffectType_None: return "None";
	case EffectType_Chorus: return "Chorus";
	case EffectType_Compressor: return "Compressor";
	case EffectType_Distortion: return "Distortion";
	case EffectType_Echo: return "Echo";
	case EffectType_Flanger: return "Flanger";
	case EffectType_Gargle: return "Gargle";
	case EffectType_Equalizer: return "Equalizer";
	case EffectType_Reverb: return "Reverb";
	}
	return "Unknown Type";
}

class Effect
{
public:
	Effect() = default;
	virtual ~Effect() = default;
public:
	virtual EffectType getType() const = 0;
	virtual GUID getGUID() const = 0;
	virtual void updateOnBuffer(SoundBuffer* pBuffer, uint32_t index) = 0;
	virtual bool getNextParam(uint32_t paramIndex, EffectParamDesc* desc) = 0;
protected:
	void* getFX(SoundBuffer* pBuffer, DWORD index, REFGUID iid)
	{
		DH_DEBUG_HR_DECL;

		void* pfx;

		REFGUID guid = getGUID();

		DH_ASSERT_HR(
			pBuffer->getBuffer()->GetObjectInPath(
				guid,
				index,
				iid,
				&pfx
			)
		);

		return pfx;
	}
};