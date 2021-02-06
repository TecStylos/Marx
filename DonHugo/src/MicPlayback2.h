#pragma once

#include "future/CaptureBuffer.h"
#include "future/SoundBuffer.h"
#include "Effects/EffectList.h"

#include <memory>
#include <array>
#include <thread>
#include <queue>
#include <mutex>

struct MicPlayback2Desc
{
	CaptureDevice* pCaptureDev;
	SoundDevice* pSoundDev[2];
	uint32_t nBlocksPerUpdate = 2048;
	uint32_t nChannels = 2;
	uint32_t nBlocksPerSec = 44100;
	uint32_t nBytesPerSample = 2;
	bool enableEffects = false;
};

class MicPlayback2
{
public:
	MicPlayback2(const MicPlayback2Desc* desc);
	~MicPlayback2();
public:
	void setVolume(float volume);
	void setVolumeMultiplier1(float m);
	void setVolumeMultiplier2(float m);
	void setNewCaptureDevice(CaptureDevice* pDevice);
public:
	bool effectsEnabled() const;
	void addEffect(std::shared_ptr<Effect> pEffect);
	void removeEffect(EffectType type);
	void applyEffects();
	void updateEffect(EffectType type);
	void swapEffects(uint32_t index1, uint32_t index2);
	bool hasEffect(EffectType type);
	EffectList* getEffectList();
public:
	bool isCapturing() const;
	bool isPlaying() const;
	float getVolume() const { return m_volume; }
	float getVolumeMultiplier1() const { return m_volumeMultiplier[0]; }
	float getVolumeMultiplier2() const { return m_volumeMultiplier[1]; }
private:
	void start();
	void startThreads();
	void stop();
	void stopThreads();
private:
	void setCaptureNotifications();
	void setSoundNotifications();
	void clearCaptureNotifications();
	void clearSoundNotifications();
private:
	void createDataBuffers();
	void deleteDataBuffers();
private:
	void autoJoinThreads();
private:
	void captureThreadFunc();
	void soundThreadFunc(uint32_t index);
	void readCaptureBuffer(DWORD block);
	void writeSoundBuffer(DWORD block, uint32_t index);
private:
	MicPlayback2Desc m_desc;
	uint32_t m_nBytesPerBlock;
	bool m_isRunning;
	float m_volume = 1.0f;
	float m_volumeMultiplier[2] = { 1.0f, 1.0f };
private:
	static constexpr uint32_t nCaptureEvents = 3;
	static constexpr uint32_t nSoundEvents = 2;
	static constexpr uint32_t nBlocks = 2;
	inline static bool s_instanceExists = false;
private:
	std::unique_ptr<CaptureBuffer> m_pCaptureBuffer;
	std::unique_ptr<SoundBuffer> m_pSoundBuffer[2];
	EffectList m_effects;
private:
	std::array<HANDLE, nCaptureEvents> m_captureEvents;
	std::array<HANDLE, nSoundEvents> m_soundEvents[2];
	std::thread m_readCaptureThread;
	std::thread m_writeSoundThread[2];
private:
	std::mutex m_fullDataBuffersMutex[2];
	std::mutex m_freeDataBuffersMutex[2];
	std::mutex m_lockBufferMutex[2];
	std::queue<char*> m_fullDataBuffers[2];
	std::queue<char*> m_freeDataBuffers[2];
};