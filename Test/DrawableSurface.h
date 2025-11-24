#pragma once
#include "IDrawableSurface.h"

// forward declare
namespace graphics
{
	namespace resource
	{
		class ITexture;
	}
}

namespace graphics::resource
{
	class DrawableSurface: public graphics::resource::IDrawableSurface
	{
	private:
		std::shared_ptr<graphics::resource::ITexture> texture;

	public:
		DrawableSurface(std::unique_ptr<graphics::resource::ITexture> tex);
		virtual ~DrawableSurface() = default;

		// cannot be copied
		DrawableSurface(const DrawableSurface&) = delete;
		DrawableSurface& operator=(const DrawableSurface&) = delete;

		// initialize methods
		virtual bool Initialize(
			unsigned int width, unsigned int height
		) override final;

		// drawing methods
		virtual void Begin() override final;
		virtual void Clear(float red, float green, float blue, float alpha) override final;
		virtual void End() override final;

		virtual void Reset() override final;

		// ISizeable methods implementation
		virtual const float GetWidth() const override final;
		virtual const float GetHeight() const override final;
		virtual const spatial::SizeF GetSize() const override final;

		// IRenderable methods implementation
		virtual math::geometry::RectF GetUVRect() const override final;
		virtual void Bind() const override final;
		virtual bool CanBind() const override final;
	};
}


