#pragma once
#include "IRenderable.h"
#include "ISizeable.h"
#include <memory>

namespace graphics::resource
{
	class ISprite : public graphics::renderable::IRenderable, public spatial::ISizeable<float>
	{
	public:
		ISprite() = default;
		virtual ~ISprite() = default;

		// ISprite methods
		const virtual math::VecF GetRenderOffset() const = 0;

		// ISizeable methods
		virtual const float GetWidth() const = 0;
		virtual const float GetHeight() const = 0;
		virtual const spatial::SizeF GetSize() const = 0;

		// irenderable methods
		virtual void Bind() const = 0;
		virtual bool CanBind() const = 0;
		virtual math::geometry::RectF GetUVRect() const = 0;
	};

}
