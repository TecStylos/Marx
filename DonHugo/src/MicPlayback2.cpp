#include "MicPlayback2.h"

MicPlayback2::MicPlayback2(const MicPlayback2Desc* desc)
{
	DH_ASSERT(!s_instanceExists);
	DH_ASSERT(!desc->enableEffects || ((float)desc->nBlocksPerUpdate / (float)desc->nBlocksPerSec) >= 0.15f);

	s_instanceExists = true;

	m_desc = *desc;
	m_nBytesPerBlock = desc->nBlocksPerUpdate * desc->nChannels * desc->nBytesPerSample;

	uint32_t buffSize = m_nBytesPerBlock * nSoundEvents;
	uint32_t nSamplesPerSec = desc->nBlocksPerSec;
	uint32_t nBitsPerSample = desc->nBytesPerSample * 8;

	m_pCaptureBuffer.reset(new CaptureBuffer(desc->pCaptureDev, buffSize, desc->nChannels, nSamplesPerSec, nBitsPerSample));
	m_pSoundBuffer[0].reset(new SoundBuffer(desc->pSoundDev[0], buffSize, desc->nChannels, nSamplesPerSec, nBitsPerSample, desc->enableEffects));
	m_pSoundBuffer[1].reset(new SoundBuffer(desc->pSoundDev[1], buffSize, desc->nChannels, nSamplesPerSec, nBitsPerSample, desc->enableEffects));

	setCaptureNotifications();
	setSoundNotifications();

	createDataBuffers();

	start();

	startThreads();
}

MicPlayback2::~MicPlayback2()
{
	stopThreads();

	stop();

	clearCaptureNotifications();
	clearSoundNotifications();

	deleteDataBuffers();
	s_instanceExists = false;
}

void MicPlayback2::setVolume(float volume)
{
	m_volume = volume;
	m_pSoundBuffer[0]->setVolume(volume * m_volumeMultiplier[0]);
	m_pSoundBuffer[1]->setVolume(volume * m_volumeMultiplier[1]);
}

void MicPlayback2::setVolumeMultiplier1(float m)
{
	m_volumeMultiplier[0] = m;
	m_pSoundBuffer[0]->setVolume(m_volume * m);
}

void MicPlayback2::setVolumeMultiplier2(float m)
{
	m_volumeMultiplier[1] = m;
	m_pSoundBuffer[1]->setVolume(m_volume * m);
}

void MicPlayback2::setNewCaptureDevice(CaptureDevice* pDevice)
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

bool MicPlayback2::effectsEnabled() const
{
	return m_desc.enableEffects;
}

void MicPlayback2::addEffect(std::shared_ptr<Effect> pEffect)
{
	m_effects.addEffect(pEffect);
	applyEffects();
}

void MicPlayback2::removeEffect(EffectType type)
{
	m_effects.removeEffect(type);
	applyEffects();
}

void MicPlayback2::applyEffects()
{
	bool wasPlaying = isPlaying();
	if (wasPlaying)
		stop();


	if (m_pSoundBuffer[0])
	{
		std::lock_guard<std::mutex> guard(m_lockBufferMutex[0]);
		m_effects.applyEffects(m_pSoundBuffer[0].get(), !m_desc.enableEffects);
	}
	if (m_pSoundBuffer[1])
	{
		std::lock_guard<std::mutex> guard(m_lockBufferMutex[1]);
		m_effects.applyEffects(m_pSoundBuffer[1].get(), !m_desc.enableEffects);
	}

	if (wasPlaying)
		start();
}

void MicPlayback2::updateEffect(EffectType type)
{
	if (m_pSoundBuffer[0]) m_effects.updateEffect(m_pSoundBuffer[0].get(), type);
	if (m_pSoundBuffer[1]) m_effects.updateEffect(m_pSoundBuffer[1].get(), type);
}

void MicPlayback2::swapEffects(uint32_t index1, uint32_t index2)
{
	m_effects.swapEffects(index1, index2);
	applyEffects();
}

bool MicPlayback2::hasEffect(EffectType type)
{
	return m_effects.hasEffect(type);
}

