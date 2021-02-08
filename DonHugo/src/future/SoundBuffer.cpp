#include "SoundBuffer.h"

#include <fstream>

#include "Tools.h"

#include <exception>

SoundBuffer::SoundBuffer(SoundDevice* pDevice, uint32_t size, uint32_t nChannels, uint32_t nSamplesPerSec, uint32_t nBitsPerSample, bool enableEffects)
{
	DH_DEBUG_HR_DECL;

	m_effectsEnabled = enableEffects;

	m_format.wFormatTag = WAVE_FORMAT_PCM;
	m_format.nChannels = nChannels;
	m_format.nSamplesPerSec = nSamplesPerSec;
	m_format.wBitsPerSample = nBitsPerSample;
	m_format.nBlockAlign = (m_format.nChannels * m_format.wBitsPerSample) / 8;
	m_format.nAvgBytesPerSec = m_format.nSamplesPerSec * m_format.nBlockAlign;
	m_format.cbSize = 0;

	m_blockSize = m_format.nBlockAlign;
	m_blockCount = size / m_blockSize;

	DWORD descFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE;
	if (enableEffects)
		descFlags |= DSBCAPS_CTRLFX;

	m_desc.dwSize = sizeof(DSBUFFERDESC);
	m_desc.dwFlags = descFlags;
	m_desc.dwBufferBytes = size;
	m_desc.dwReserved = 0;
	m_desc.lpwfxFormat = &m_format;
	m_desc.guid3DAlgorithm = DS3DALG_DEFAULT;

	ComPtr<IDirectSoundBuffer> pBuffer;

	DH_THROW_HR(
		pDevice->getDev()->CreateSoundBuffer(
		&m_desc,
		pBuffer.GetAddressOf(),
		NULL
		)
	);

	DH_ASSERT_HR(
		pBuffer->QueryInterface(
			IID_IDirectSoundBuffer8,
			(void**)m_pBuffer.GetAddressOf()
		)
	);
}

void SoundBuffer::update(const char* pData)
{
	DH_DEBUG_HR_DECL;

	char* bufferPtr;
	uint32_t bufferSize;

	DH_ASSERT_HR(
		m_pBuffer->Lock(
			0, m_desc.dwBufferBytes,
			(void**)&bufferPtr,
			(DWORD*)&bufferSize,
			NULL,
			0, 0
		)
	);

	memcpy(bufferPtr, pData, bufferSize);

	DH_ASSERT_HR(
		m_pBuffer->Unlock(
			(void*)bufferPtr,
			bufferSize,
			NULL, 0
		)
	);
}

void SoundBuffer::start(uint32_t block, bool loop)
{
	setCurrBlock(block);
	resume(loop);
}

void SoundBuffer::resume(bool loop)
{
	DH_DEBUG_HR_DECL;

	DWORD dwFlags = 0;
	if (loop)
		dwFlags |= DSBPLAY_LOOPING;

	DH_ASSERT_HR(
		m_pBuffer->Play(
			0, 0, dwFlags
		)
	);
}

void SoundBuffer::stop()
{
	DH_DEBUG_HR_DECL;

	DH_ASSERT_HR(
		m_pBuffer->Stop()
	);
}

void SoundBuffer::setVolume(float volume)
{
	DH_DEBUG_HR_DECL;

	DH_ASSERT_HR(
		m_pBuffer->SetVolume(
			normVolumeToDB(volume)
		)
	);
}

void SoundBuffer::setCurrBlock(uint32_t block)
{
	DH_DEBUG_HR_DECL;

	DH_ASSERT_HR(
		m_pBuffer->SetCurrentPosition(
			block * m_blockSize
		)
	);
}

bool SoundBuffer::isPlaying() const
{
	DH_DEBUG_HR_DECL;
	
	DWORD dwStatus;

	DH_THROW_HR_MSG(
		m_pBuffer->GetStatus(
			&dwStatus
		),
		"Unable to check buffer status"
	);

	return (dwStatus & DSBSTATUS_PLAYING);
}

uint32_t SoundBuffer::getCurrBlock() const
{
	DH_DEBUG_HR_DECL;

	DWORD pos;

	DH_ASSERT_HR(
		m_pBuffer->GetCurrentPosition(
			&pos,
			NULL
		)
	);
	
	return pos / m_blockSize;
}

std::shared_ptr<SoundBuffer> SoundBuffer::loadFromFile(SoundDevice* pDevice, const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::in | std::ios::binary);

	if (!file.good())
		throw std::exception("Cannot open sound file!");

	WaveFileHeader header;
	file.read((char*)&header, sizeof(WaveFileHeader));
	if (!eqArray(header.chunkId, "RIFF", 4) || !eqArray(header.format, "WAVE", 4))
		throw std::exception("Invalid file format!");
	if (!eqArray(header.subChunkId, "fmt ", 4))
		throw std::exception("Invalid chunk format!");
	DH_ASSERT(header.audioFormat == WAVE_FORMAT_PCM);
	if (header.audioFormat != WAVE_FORMAT_PCM)
		throw std::exception("Invalid wave format!");

	DH_ASSERT(eqArray(header.dataChunkId, "data", 4));

	std::shared_ptr<SoundBuffer> pSoundBuff = std::make_shared<SoundBuffer>(pDevice, header.dataSize, header.numChannels, header.sampleRate, header.bitsPerSample, true);

	file.seekg(sizeof(WaveFileHeader));
	auto waveData = new char[header.dataSize];

	file.read(waveData, header.dataSize);
	file.close();

	pSoundBuff->update(waveData);

	delete[] waveData;

	return pSoundBuff;
}
