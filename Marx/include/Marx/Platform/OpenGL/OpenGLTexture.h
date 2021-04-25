#pragma once

#include <string>

#include "Marx/Renderer/Texture.h"

namespace Marx
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();
	public:
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual uint32_t getNumChannels() const override { return m_nChannels; }
		virtual void bind(uint32_t slot) const override;
		virtual void update(const uint8_t* pData) override;
		virtual void updatePartial(const uint8_t* const* pData, const uint32_t* pOffsetX, const uint32_t* pOffsetY, const uint32_t* pWidth, const uint32_t* pHeight, uint32_t nBuffers) override;
	private:
		std::string m_path;
		uint32_t m_width;
		uint32_t m_height;
		int m_nChannels;
		uint32_t m_texture;
	};
}