#pragma once

#include "DirectSoundIncludes.h"

#include <vector>
#include <string>

struct SoundDeviceEnum
{
	GUID guid;
	std::string strDescription;
	std::string strModule;
};

class SoundDevice
{
public:
	SoundDevice(HWND hWnd, GUID guid, const std::string& description);
	~SoundDevice() = default;
public:
	void setVolume(float volume);
public:
	IDirectSound8* getDev() { return m_pDevice.Get(); }
public:
	LPGUID getLPGuid() { return &m_guid; }
	const std::string& getDescription() const { return m_description; }
public:
	static std::vector<SoundDeviceEnum> getAvailDevices();
	static GUID getGuidFromDescription(const std::string strDescription);
	static GUID getGuidFromModule(const std::string strModule);
private:
	GUID m_guid;
	std::string m_description;
private:
	ComPtr<IDirectSound8> m_pDevice;
	ComPtr<IDirectSoundBuffer> m_pPrimBuff;
};