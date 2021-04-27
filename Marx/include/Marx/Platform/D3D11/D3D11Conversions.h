#pragma once

#include "Marx/Platform/DX11/DX11Includes.h"
#include "Marx/Platform/Win32/Win32Includes.h"

namespace Marx
{
	static DXGI_FORMAT shaderDataTypeToDXGI_FORMAT(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:   return DXGI_FORMAT_UNKNOWN; // Unknown but supported
		case ShaderDataType::Float1: return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Mat3:   return DXGI_FORMAT_UNKNOWN; // Unknown but supported
		case ShaderDataType::Mat4:   return DXGI_FORMAT_UNKNOWN; // Unknown but supported
		case ShaderDataType::Int1:   return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:   return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:   return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:   return DXGI_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::Bool1:  return DXGI_FORMAT_UNKNOWN; // Unknown but supported
		}

		MX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return DXGI_FORMAT_UNKNOWN;
	}

	static std::string shaderDataTypeToHLSL_String(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:   break; // Unsupported
		case ShaderDataType::Float1: return "float1";
		case ShaderDataType::Float2: return "float2";
		case ShaderDataType::Float3: return "float3";
		case ShaderDataType::Float4: return "float4";
		case ShaderDataType::Mat3:   return "float3x3";
		case ShaderDataType::Mat4:   return "float4x4";
		case ShaderDataType::Int1:   return "int1";
		case ShaderDataType::Int2:   return "int2";
		case ShaderDataType::Int3:   return "int3";
		case ShaderDataType::Int4:   return "int4";
		case ShaderDataType::Bool1:  return "bool1";
		}

		MX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return "";
	}

	static ShaderDataType hlslStringToShaderDataType(const std::string& typeStr)
	{
		if ("float1" == typeStr)
			return ShaderDataType::Float1;
		else if ("float2" == typeStr)
			return ShaderDataType::Float2;
		else if ("float3" == typeStr)
			return ShaderDataType::Float3;
		else if ("float4" == typeStr)
			return ShaderDataType::Float4;

		else if ("float3x3" == typeStr)
			return ShaderDataType::Mat3;
		else if ("float4x4" == typeStr)
			return ShaderDataType::Mat4;

		else if ("int1" == typeStr)
			return ShaderDataType::Int1;
		else if ("int2" == typeStr)
			return ShaderDataType::Int2;
		else if ("int3" == typeStr)
			return ShaderDataType::Int3;
		else if ("int4" == typeStr)
			return ShaderDataType::Int4;

		else if ("bool1" == typeStr)
			return ShaderDataType::Bool1;

		MX_CORE_ASSERT(false, "Unknown ShaderDataType string!");
		return ShaderDataType::None;
	}
}