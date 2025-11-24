#include "Logger.h"
#include "ITexture.h"
#include "DrawableSurface.h"

graphics::resource::DrawableSurface::DrawableSurface(std::unique_ptr<graphics::resource::ITexture> tex)
	:texture(std::move(tex))
{
}

// initialize methods
bool graphics::resource::DrawableSurface::Initialize(unsigned int width, unsigned int height) 
{
	if (!texture->Initialize(width, height))
	{
		LOGERROR("Failed to initialize DrawableSurface' texture resource.");
		return false;
	}

	return true;
}

math::geometry::RectF graphics::resource::DrawableSurface::GetUVRect() const
{
	return math::geometry::RectF{ 0, 0, 1, 1 };
}

void graphics::resource::DrawableSurface::Bind() const
{
	texture->Bind();
}

bool graphics::resource::DrawableSurface::CanBind() const
{
	return texture->CanBind();
}

// drawing methods
void graphics::resource::DrawableSurface::Begin()
{
	texture->BeginDraw();
}

void graphics::resource::DrawableSurface::Clear(float red, float green, float blue, float alpha)
{
	texture->Clear(red, green, blue, alpha);
}

void graphics::resource::DrawableSurface::End()
{
	texture->EndDraw();
}

const float graphics::resource::DrawableSurface::GetWidth() const
{
	return static_cast<float>(texture->GetWidth());
}

const float graphics::resource::DrawableSurface::GetHeight() const
{
	return static_cast<float>(texture->GetHeight());
}

const spatial::SizeF graphics::resource::DrawableSurface::GetSize() const
{
	return spatial::SizeF
	{
		static_cast<float>(texture->GetWidth()),
		static_cast<float>(texture->GetHeight())
	};
}

void graphics::resource::DrawableSurface::Reset()
{
	texture->Reset();
}
