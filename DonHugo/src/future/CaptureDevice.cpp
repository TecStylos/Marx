#include "CaptureDevice.h"

CaptureDevice::CaptureDevice(GUID guid, const std::string& description)
	: m_guid(guid), m_description(description)
{
	DH_DEBUG_HR_DECL;

	DH_THROW_HR_MSG(
		DirectSoundCaptureCreate(
			&m_guid,
			m_pDevice.GetAddressOf(),
			0
		),
		"Unable to create capture device!"
	);
}

std::vector<CaptureDeviceEnum> CaptureDevice::getAvailDevices()
{
	DH_DEBUG_HR_DECL;

	std::vector<CaptureDeviceEnum> devices;
	
	auto callback = [](LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext) -> BOOL
	{
		if (lpGuid != NULL)
		{
			auto devices = (std::vector<CaptureDeviceEnum>*)lpContext;
			CaptureDeviceEnum deviceEnum;
			memcpy(&deviceEnum.guid, lpGuid, sizeof(GUID));
			deviceEnum.strDescription = lpcstrDescription;
			deviceEnum.strModule = lpcstrModule;

			devices->push_back(deviceEnum);
		}

		return true;
	};

	DH_ASSERT_HR(DirectSoundCaptureEnumerate(callback, &devices));

	return devices;
}

GUID CaptureDevice::getGuidFromDescription(const std::string strDescription)
{
	for (auto& device : getAvailDevices())
		if (device.strDescription == strDescription)
			return device.guid;
	return GUID();
}

GUID CaptureDevice::getGuidFromModule(const std::string strModule)
{
	for (auto& device : getAvailDevices())
		if (device.strModule == strModule)
			return device.guid;
	return GUID();
}
