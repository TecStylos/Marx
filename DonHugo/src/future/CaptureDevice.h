#pragma once

#include "DirectSoundIncludes.h"

#include <vector>
#include <string>

struct CaptureDeviceEnum
{
	LPGUID lpGuid;
	std::string strDescription;
	std::string strModule;
};

class CaptureDevice
{
public:
	CaptureDevice(LPGUID lpGuid = NULL, const std::string& description = "Default Input Device");
	~CaptureDevice() = default;
public:
	IDirectSoundCapture* getDev() { return m_pDevice.Get(); }
	LPGUID getGuid() const { return m_lpGuid; }
	const std::string& getDescription() const { return m_description; }
public:
	static std::vector<CaptureDeviceEnum> getAvailDevices();
	static LPGUID getGuidFromDescription(const std::string strDescription);
	static LPGUID getGuidFromModule(const std::string strModule);
private:
	LPGUID m_lpGuid;
	std::string m_description;
	ComPtr<IDirectSoundCapture> m_pDevice;
};