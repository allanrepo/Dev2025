#pragma once
#include "IRenderable.h"
#include "ISizeable.h"

namespace graphics::resource
{
	class IImageSurface : public graphics::renderable::IRenderable, public spatial::ISizeable<float>
	{
	protected:
		// cannot be deleted
		virtual ~IImageSurface() = default;

	public:
		virtual bool Initialize(const wchar_t* fileNamePath) = 0;
	};
}


