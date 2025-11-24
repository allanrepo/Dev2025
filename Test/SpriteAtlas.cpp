#include "SpriteAtlas.h"
#include "Logger.h"
#include "Sprite.h"
#include "ITexture.h"


graphics::resource::SpriteAtlas::SpriteAtlas(std::unique_ptr<graphics::resource::ITexture> tex)
	:m_texture(std::move(tex))
{
}

bool graphics::resource::SpriteAtlas::Initialize(const wchar_t* fileNamePath)
{
	if (!m_texture->Initialize(fileNamePath))
	{
		LOGERROR("Failed to initialize SpriteSheet' texture resource.");
		return false;
	}

	return true;
}

void graphics::resource::SpriteAtlas::AddUVRect(math::geometry::RectF rect)
{
	m_nUVs.push_back(rect);
}

const math::geometry::RectF graphics::resource::SpriteAtlas::GetUVRect(int index) const
{
	return m_nUVs[index];
}

math::geometry::RectF graphics::resource::SpriteAtlas::GetUVRect() const
{
	return math::geometry::RectF{ 0, 0, GetWidth(), GetHeight() };
}

void graphics::resource::SpriteAtlas::Bind() const
{
	return m_texture->Bind();
}

bool graphics::resource::SpriteAtlas::CanBind() const
{
	return m_texture->CanBind();
}

const float graphics::resource::SpriteAtlas::GetWidth() const
{
	return static_cast<float>(m_texture->GetWidth());
}

const float graphics::resource::SpriteAtlas::GetHeight() const
{
	return static_cast<float>(m_texture->GetHeight());
}

const spatial::SizeF graphics::resource::SpriteAtlas::GetSize() const
{
	return spatial::SizeF{
		static_cast<float>(m_texture->GetWidth()),
		static_cast<float>(m_texture->GetHeight())
	};
}

