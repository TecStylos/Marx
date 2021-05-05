#include "mxpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include <glad/glad.h>

namespace Marx
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_path(path)
	{
		int width, height;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &m_nChannels, 4);
		MX_CORE_ASSERT(data, "Cannot load image file!");
		m_width = width;
		m_height = height;

		glGenTextures(1, &m_texture);
		glTexStorage2D(m_texture, 1, GL_RGBA8, m_width, m_height);
		//glTextureStorage2D(m_texture, 1, GL_RGBA8, m_width, m_height);

		glTexParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexSubImage2D(m_texture, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_texture);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glBindTexture(slot, m_texture);
	}

	void OpenGLTexture2D::update(const uint8_t* pData)
	{
		glTexSubImage2D(
			m_texture, 0,
			0, 0,
			m_width, m_height,
			GL_RGBA, GL_UNSIGNED_BYTE,
			pData
		);
	}

	void OpenGLTexture2D::updatePartial(const uint8_t* const* pData, const uint32_t* pOffsetX, const uint32_t* pOffsetY, const uint32_t* pWidth, const uint32_t* pHeight, uint32_t nBuffers)
	{
		for (uint32_t i = 0; i < nBuffers; ++i)
		{
			glTexSubImage2D(
				m_texture, 0,
				pOffsetX[i], pOffsetY[i],
				pWidth[i], pHeight[i],
				GL_RGBA, GL_UNSIGNED_BYTE,
				pData[i]
			);
		}
	}
}