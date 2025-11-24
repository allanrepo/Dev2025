#pragma once
#include <string>
#include "ITexture.h"

namespace graphics::resource
{
	class TextureImpl : public graphics::resource::ITexture
	{
	public:
		TextureImpl() = default;
		virtual ~TextureImpl() = default;

		// cannot be copied
		TextureImpl(const TextureImpl&) = delete;
		TextureImpl& operator=(const TextureImpl&) = delete;

		// initialize with data
		virtual bool Initialize(
			unsigned int width, unsigned int height,
			const void* srcData,
			unsigned int bytesPerRow
		) = 0;

		// initialize empty texture
		virtual bool Initialize(
			unsigned int width, unsigned int height
		) = 0;

		// initialize with file
		virtual bool Initialize(const wchar_t* fileNamePath) = 0;

		// drawing methods
		virtual void BeginDraw() = 0;
		virtual void Clear(float red, float green, float blue, float alpha) = 0;
		virtual void EndDraw() = 0;

		virtual void Bind() = 0;
		virtual bool CanBind() = 0;

		virtual const unsigned int GetWidth() const = 0;
		virtual const unsigned int GetHeight() const = 0;

		virtual void Reset() = 0;

		virtual bool SaveToFile(const wchar_t* filename) = 0;
	};
}

