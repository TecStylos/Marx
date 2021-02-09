#pragma once

#include <memory>

#include "DirectSoundIncludes.h"

#include "SoundDevice.h"

struct RIFFHeader
{
	char chunkID[4];
	uint32_t chunkSize;
	char format[4];
};

struct FMTHeader
{
	char chunkID[4];
	uint32_t chunkSize;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t bytesPerSecond;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
};

struct DataHeader
{
	char chunkID[4];
	uint32_t chunkSize;
};

class SoundBuffer
{
public:
	SoundBuffer(SoundDevice* pDevice, uint32_t size, uint32_t nChannels, uint32_t nSamplesPerSec, uint32_t nBitsPerSample, bool enableEffects = false);
	~SoundBuffer() = default;
public:
	void update(const char* pData);
	void start(uint32_t block = 0, bool loop = false);
	void resume(bool loop = false);
	void stop();
	void setVolume(float volume);
	void setCurrBlock(uint32_t block);
public:
	bool isPlaying() const;
	bool effectsEnabled() const { return m_effectsEnabled; }
	uint32_t getSize() const { return m_desc.dwBufferBytes; }
	uint32_t getNumChannels() const { return m_format.nChannels; }
	uint32_t getSampleRate() const { return m_format.nSamplesPerSec; }
	uint32_t getBitsPerSample() const { return m_format.wBitsPerSample; }
	uint32_t getCurrBlock() const;
	uint32_t getBlockCount() const { return m_blockCount; }
public:
	IDirectSoundBuffer8* getBuffer() { return m_pBuffer.Get(); }
public:
	static std::shared_ptr<SoundBuffer> loadFromFile(SoundDevice* pDevice, const std::string& filepath);
private:
	uint32_t m_blockSize;
	uint32_t m_blockCount;
	WAVEFORMATEX m_format;
	DSBUFFERDESC m_desc;
	bool m_effectsEnabled;
private:
	ComPtr<IDirectSoundBuffer8> m_pBuffer;
};