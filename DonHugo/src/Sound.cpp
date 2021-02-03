#include "Sound.h"

Sound::Sound(SoundDevice* pDevice, const std::string& filepath)
	: m_filepath(filepath), m_volume(1.0f), m_loopingEnabled(false)
{
	m_pBuffer.reset(
		SoundBuffer::loadFromFile(
			pDevice,
			filepath
		)
	);
}

void Sound::enableLooping(bool loopingEnabled)
{
	m_loopingEnabled = loopingEnabled;

	if (isPlaying())
	{
		stop();
		resume();
	}
}

void Sound::setNewDevice(SoundDevice* pDevice)
{
	bool wasPlaying = isPlaying();
	uint32_t currBlock = getCurrBlock();
	if (wasPlaying)
		stop();

	m_pBuffer.reset(SoundBuffer::loadFromFile(pDevice, m_filepath));
	m_pBuffer->setVolume(m_volume);

	if (wasPlaying)
		resume();
}