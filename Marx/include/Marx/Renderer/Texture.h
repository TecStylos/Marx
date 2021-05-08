#pragma once

#include "Marx/Core.h"

namespace Marx
{
	class TextureLoadException : public std::runtime_error
	{
	public:
		TextureLoadException(const char* msg)
			: std::runtime_error(msg)
		{}
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;
	public:
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getNumChannels() const = 0;
		virtual void bind(uint32_t slot = 0) const = 0;
	public:
		// Updates the whole texture data.
		//
		// @param pData The new data for the texture (The size of the buffer must be equal to the size of the texture)
		virtual void update(const unsigned char* pData) = 0;
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;
		// Updates one or more parts of the underlying texture.
		//
		// @param pPixels Array of pixel buffers
		// @param pOffsetX Array of offsets along the x-axis (in pixels not bytes)
		// @param pOffsetY Array of offsets along the y-axis (in pixels not bytes)
		// @param pWidth Array of widths for each pixel buffer (in pixels not bytes)
		// @param pHeight Array of heights for each pixel buffer (int pixels not bytes)
		// @param nBuffers The number of buffers in pPixels
		virtual void updatePartial(const unsigned char* const* pPixels, const uint32_t* pOffsetX, const uint32_t* pOffsetY, const uint32_t* pWidth, const uint32_t* pHeight, uint32_t nBuffers) = 0;
	public:
		static Reference<Texture2D> create(const std::string& path);
	};
}