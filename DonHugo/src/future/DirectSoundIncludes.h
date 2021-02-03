#pragma once

#define INITGUID

#include <dsound.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#pragma comment(lib, "dsound.lib")

#include <comdef.h>

#define DH_DEBUG_HR_DECL HRESULT hr
#define DH_ASSERT_HR(r) { if (FAILED(hr = (r))) { _com_error err(hr); LPCTSTR errMsg = err.ErrorMessage(); __debugbreak(); } }
#define DH_ASSERT(r) { if (!(r)) __debugbreak(); }