EffectList* MicPlayback2::getEffectList()
{
	return &m_effects;
}

bool MicPlayback2::isCapturing() const
{
	return m_pCaptureBuffer ? m_pCaptureBuffer->isCapturing() : false;
}

bool MicPlayback2::isPlaying() const
{
	return
		m_pSoundBuffer[0] ? m_pSoundBuffer[0]->isPlaying() :
		m_pSoundBuffer[1] ? m_pSoundBuffer[1]->isPlaying() : false;
}

void MicPlayback2::start()
{
	m_pCaptureBuffer->start();
	m_pSoundBuffer[0]->start(0, true);
	m_pSoundBuffer[1]->start(0, true);
}

void MicPlayback2::startThreads()
{
	m_isRunning = true;
	m_readCaptureThread = std::thread(&MicPlayback2::captureThreadFunc, this);
	m_writeSoundThread[0] = std::thread(&MicPlayback2::soundThreadFunc, this, 0);
	m_writeSoundThread[1] = std::thread(&MicPlayback2::soundThreadFunc, this, 1);
}

void MicPlayback2::stop()
{
	m_pCaptureBuffer->stop();
	m_pSoundBuffer[0]->stop();
	m_pSoundBuffer[1]->stop();
}

void MicPlayback2::stopThreads()
{
	autoJoinThreads();
}

void MicPlayback2::setCaptureNotifications()
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

void MicPlayback2::setSoundNotifications()
{
	DH_DEBUG_HR_DECL;

	ComPtr<IDirectSoundNotify> pNotify[2];
	std::array<DSBPOSITIONNOTIFY, nSoundEvents> posNotifies[2];

	DH_ASSERT_HR(
		m_pSoundBuffer[0]->getBuffer()->QueryInterface(
			IID_IDirectSoundNotify,
			(LPVOID*)pNotify[0].GetAddressOf()
		)
	);
	DH_ASSERT_HR(
		m_pSoundBuffer[1]->getBuffer()->QueryInterface(
			IID_IDirectSoundNotify,
			(LPVOID*)pNotify[1].GetAddressOf()
		)
	);

	for (uint32_t i = 0; i < nSoundEvents; ++i)
	{
		DH_ASSERT(
			m_soundEvents[0][i] = CreateEvent(NULL, TRUE, FALSE, NULL)
		);
		posNotifies[0][i].dwOffset = m_nBytesPerBlock * (i + 1) - 1;
		posNotifies[0][i].hEventNotify = m_soundEvents[0][i];

		DH_ASSERT(
			m_soundEvents[1][i] = CreateEvent(NULL, TRUE, FALSE, NULL)
		);
		posNotifies[1][i].dwOffset = m_nBytesPerBlock * (i + 1) - 1;
		posNotifies[1][i].hEventNotify = m_soundEvents[1][i];
	}

	DH_ASSERT_HR(
		pNotify[0]->SetNotificationPositions(
			nSoundEvents,
			posNotifies[0].data()
		)
	);
	DH_ASSERT_HR(
		pNotify[1]->SetNotificationPositions(
			nSoundEvents,
			posNotifies[1].data()
		)
	);
}

void MicPlayback2::clearCaptureNotifications()
{
	for (uint32_t i = 0; i < nCaptureEvents; ++i)
		CloseHandle(m_captureEvents[i]);
}

void MicPlayback2::clearSoundNotifications()
{
	for (uint32_t i = 0; i < nSoundEvents; ++i)
	{
		CloseHandle(m_soundEvents[0][i]);
		CloseHandle(m_soundEvents[1][i]);
	}
}

void MicPlayback2::createDataBuffers()
{
	deleteDataBuffers();

	for (uint32_t index = 0; index < 2; ++index)
	{
		std::lock_guard<std::mutex> guard(m_freeDataBuffersMutex[index]);

		for (uint32_t i = 0; i < nBlocks; ++i)
		{
			m_freeDataBuffers[index].push(new char[m_nBytesPerBlock]);
		}
	}
}

