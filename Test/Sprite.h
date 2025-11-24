#pragma once
#include "ISprite.h"
#include <memory>

// forward declare
namespace graphics
{
	namespace factory
	{
		class SpriteFactory;
	}

	namespace resource
	{
		class ISpriteAtlas;
	}
}

namespace graphics::resource
{
	// is renderable
	// has pointer to sprite atlas 
	// has source rect (normalized rectangular coordinates or UV)
	class Sprite : public ISprite
	{
	private:
		graphics::resource::ISpriteAtlas* m_spriteAtlas = nullptr;
		math::geometry::RectF m_rect;
		math::VecF m_renderOffset;

	public:
		// use this constructor if you have the sprite atlas and the source rect, or you can use factory to get the sprite
		Sprite(graphics::resource::ISpriteAtlas* spriteAtlas, math::geometry::RectF rect, math::VecF renderOffset);
		~Sprite() = default;

	public:
		const virtual math::VecF GetRenderOffset() const;


		// ISizeable methods implementation
		virtual const float GetWidth() const override final;
		virtual const float GetHeight() const override final;
		virtual const spatial::SizeF GetSize() const override final;

		// IRenderable methods implementation
		virtual void Bind() const override final;
		virtual bool CanBind() const override final;
		virtual math::geometry::RectF GetUVRect() const override final;
	};
}
