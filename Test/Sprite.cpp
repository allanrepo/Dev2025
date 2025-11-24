#include "Sprite.h"
#include "ISpriteAtlas.h"

graphics::resource::Sprite::Sprite(graphics::resource::ISpriteAtlas* spriteAtlas, math::geometry::RectF rect, math::VecF renderOffset) :
	m_rect(rect),
	m_spriteAtlas(spriteAtlas)
{
	// the render offset we received is normalized, because the caller that is creating sprite does not know yet about the sprite atlas to be used
	// so we convert to absolute value here
	renderOffset.x *= spriteAtlas->GetWidth() * rect.GetWidth();
	renderOffset.y *= spriteAtlas->GetHeight() * rect.GetHeight();
	m_renderOffset = renderOffset;
}

const math::VecF graphics::resource::Sprite::GetRenderOffset() const
{
	return m_renderOffset;
}

void graphics::resource::Sprite::Bind() const
{
	m_spriteAtlas->Bind();
}

bool graphics::resource::Sprite::CanBind() const
{
	return m_spriteAtlas->CanBind();
}

math::geometry::RectF graphics::resource::Sprite::GetUVRect() const
{
	return m_rect;
}

const float graphics::resource::Sprite::GetWidth() const
{
	return m_spriteAtlas->GetWidth()*(m_rect.right - m_rect.left);
}

const float graphics::resource::Sprite::GetHeight() const
{
	return m_spriteAtlas->GetHeight()*(m_rect.bottom - m_rect.top);
}

const spatial::SizeF graphics::resource::Sprite::GetSize() const
{
	return spatial::SizeF{
		m_spriteAtlas->GetWidth()* (m_rect.right - m_rect.left),
		m_spriteAtlas->GetHeight()* (m_rect.bottom - m_rect.top)
	};
}


