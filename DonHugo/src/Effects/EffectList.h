#pragma once

#include "Effect.h"

class EffectList
{
public:
	EffectList() = default;
	~EffectList() = default;
public:
	void addEffect(std::shared_ptr<Effect> effect);
	void removeEffect(EffectType type);
	void applyEffects(SoundBuffer* pBuffer, bool clearEffects = false);
	void updateEffect(SoundBuffer* pBuffer, EffectType type);
	void swapEffects(uint32_t index1, uint32_t index2);
	uint32_t size() const { return (uint32_t)m_effectTypes.size(); }
	Effect* getEffect(EffectType type);
	bool hasEffect(EffectType type);
public:
	std::vector<EffectType>::iterator begin() { return m_effectTypes.begin(); }
	std::vector<EffectType>::iterator end() { return m_effectTypes.end(); }
private:
	std::vector<EffectType> m_effectTypes;
	std::unordered_map<EffectType, std::shared_ptr<Effect>> m_effects;
};