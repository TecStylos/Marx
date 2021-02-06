#include "CaptureBuffer.h"

CaptureBuffer::CaptureBuffer(CaptureDevice* pDevice, uint32_t size, uint32_t nChannels, uint32_t nSamplesPerSec, uint32_t nBitsPerSample)
{
	m_format.wFormatTag = WAVE_FORMAT_PCM;
	m_format.nChannels = nChannels;
	m_format.nSamplesPerSec = nSamplesPerSec;
	m_format.wBitsPerSample = nBitsPerSample;
	m_format.nBlockAlign = (m_format.nChannels * m_format.wBitsPerSample) / 8;
	m_format.nAvgBytesPerSec = m_format.nSamplesPerSec * m_format.nBlockAlign;
	m_format.cbSize = 0;

	m_desc.dwSize = sizeof(DSCBUFFERDESC);
	m_desc.dwFlags = DSCBCAPS_WAVEMAPPED;
	m_desc.dwBufferBytes = size;
	m_desc.dwReserved = 0;
	m_desc.lpwfxFormat = &m_format;
	m_desc.dwFXCount = 0;
	m_desc.lpDSCFXDesc = 0;

	setNewDevice(pDevice);
}

void CaptureBuffer::setNewDevice(CaptureDevice* pDevice)
{
	DH_DEBUG_HR_DECL;

	IDirectSoundCaptureBuffer* pNewBuffer = nullptr;

	DH_ASSERT_HR(
		pDevice->getDev()->CreateCaptureBuffer(
			&m_desc,
			&pNewBuffer,
			0
		)
	);

	if (pNewBuffer)
		m_pBuffer = pNewBuffer;
}

bool CaptureBuffer::isCapturing() const
{
	DH_DEBUG_HR_DECL;

	DWORD dwStatus;

	DH_ASSERT_HR(
		m_pBuffer->GetStatus(
			&dwStatus
		)
	);

	return dwStatus & DSCBSTATUS_CAPTURING;
}