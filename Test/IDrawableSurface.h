#pragma once
#include "IRenderable.h"
#include "ISizeable.h"

namespace graphics::resource
{
	class IDrawableSurface: public graphics::renderable::IRenderable, public spatial::ISizeable<float>
	{
	protected:
		// cannot be deleted
		virtual ~IDrawableSurface() = default;

	public:
		virtual bool Initialize(
			unsigned int width, unsigned int height
		) = 0;

		virtual void Reset() = 0;

		// drawing methods
		virtual void Begin() = 0;
		virtual void Clear(float red, float green, float blue, float alpha) = 0;
		virtual void End() = 0;
	};
}


