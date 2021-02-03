#pragma once

#include "future/SoundBuffer.h"

#include <memory>

class Sound
{
public:
	Sound(SoundDevice* pDevice, const std::string& filepath);
	~Sound() = default;
public:
	void start() { m_pBuffer->start(0, m_loopingEnabled); }
	void resume() { m_pBuffer->resume(m_loopingEnabled); }
	void stop() { m_pBuffer->stop(); }
public:
	void setCurrBlock(uint32_t block) { m_pBuffer->setCurrBlock(block); }
	void setVolume(float volume) { m_volume = volume; m_pBuffer->setVolume(m_volume); }
	void enableLooping(bool loopingEnabled);
	void setNewDevice(SoundDevice* pDevice);
public:
	bool isPlaying() const { return m_pBuffer->isPlaying(); }
	uint32_t getSize() const { return m_pBuffer->getSize(); }
	uint32_t getNumChannels() const { return m_pBuffer->getNumChannels(); }
	uint32_t getSampleRate() const { return m_pBuffer->getSampleRate(); }
	uint32_t getBitsPerSample() const { return m_pBuffer->getBitsPerSample(); }
	uint32_t getCurrBlock() const { return m_pBuffer->getCurrBlock(); }
	uint32_t getBlockCount() const { return m_pBuffer->getBlockCount(); }
public:
	const std::string& getFilepath() const { return m_filepath; }
	float getVolume() const { return m_volume; }
	bool getLoopingEnabled() const { return m_loopingEnabled; }
private:
	std::string m_filepath;
	float m_volume;
	bool m_loopingEnabled;
private:
	std::shared_ptr<SoundBuffer> m_pBuffer;
};