void MicPlayback2::deleteDataBuffers()
{
	for (uint32_t index = 0; index < 2; ++index)
	{
		{
			std::lock_guard<std::mutex> guard(m_fullDataBuffersMutex[index]);

			while (!m_fullDataBuffers[index].empty())
			{
				delete[] m_fullDataBuffers[index].front();
				m_fullDataBuffers[index].pop();
			}
		}
		{
			std::lock_guard<std::mutex> guard(m_freeDataBuffersMutex[index]);

			while (!m_freeDataBuffers[index].empty())
			{
				delete[] m_freeDataBuffers[index].front();
				m_freeDataBuffers[index].pop();
			}
		}
	}
}

void MicPlayback2::autoJoinThreads()
{
	if (m_isRunning)
	{
		m_isRunning = false;
		m_readCaptureThread.join();
		m_writeSoundThread[0].join();
		m_writeSoundThread[1].join();
	}
}

void MicPlayback2::captureThreadFunc()
{
	try
	{
		do
		{
			DWORD dwResult = WaitForMultipleObjects(nCaptureEvents, m_captureEvents.data(), false, 10) - WAIT_OBJECT_0;
			if (dwResult < nCaptureEvents)
			{
				readCaptureBuffer(dwResult);
				ResetEvent(m_captureEvents[dwResult]);
			}
		} while (m_isRunning);
	}
	catch (std::exception& e)
	{
		std::string what = e.what();
		__debugbreak();
	}
}

void MicPlayback2::soundThreadFunc(uint32_t index)
{
	try
	{
		do
		{
			DWORD dwResult = WaitForMultipleObjects(nSoundEvents, m_soundEvents[index].data(), false, 10) - WAIT_OBJECT_0;
			if (dwResult < nSoundEvents)
			{
				writeSoundBuffer(dwResult, index);
				ResetEvent(m_soundEvents[index][dwResult]);
			}
		} while (m_isRunning);
	}
	catch (std::exception& e)
	{
		std::string what = e.what();
		__debugbreak();
	}
}

void MicPlayback2::readCaptureBuffer(DWORD block)
{
	DH_DEBUG_HR_DECL;

	uint32_t nLockFails = 0;

	char* buffer[2] = { 0, 0 };

	for (uint32_t index = 0; index < 2; ++index)
	{
		std::lock_guard<std::mutex> guard(m_freeDataBuffersMutex[index]);

		if (!m_freeDataBuffers[index].empty())
		{
			buffer[index] = m_freeDataBuffers[index].front();
			m_freeDataBuffers[index].pop();
		}
	}

	if (buffer[0] || buffer[1])
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
			if (buffer[0]) memcpy(buffer[0], pCaptureData, captureLen);
			if (buffer[1]) memcpy(buffer[1], pCaptureData, captureLen);

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

		for (uint32_t index = 0; index < 2; ++index)
		{
			if (buffer[index])
			{
				std::lock_guard<std::mutex> guard(m_fullDataBuffersMutex[index]);
				m_fullDataBuffers[index].push(buffer[index]);
			}
		}
	}
}

void MicPlayback2::writeSoundBuffer(DWORD block, uint32_t index)
{
	DH_DEBUG_HR_DECL;

	static uint32_t nLockFails = 0;

	char* buffer = 0;

	{
		std::lock_guard<std::mutex> guard(m_fullDataBuffersMutex[index]);

		if (!m_fullDataBuffers[index].empty())
		{
			buffer = m_fullDataBuffers[index].front();
			m_fullDataBuffers[index].pop();
		}
	}

	if (buffer)
	{
		LPVOID pSoundData;
		DWORD soundLen;

		std::lock_guard<std::mutex> guard(m_lockBufferMutex[index]);

		if (SUCCEEDED(
			m_pSoundBuffer[index]->getBuffer()->Lock(
				block * m_nBytesPerBlock, m_nBytesPerBlock,
				&pSoundData, &soundLen,
				NULL, 0, 0
			)))
		{
			memcpy(pSoundData, buffer, soundLen);

			DH_ASSERT_HR(
				m_pSoundBuffer[index]->getBuffer()->Unlock(
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
			std::lock_guard<std::mutex> guard(m_freeDataBuffersMutex[index]);
			m_freeDataBuffers[index].push(buffer);
		}
	}
}
