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
		virtual void bind(uint32_t slot) const override;
	private:
		std::string m_path;
		uint32_t m_width;
		uint32_t m_height;
		ComPtr<ID3D11Texture2D> m_pTexture;
		ComPtr<ID3D11ShaderResourceView> m_pView;
		ComPtr<ID3D11SamplerState> m_pSampler;
	};
}