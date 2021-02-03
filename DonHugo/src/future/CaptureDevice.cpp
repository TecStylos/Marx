#include "CaptureDevice.h"

CaptureDevice::CaptureDevice(LPGUID lpGuid, const std::string& description)
	: m_lpGuid(lpGuid), m_description(description)
{
	DH_DEBUG_HR_DECL;

	DH_ASSERT_HR(
		DirectSoundCaptureCreate(
			lpGuid,
			m_pDevice.GetAddressOf(),
			0
		)
	);
}

std::vector<CaptureDeviceEnum> CaptureDevice::getAvailDevices()
{
	DH_DEBUG_HR_DECL;

	std::vector<CaptureDeviceEnum> devices;
	
	auto callback = [](LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext) -> BOOL
	{
		auto devices = (std::vector<CaptureDeviceEnum>*)lpContext;
		CaptureDeviceEnum deviceEnum;
		deviceEnum.lpGuid = lpGuid;
		deviceEnum.strDescription = lpcstrDescription;
		deviceEnum.strModule = lpcstrModule;

		devices->push_back(deviceEnum);

		return true;
	};

	DH_ASSERT_HR(DirectSoundCaptureEnumerate(callback, &devices));

	return devices;
}

LPGUID CaptureDevice::getGuidFromDescription(const std::string strDescription)
{
	for (auto& device : getAvailDevices())
		if (device.strDescription == strDescription)
			return device.lpGuid;
	return 0;
}

LPGUID CaptureDevice::getGuidFromModule(const std::string strModule)
{
	for (auto& device : getAvailDevices())
		if (device.strModule == strModule)
			return device.lpGuid;
	return 0;
}
