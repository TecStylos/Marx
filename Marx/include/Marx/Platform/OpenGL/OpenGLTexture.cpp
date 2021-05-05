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

		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		update(data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_texture);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}

	void OpenGLTexture2D::update(const uint8_t* pData)
	{
		bind(0);
		glTexSubImage2D(
			GL_TEXTURE_2D, 0,
			0, 0,
			m_width, m_height,
			GL_RGBA, GL_UNSIGNED_BYTE,
			pData
		);
	}

	void OpenGLTexture2D::updatePartial(const uint8_t* const* pData, const uint32_t* pOffsetX, const uint32_t* pOffsetY, const uint32_t* pWidth, const uint32_t* pHeight, uint32_t nBuffers)
	{
		bind(0);
		for (uint32_t i = 0; i < nBuffers; ++i)
		{
			glTexSubImage2D(
				GL_TEXTURE_2D, 0,
				pOffsetX[i], pOffsetY[i],
				pWidth[i], pHeight[i],
				GL_RGBA, GL_UNSIGNED_BYTE,
				pData[i]
			);
		}
	}
}