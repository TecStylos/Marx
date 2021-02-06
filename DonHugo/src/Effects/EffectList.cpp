#include "EffectList.h"

void EffectList::addEffect(std::shared_ptr<Effect> effect)
{
	EffectType type = effect->getType();
	DH_ASSERT(m_effects.find(type) == m_effects.end());

	m_effects.emplace(type, effect);

	m_effectTypes.push_back(type);
}

void EffectList::removeEffect(EffectType type)
{
	DH_ASSERT(m_effects.find(type) != m_effects.end());

	m_effects.erase(type);

	for (uint32_t i = 0; i < m_effectTypes.size(); ++i)
	{
		if (m_effectTypes[i] == type)
		{
			m_effectTypes.erase(m_effectTypes.begin() + i);
			break;
		}
	}
}

void EffectList::applyEffects(SoundBuffer* pBuffer, bool clearEffects)
{
	DH_DEBUG_HR_DECL;

	if (clearEffects)
	{
		DH_ASSERT_HR(pBuffer->getBuffer()->SetFX(0, NULL, NULL));
		return;
	}

	DWORD effectCount = (DWORD)m_effects.size();

	std::vector<DSEFFECTDESC> effectDescs;
	effectDescs.resize(effectCount);

	uint32_t i = 0;
	for (EffectType type : m_effectTypes)
	{
		auto elem = m_effects.find(type);

		DSEFFECTDESC* desc = &effectDescs[i];
		desc->dwSize = sizeof(DSEFFECTDESC);
		desc->dwFlags = DSFX_LOCSOFTWARE;
		desc->guidDSFXClass = elem->second->getGUID();
		++i;
	}

	std::vector<DWORD> resultCodes;
	resultCodes.resize(effectCount);

	DH_ASSERT_HR(
		pBuffer->getBuffer()->SetFX(
			effectCount,
			effectDescs.data(),
			resultCodes.data()
		)
	);
}

void EffectList::updateEffect(SoundBuffer* pBuffer, EffectType type)
{
	auto elem = m_effects.find(type);
	DH_ASSERT(elem != m_effects.end());

	/*uint32_t index = 0;
	for (uint32_t i = 0; i < m_effectTypes.size(); ++i)
	{
		if (m_effectTypes[i] == type)
		{
			index = i;
			break;
		}
	}*/

	elem->second->updateOnBuffer(pBuffer, 0);
}

void EffectList::swapEffects(uint32_t index1, uint32_t index2)
{
	std::swap(m_effectTypes[index1], m_effectTypes[index2]);
}

Effect* EffectList::getEffect(EffectType type)
{
	auto elem = m_effects.find(type);
	DH_ASSERT(elem != m_effects.end());

	return elem->second.get();
}

bool EffectList::hasEffect(EffectType type)
{
	return m_effects.find(type) != m_effects.end();
}
