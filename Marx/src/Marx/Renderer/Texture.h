#pragma once

#include "Marx/Core.h"

namespace Marx
{
	class Texture
	{
	public:
		virtual ~Texture() = default;
	public:
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual void bind(uint32_t slot = 0) const = 0;
	public:
		//virtual void update(void* data) = 0;
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;
		//virtual void updatePartial(void* data, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height) = 0;
	public:
		static Reference<Texture2D> create(const std::string& path);
	};
}