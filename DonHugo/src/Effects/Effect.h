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
	EffectType_Reverb,
	EffectType_Count
};

inline std::string effectTypeToString(EffectType type)
{
#define EFF_TYPE(t) case EffectType_##t: return #t
	switch (type)
	{
		EFF_TYPE(None);
		EFF_TYPE(Chorus);
		EFF_TYPE(Compressor);
		EFF_TYPE(Distortion);
		EFF_TYPE(Echo);
		EFF_TYPE(Flanger);
		EFF_TYPE(Gargle);
		EFF_TYPE(Equalizer);
		EFF_TYPE(Reverb);
	}
	return "Unknown";
#undef EFF_TYPE
}

inline EffectType effectStringToType(const std::string& str)
{
#define EFF_TYPE(t) if (str == #t) return EffectType_##t
	EFF_TYPE(None);
	EFF_TYPE(Chorus);
	EFF_TYPE(Compressor);
	EFF_TYPE(Distortion);
	EFF_TYPE(Echo);
	EFF_TYPE(Flanger);
	EFF_TYPE(Gargle);
	EFF_TYPE(Equalizer);
	EFF_TYPE(Reverb);
#undef EFF_TYPE

	return EffectType_None;
}

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
	const char* name = "None";
	const char* internalName = "None";
	const char* help = "No help";
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
public:
	static std::shared_ptr<Effect> create(EffectType type);
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