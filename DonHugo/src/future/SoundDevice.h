#pragma once

#include "DirectSoundIncludes.h"

#include <vector>
#include <string>

struct SoundDeviceEnum
{
	LPGUID lpGuid;
	std::string strDescription;
	std::string strModule;
};

class SoundDevice
{
public:
	SoundDevice(HWND hWnd, LPGUID lpGuid = NULL, const std::string& description = "Default Output Device");
	~SoundDevice() = default;
public:
	void setVolume(float volume);
public:
	IDirectSound8* getDev() { return m_pDevice.Get(); }
public:
	LPGUID getGuid() const { return m_lpGuid; }
	const std::string& getDescription() const { return m_description; }
public:
	static std::vector<SoundDeviceEnum> getAvailDevices();
	static LPGUID getGuidFromDescription(const std::string strDescription);
	static LPGUID getGuidFromModule(const std::string strModule);
private:
	LPGUID m_lpGuid;
	std::string m_description;
private:
	ComPtr<IDirectSound8> m_pDevice;
	ComPtr<IDirectSoundBuffer> m_pPrimBuff;
};