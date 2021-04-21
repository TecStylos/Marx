#include "mxpch.h"
#include "D3D11Texture.h"

#include "stb_image.h"

#include "Marx/Platform/D3D11/D3D11InfoManager.h"
#include "Marx/Platform/D3D11/D3D11InfoException.h"
#include "Marx/Platform/D3D11/D3D11ExceptionMacros.h"

namespace Marx
{
	D3D11Texture2D::D3D11Texture2D(const std::string& path)
		: m_path(path)
	{
		MX_DEBUG_HR_DECL;

		int width, height;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &m_nChannels, 4);
		if (!data)
			throw TextureLoadException("Cannot load image file!");
		m_width = width;
		m_height = height;
		
		DXGI_FORMAT texFormat = (DXGI_FORMAT)0;
		m_nChannels = 4; //if (m_nChannels == 4)
		texFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (!texFormat)
			TextureLoadException("Invalid image format!");

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = texDesc.ArraySize = 1;
		texDesc.Format = texFormat;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA pInitData;
		pInitData.pSysMem = data;
		pInitData.SysMemPitch = width * m_nChannels * sizeof(char);
		pInitData.SysMemSlicePitch = 0;

		MX_VERIFY_THROW_HR_INFO(
			D3D11GraphicsContext::D3D11Manager::getDevice()->CreateTexture2D(
				&texDesc,
				&pInitData,
				m_pTexture.GetAddressOf()
			)
		);

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texFormat;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipLevels = 1;
		viewDesc.Texture2D.MostDetailedMip = 0;

		MX_VERIFY_THROW_HR_INFO(
			D3D11GraphicsContext::D3D11Manager::getDevice()->CreateShaderResourceView(
				m_pTexture.Get(),
				&viewDesc,
				m_pView.GetAddressOf()
			)
		);

		stbi_image_free(data);

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0.8f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.8f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		MX_VERIFY_THROW_HR_INFO(
			D3D11GraphicsContext::D3D11Manager::getDevice()->CreateSamplerState(
				&samplerDesc,
				m_pSampler.GetAddressOf()
			)
		);
	}

	void D3D11Texture2D::bind(uint32_t slot) const
	{
		D3D11GraphicsContext::D3D11Manager::getContext()->PSSetShaderResources(slot, 1, m_pView.GetAddressOf());
		D3D11GraphicsContext::D3D11Manager::getContext()->PSSetSamplers(slot, 1, m_pSampler.GetAddressOf());
	}

	void D3D11Texture2D::update(const BYTE* pData)
	{
		uint32_t offsetX = 0;
		uint32_t offsetY = 0;
		updatePartial(&pData, &offsetX, &offsetY, &m_width, &m_height, 1);
	}

	void D3D11Texture2D::updatePartial(const BYTE* const* pData, const uint32_t* pOffsetX, const uint32_t* pOffsetY, const uint32_t* pWidth, const uint32_t* pHeight, uint32_t nBuffers)
	{
		for (uint32_t i = 0; i < nBuffers; ++i)
		{
			D3D11_BOX region;
			region.left = pOffsetX[i];
			region.right = pOffsetX[i] + pWidth[i];
			region.top = pOffsetY[i];
			region.bottom = pOffsetY[i] + pHeight[i];
			region.front = 0;
			region.back = 1;
			D3D11GraphicsContext::D3D11Manager::getContext()->UpdateSubresource(m_pTexture.Get(), 0, &region, pData[i], pWidth[i] * m_nChannels * sizeof(char), 0);
		}
	}
}