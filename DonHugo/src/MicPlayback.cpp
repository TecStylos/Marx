#include "MicPlayback.h"

MicPlayback::MicPlayback(CaptureDevice* pCaptureDev, SoundDevice* pSoundDev, uint32_t updateIntervalBlocks, uint32_t nChannels, uint32_t nBlocksPerSec, uint32_t nBytesPerSample)
{
	m_updateIntervalBlocks = updateIntervalBlocks;
	m_nBytesPerBlock = updateIntervalBlocks * nChannels * nBytesPerSample;

	uint32_t buffSize = m_nBytesPerBlock * nSoundEvents;
	uint32_t nSamplesPerSec = nBlocksPerSec;
	uint32_t nBitsPerSample = nBytesPerSample * 8;

	m_pCaptureBuffer.reset(new CaptureBuffer(pCaptureDev, buffSize, nChannels, nSamplesPerSec, nBitsPerSample));
	m_pSoundBuffer.reset(new SoundBuffer(pSoundDev, buffSize, nChannels, nSamplesPerSec, nBitsPerSample));

	setCaptureNotifications();
	setSoundNotifications();

	createDataBuffers();

	start();
}

MicPlayback::~MicPlayback()
{
	stop();

	clearCaptureNotifications();
	clearSoundNotifications();

	deleteDataBuffers();
}

void MicPlayback::start()
{
	m_pCaptureBuffer->start();
	m_pSoundBuffer->start(0, true);

	m_isRunning = true;
	m_readCaptureThread = std::thread(&MicPlayback::captureThreadFunc, this);
	m_writeSoundThread = std::thread(&MicPlayback::soundThreadFunc, this);
}

void MicPlayback::stop()
{
	m_pCaptureBuffer->stop();
	m_pSoundBuffer->stop();

	autoJoinThreads();
}

void MicPlayback::setVolume(float volume)
{}

void MicPlayback::setNewCaptureDevice(CaptureDevice* pDevice)
{
	bool wasRunning = m_isRunning;

	if (wasRunning)
		stop();

	clearCaptureNotifications();

	m_pCaptureBuffer->setNewDevice(pDevice);

	setCaptureNotifications();

	if (wasRunning)
		start();
}

void MicPlayback::setCaptureNotifications()
{
	DH_DEBUG_HR_DECL;

	ComPtr<IDirectSoundNotify> pNotify;
	std::array<DSBPOSITIONNOTIFY, nCaptureEvents> posNotifies;

	DH_ASSERT_HR(
		m_pCaptureBuffer->getBuffer()->QueryInterface(
			IID_IDirectSoundNotify,
			(LPVOID*)pNotify.GetAddressOf()
		)
	);

	for (uint32_t i = 0; i < nCaptureEvents; ++i)
	{
		DH_ASSERT(
			m_captureEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL)
		);

		posNotifies[i].dwOffset = (i + 1 == nCaptureEvents) ? DSBPN_OFFSETSTOP : (m_nBytesPerBlock * (i + 1) - 1);
		posNotifies[i].hEventNotify = m_captureEvents[i];
	}

	DH_ASSERT_HR(
		pNotify->SetNotificationPositions(
			nCaptureEvents,
			posNotifies.data()
		)
	);
}

void MicPlayback::setSoundNotifications()
{
	DH_DEBUG_HR_DECL;

	ComPtr<IDirectSoundNotify> pNotify;
	std::array<DSBPOSITIONNOTIFY, nSoundEvents> posNotifies;

	DH_ASSERT_HR(
		m_pSoundBuffer->getBuffer()->QueryInterface(
			IID_IDirectSoundNotify,
			(LPVOID*)pNotify.GetAddressOf()
		)
	);

	for (uint32_t i = 0; i < nSoundEvents; ++i)
	{
		DH_ASSERT(
			m_soundEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL)
		);

		posNotifies[i].dwOffset = m_nBytesPerBlock * (i + 1) - 1;
		posNotifies[i].hEventNotify = m_soundEvents[i];
	}

	DH_ASSERT_HR(
		pNotify->SetNotificationPositions(
			nSoundEvents,
			posNotifies.data()
		)
	);
}

void MicPlayback::clearCaptureNotifications()
{
	for (uint32_t i = 0; i < nCaptureEvents; ++i)
		CloseHandle(m_captureEvents[i]);
	
}

void MicPlayback::clearSoundNotifications()
{
	for (uint32_t i = 0; i < nSoundEvents; ++i)
		CloseHandle(m_soundEvents[i]);
}

