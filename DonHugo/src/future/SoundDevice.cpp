#include "SoundDevice.h"

#include "Tools.h"

SoundDevice::SoundDevice(HWND hWnd, LPGUID lpGuid, const std::string& description)
	: m_lpGuid(lpGuid), m_description(description)
{
	DH_DEBUG_HR_DECL;
	DH_ASSERT_HR(
		DirectSoundCreate8(
			lpGuid,
			m_pDevice.GetAddressOf(),
			0
		)
	);
	DH_ASSERT_HR(
		m_pDevice->SetCooperativeLevel(
			hWnd,
			DSSCL_PRIORITY
		)
	);

	DSBUFFERDESC desc;
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	desc.dwBufferBytes = 0;
	desc.dwReserved = 0;
	desc.lpwfxFormat = NULL;
	desc.guid3DAlgorithm = GUID_NULL;

	DH_ASSERT_HR(
		m_pDevice->CreateSoundBuffer(
			&desc,
			m_pPrimBuff.GetAddressOf(),
			NULL
		)
	);

	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nSamplesPerSec = 44100;
	format.wBitsPerSample = 16;
	format.nChannels = 2;
	format.nBlockAlign = (format.wBitsPerSample / 8) * format.nChannels;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	format.cbSize = 0;

	DH_ASSERT_HR(
		m_pPrimBuff->SetFormat(
			&format
		)
	);
}

void SoundDevice::setVolume(float volume)
{
	DH_DEBUG_HR_DECL;

	DH_ASSERT_HR(
		m_pPrimBuff->SetVolume(
			normVolumeToDB(volume)
		)
	);
}

std::vector<SoundDeviceEnum> SoundDevice::getAvailDevices()
{
	DH_DEBUG_HR_DECL;

	std::vector<SoundDeviceEnum> devices;

	auto callback = [](LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext) -> BOOL
	{
		auto devices = (std::vector<SoundDeviceEnum>*)lpContext;
		SoundDeviceEnum deviceEnum;
		deviceEnum.lpGuid = lpGuid;
		deviceEnum.strDescription = lpcstrDescription;
		deviceEnum.strModule = lpcstrModule;

		devices->push_back(deviceEnum);

		return true;
	};

	DH_ASSERT_HR(DirectSoundEnumerate(callback, &devices));

	return devices;
}

LPGUID SoundDevice::getGuidFromDescription(const std::string strDescription)
{
	for (auto& device : getAvailDevices())
		if (device.strDescription == strDescription)
			return device.lpGuid;
	return 0;
}

LPGUID SoundDevice::getGuidFromModule(const std::string strModule)
{
	for (auto& device : getAvailDevices())
		if (device.strModule == strModule)
			return device.lpGuid;
	return 0;
}
