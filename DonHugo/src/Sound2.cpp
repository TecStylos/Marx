#include "Sound2.h"

Sound2::Sound2(SoundDevice* pDev1, SoundDevice* pDev2, const std::string& filepath)
	: m_filepath(filepath), m_volume(1.0f), m_loopingEnabled(false)
{
	if (pDev1) setNewDevice1(pDev1);
	if (pDev2) setNewDevice2(pDev2);
}

void Sound2::start()
{
	if (m_pBuf1) m_pBuf1->start(0, m_loopingEnabled);
	if (m_pBuf2) m_pBuf2->start(0, m_loopingEnabled);
}

void Sound2::resume()
{
	if (m_pBuf1) m_pBuf1->resume(m_loopingEnabled);
	if (m_pBuf2) m_pBuf2->resume(m_loopingEnabled);
}

void Sound2::stop()
{
	if (m_pBuf1) m_pBuf1->stop();
	if (m_pBuf2) m_pBuf2->stop();
}

void Sound2::setCurrBlock(uint32_t block)
{
	if (m_pBuf1) m_pBuf1->setCurrBlock(block);
	if (m_pBuf2) m_pBuf2->setCurrBlock(block);
}

void Sound2::setVolume(float volume)
{
	m_volume = volume;

	if (m_pBuf1) m_pBuf1->setVolume(volume);
	if (m_pBuf2) m_pBuf2->setVolume(volume);
}

void Sound2::enableLooping(bool loopingEnabled)
{
	m_loopingEnabled = loopingEnabled;

	if (isPlaying())
	{
		stop();
		resume();
	}
}

void Sound2::setNewDevice1(SoundDevice* pDev1)
{
	bool wasPlaying = isPlaying();
	if (wasPlaying)
		stop();

	uint32_t currBlock = getCurrBlock();
	
	m_pBuf1.reset();

	if (pDev1)
	{
		m_pBuf1 = SoundBuffer::loadFromFile(pDev1, m_filepath);
		m_pBuf1->setVolume(m_volume);
		m_pBuf1->setCurrBlock(currBlock);
		applyEffects();
		for (auto e : m_effects)
			updateEffect(e);
	}

	if (wasPlaying)
		resume();
}

void Sound2::setNewDevice2(SoundDevice* pDev2)
{
	bool wasPlaying = isPlaying();
	if (wasPlaying)
		stop();

	uint32_t currBlock = getCurrBlock();

	m_pBuf2.reset();

	if (pDev2)
	{
		m_pBuf2 = SoundBuffer::loadFromFile(pDev2, m_filepath);
		if (m_pBuf2)
		{
			m_pBuf2->setVolume(m_volume);
			m_pBuf2->setCurrBlock(currBlock);
		}
		applyEffects();
		for (auto e : m_effects)
			updateEffect(e);
	}

	if (wasPlaying)
		resume();
}

bool& Sound2::useEffects()
{
	return m_useEffects;
}

void Sound2::addEffect(std::shared_ptr<Effect> pEffect)
{
	m_effects.addEffect(pEffect);
	applyEffects();
}

void Sound2::removeEffect(EffectType type)
{
	m_effects.removeEffect(type);
	applyEffects();
}

void Sound2::applyEffects()
{
	bool wasPlaying = isPlaying();
	if (wasPlaying)
		stop();

	if (m_pBuf1) m_effects.applyEffects(m_pBuf1.get(), !m_useEffects);
	if (m_pBuf2) m_effects.applyEffects(m_pBuf2.get(), !m_useEffects);

	if (wasPlaying)
		resume();
}

void Sound2::updateEffect(EffectType type)
{
	if (!m_useEffects)
		return;

	if (m_pBuf1) m_effects.updateEffect(m_pBuf1.get(), type);
	if (m_pBuf2) m_effects.updateEffect(m_pBuf2.get(), type);
}

void Sound2::swapEffects(uint32_t index1, uint32_t index2)
{
	m_effects.swapEffects(index1, index2);
	applyEffects();
}

bool Sound2::hasEffect(EffectType type)
{
	return m_effects.hasEffect(type);
}

bool Sound2::isPlaying() const
{
	return
		m_pBuf1 ? m_pBuf1->isPlaying() :
		m_pBuf2 ? m_pBuf2->isPlaying() : false;
}

bool Sound2::effectsEnabled() const
{
	return
		m_pBuf1 ? m_pBuf1->effectsEnabled() :
		m_pBuf2 ? m_pBuf2->effectsEnabled() : false;
}

uint32_t Sound2::getSize() const
{
	return
		m_pBuf1 ? m_pBuf1->getSize() :
		m_pBuf2 ? m_pBuf2->getSize() : 0;
}

uint32_t Sound2::getNumChannels() const
{
	return
		m_pBuf1 ? m_pBuf1->getNumChannels() :
		m_pBuf2 ? m_pBuf2->getNumChannels() : 1;
}

uint32_t Sound2::getSampleRate() const
{
	return
		m_pBuf1 ? m_pBuf1->getSampleRate() :
		m_pBuf2 ? m_pBuf2->getSampleRate() : 1;
}

uint32_t Sound2::getBitsPerSample() const
{
	return
		m_pBuf1 ? m_pBuf1->getBitsPerSample() :
		m_pBuf2 ? m_pBuf2->getBitsPerSample() : 8;
}

uint32_t Sound2::getCurrBlock() const
{
	return
		m_pBuf1 ? m_pBuf1->getCurrBlock() :
		m_pBuf2 ? m_pBuf2->getCurrBlock() : 0;
}

uint32_t Sound2::getBlockCount() const
{
	return
		m_pBuf1 ? m_pBuf1->getBlockCount() :
		m_pBuf2 ? m_pBuf2->getBlockCount() : 1;
}

EffectList* Sound2::getEffectList()
{
	return &m_effects;
}
