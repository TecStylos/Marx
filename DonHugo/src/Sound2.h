#pragma once

#include "future/SoundBuffer.h"
#include "Effects/EffectList.h"

#include <memory>

class Sound2
{
public:
	Sound2(SoundDevice* pDev1, SoundDevice* pDev2, const std::string& filepath);
	~Sound2() = default;
public:
	void start();
	void resume();
	void stop();
public:
	void setCurrBlock(uint32_t block);
	void setVolume(float volume);
	void enableLooping(bool loopingEnabled);
	void setNewDevice1(SoundDevice* pDev1);
	void setNewDevice2(SoundDevice* pDev2);
public:
	bool& useEffects();
	void addEffect(std::shared_ptr<Effect> pEffect);
	void removeEffect(EffectType type);
	void applyEffects();
	void updateEffect(EffectType type);
	void swapEffects(uint32_t index1, uint32_t index2);
	bool hasEffect(EffectType type);
public:
	bool isPlaying() const;
	bool effectsEnabled() const;
	uint32_t getSize() const;
	uint32_t getNumChannels() const;
	uint32_t getSampleRate() const;
	uint32_t getBitsPerSample() const;
	uint32_t getCurrBlock() const;
	uint32_t getBlockCount() const;
	EffectList* getEffectList();
public:
	const std::string& getFilepath() const { return m_filepath; }
	float getVolume() const { return m_volume; }
	bool getLoopingEnabled() const { return m_loopingEnabled; }
private:
	std::string m_filepath;
	bool m_useEffects = false;
	float m_volume;
	bool m_loopingEnabled;
private:
	std::shared_ptr<SoundBuffer> m_pBuf1;
	std::shared_ptr<SoundBuffer> m_pBuf2;
	EffectList m_effects;
};