void MicPlayback::createDataBuffers()
{
	deleteDataBuffers();

	{
		std::lock_guard<std::mutex> guard(m_freeDataBuffersMutex);

		for (uint32_t i = 0; i < nBlocks; ++i)
		{
			m_freeDataBuffers.push(new char[m_nBytesPerBlock]);
		}
	}
}

void MicPlayback::deleteDataBuffers()
{
	{
		std::lock_guard<std::mutex> guard(m_fullDataBuffersMutex);

		while (!m_fullDataBuffers.empty())
		{
			delete m_fullDataBuffers.front();
			m_fullDataBuffers.pop();
		}
	}
	{
		std::lock_guard<std::mutex> guard(m_freeDataBuffersMutex);

		while (!m_freeDataBuffers.empty())
		{
			delete m_freeDataBuffers.front();
			m_freeDataBuffers.pop();
		}
	}
}

void MicPlayback::autoJoinThreads()
{
	if (m_isRunning)
	{
		m_isRunning = false;
		m_readCaptureThread.join();
		m_writeSoundThread.join();
	}
}

void MicPlayback::captureThreadFunc()
{
	do
	{
		DWORD dwResult = WaitForMultipleObjects(nCaptureEvents, m_captureEvents.data(), false, 10) - WAIT_OBJECT_0;
		if (dwResult == nCaptureEvents - 1)
		{
			m_isRunning = false;
		}
		else if (dwResult < nCaptureEvents)
		{
			readCaptureBuffer(dwResult);
			ResetEvent(m_captureEvents[dwResult]);
		}
	} while (m_isRunning);
}

void MicPlayback::soundThreadFunc()
{
	do
	{
		DWORD dwResult = WaitForMultipleObjects(nSoundEvents, m_soundEvents.data(), false, 10) - WAIT_OBJECT_0;
		if (dwResult < nSoundEvents)
		{
			writeSoundBuffer(dwResult);
			ResetEvent(m_soundEvents[dwResult]);
		}
	} while (m_isRunning);
}

void MicPlayback::readCaptureBuffer(DWORD block)
{
	DH_DEBUG_HR_DECL;

	uint32_t nLockFails = 0;

	char* buffer = 0;

	{
		std::lock_guard<std::mutex> guard(m_freeDataBuffersMutex);

		if (!m_freeDataBuffers.empty())
		{
			buffer = m_freeDataBuffers.front();
			m_freeDataBuffers.pop();
		}
	}

	if (buffer)
	{
		LPVOID pCaptureData;
		DWORD captureLen;

		if (SUCCEEDED(
			m_pCaptureBuffer->getBuffer()->Lock(
				block * m_nBytesPerBlock, m_nBytesPerBlock,
				&pCaptureData, &captureLen,
				NULL, 0, 0
			)))
		{
			memcpy(buffer, pCaptureData, captureLen);

			DH_ASSERT_HR(
				m_pCaptureBuffer->getBuffer()->Unlock(
					pCaptureData, captureLen,
					NULL, 0
				)
			);
		}
		else
		{
			++nLockFails;
		}

		{
			std::lock_guard<std::mutex> guard(m_fullDataBuffersMutex);
			m_fullDataBuffers.push(buffer);
		}
	}
}

void MicPlayback::writeSoundBuffer(DWORD block)
{
	DH_DEBUG_HR_DECL;

	static uint32_t nLockFails = 0;

	char* buffer = 0;

	{
		std::lock_guard<std::mutex> guard(m_fullDataBuffersMutex);

		if (!m_fullDataBuffers.empty())
		{
			buffer = m_fullDataBuffers.front();
			m_fullDataBuffers.pop();
		}
	}

	if (buffer)
	{
		LPVOID pSoundData;
		DWORD soundLen;

		if (SUCCEEDED(
			m_pSoundBuffer->getBuffer()->Lock(
				block * m_nBytesPerBlock, m_nBytesPerBlock,
				&pSoundData, &soundLen,
				NULL, 0, 0
			)))
		{
			memcpy(pSoundData, buffer, soundLen);

			DH_ASSERT_HR(
				m_pSoundBuffer->getBuffer()->Unlock(
					pSoundData, soundLen,
					NULL, 0
				)
			);
		}
		else
		{
			++nLockFails;
		}

		{
			std::lock_guard<std::mutex> guard(m_freeDataBuffersMutex);
			m_freeDataBuffers.push(buffer);
		}
	}
}
