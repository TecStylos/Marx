#pragma once

#include "DirectSoundIncludes.h"

#include <vector>
#include <string>

struct CaptureDeviceEnum
{
	GUID guid;
	std::string strDescription;
	std::string strModule;
};

class CaptureDevice
{
public:
	CaptureDevice(GUID guid, const std::string& description);
	~CaptureDevice() = default;
public:
	IDirectSoundCapture* getDev() { return m_pDevice.Get(); }
	LPGUID getLPGuid() { return &m_guid; }
	const std::string& getDescription() const { return m_description; }
public:
	static std::vector<CaptureDeviceEnum> getAvailDevices();
	static GUID getGuidFromDescription(const std::string strDescription);
	static GUID getGuidFromModule(const std::string strModule);
private:
	GUID m_guid;
	std::string m_description;
	ComPtr<IDirectSoundCapture> m_pDevice;
};