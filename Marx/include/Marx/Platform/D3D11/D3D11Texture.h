#pragma once

#include "Marx/Renderer/Texture.h"

#include "D3D11GraphicsContext.h"

namespace Marx
{
	class D3D11Texture2D : public Texture2D
	{
	public:
		D3D11Texture2D(const std::string& path);
		virtual ~D3D11Texture2D() = default;
	public:
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual uint32_t getNumChannels() const override { return m_nChannels; }
		virtual void bind(uint32_t slot) const override;
		virtual void update(const unsigned char* pData) override;
		virtual void updatePartial(const unsigned char* const* pData, const uint32_t* pOffsetX, const uint32_t* pOffsetY, const uint32_t* pWidth, const uint32_t* pHeight, uint32_t nBuffers) override;
	private:
		std::string m_path;
		uint32_t m_width;
		uint32_t m_height;
		int m_nChannels;
		ComPtr<ID3D11Texture2D> m_pTexture;
		ComPtr<ID3D11ShaderResourceView> m_pView;
		ComPtr<ID3D11SamplerState> m_pSampler;
	};
}