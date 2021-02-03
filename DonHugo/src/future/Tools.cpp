#include "Tools.h"

bool eqArray(const void* arr1, const void* arr2, uint32_t nBytes)
{
	auto a = (const char*)arr1;
	auto b = (const char*)arr2;

	for (uint32_t i = 0; i < nBytes; ++i)
		if (a[i] != b[i])
			return false;
	return true;
}

float convertBytes(uint32_t nBytes, const char** sizeName)
{
	if (nBytes > 1000000000)
	{
		*sizeName = "GB";
		return nBytes / 1000000000.0f;
	}
	if (nBytes > 1000000)
	{
		*sizeName = "MB";
		return nBytes / 1000000.0f;
	}
	if (nBytes > 1000)
	{
		*sizeName = "KB";
		return nBytes / 1000.0f;
	}

	*sizeName = "B";
	return (float)nBytes;
}

long normVolumeToDB(float volume)
{
	long dbVolume = (long)(-10000.0f * (1.0f - volume));
	dbVolume = (long)(2000 * log10(volume));
	dbVolume = std::max(dbVolume, (long)-10000);
	return std::min(dbVolume, (long)0);
}

std::string extractNameFromFilepath(const std::string& filepath)
{
	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
	auto lastDot = filepath.rfind('.');
	auto count = (lastDot == std::string::npos) ? filepath.size() - lastSlash : lastDot - lastSlash;
	return filepath.substr(lastSlash, lastDot - lastSlash);
}
