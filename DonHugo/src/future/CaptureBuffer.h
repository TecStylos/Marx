#pragma once

#include "DirectSoundIncludes.h"

#include "CaptureDevice.h"

class CaptureBuffer
{
public:
	CaptureBuffer(CaptureDevice* pDevice, uint32_t size, uint32_t nChannels = 1, uint32_t nSamplesPerSec = 8000, uint32_t nBitsPerSample = 8);
	~CaptureBuffer() = default;
public:
	void start() { DH_DEBUG_HR_DECL;  DH_ASSERT_HR(m_pBuffer->Start(DSCBSTART_LOOPING)); }
	void stop() { DH_DEBUG_HR_DECL;  DH_ASSERT_HR(m_pBuffer->Stop()); }
public:
	void setNewDevice(CaptureDevice* pDevice);
public:
	bool isCapturing() const;
	uint32_t getSize() const { return m_desc.dwBufferBytes; }
	uint32_t getNumChannels() const { return m_format.nChannels; }
	uint32_t getSampleRate() const { return m_format.nSamplesPerSec; }
	uint32_t getBitsPerSample() const { return m_format.wBitsPerSample; }
public:
	IDirectSoundCaptureBuffer* getBuffer() { return m_pBuffer.Get(); }
private:
	WAVEFORMATEX m_format;
	DSCBUFFERDESC m_desc;
private:
	ComPtr<IDirectSoundCaptureBuffer> m_pBuffer;
};