#pragma once

#define INITGUID

#include <mmreg.h>
#include <dsound.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <comdef.h>

#include <exception>

#define DH_DEBUG_HR_DECL HRESULT hr
#define DH_ASSERT_HR(r) { if (FAILED(hr = (r))) { _com_error err(hr); LPCTSTR errMsg = err.ErrorMessage(); __debugbreak(); } }
#define DH_THROW_HR(r) { if (FAILED(hr = (r))) { _com_error err(hr); LPCTSTR errMsg = err.ErrorMessage(); throw std::exception(errMsg); } }
#define DH_THROW_HR_MSG(r, msg) { if (FAILED(hr = (r))) { throw std::exception(msg); } }
#define DH_ASSERT(r) { if (!(r)) __debugbreak(); }