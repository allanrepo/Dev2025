#pragma once
#include <memory>
#include <vector>
#include "ISpriteAtlas.h"

// forward declare
namespace graphics
{
	namespace factory
	{
		class SpriteAtlasFactory;
	}
	namespace resource
	{
		class ISprite;
		class ITexture;
	}
}

namespace graphics::resource
{
	class SpriteAtlas : public graphics::resource::ISpriteAtlas
	{
	private:
		friend class graphics::factory::SpriteAtlasFactory;

		std::shared_ptr<graphics::resource::ITexture> m_texture;
		std::vector<math::geometry::RectF> m_nUVs;

		// private constructor, only SpriteAtlasFactory can create this
		SpriteAtlas(std::unique_ptr<graphics::resource::ITexture> tex);

	public:
		virtual ~SpriteAtlas() = default;

		// ISpriteAtlas methods implementation
		virtual bool Initialize(const wchar_t* fileNamePath) override final;
		virtual void AddUVRect(math::geometry::RectF rect) override final;
		virtual const inline math::geometry::RectF GetUVRect(int index) const override final;

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

