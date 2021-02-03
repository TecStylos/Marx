#pragma once

#include "future/CaptureBuffer.h"
#include "future/SoundBuffer.h"

#include <memory>
#include <array>
#include <thread>
#include <queue>
#include <mutex>


class MicPlayback
{
public:
	MicPlayback(CaptureDevice* pCaptureDev, SoundDevice* pSoundDev, uint32_t updateIntervalBlocks = 400, uint32_t nChannels = 2, uint32_t nBlocksPerSec = 8000, uint32_t nBytesPerSample = 2);
	~MicPlayback();
private:
	void start();
	void stop();
public:
	void setVolume(float volume);
	void setNewCaptureDevice(CaptureDevice* pDevice);
public:
	bool isCapturing() { return m_pCaptureBuffer->isCapturing(); }
	bool isPlaying() { return m_pSoundBuffer->isPlaying(); }
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
	void soundThreadFunc();
	void readCaptureBuffer(DWORD block);
	void writeSoundBuffer(DWORD block);
private:
	uint32_t m_updateIntervalBlocks;
	uint32_t m_nBytesPerBlock;
	bool m_isRunning;
private:
	static constexpr uint32_t nCaptureEvents = 3;
	static constexpr uint32_t nSoundEvents = 2;
	static constexpr uint32_t nBlocks = 2;
private:
	std::unique_ptr<CaptureBuffer> m_pCaptureBuffer;
	std::unique_ptr<SoundBuffer> m_pSoundBuffer;
private:
	std::array<HANDLE, nCaptureEvents> m_captureEvents;
	std::array<HANDLE, nSoundEvents> m_soundEvents;
	std::thread m_readCaptureThread;
	std::thread m_writeSoundThread;
private:
	std::mutex m_fullDataBuffersMutex;
	std::mutex m_freeDataBuffersMutex;
	std::queue<char*> m_fullDataBuffers;
	std::queue<char*> m_freeDataBuffers;
};