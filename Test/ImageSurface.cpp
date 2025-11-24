#include "ImageSurface.h"
#include "Logger.h"
#include "ContextCache.h"
#include "ITexture.h"

graphics::resource::ImageSurface::ImageSurface(std::unique_ptr<graphics::resource::ITexture> tex)
	:texture(std::move(tex))
{
}

// initialize methods
bool graphics::resource::ImageSurface::Initialize(const wchar_t* fileNamePath)
{
	if (!texture->Initialize(fileNamePath))
	{
		LOGERROR("Failed to initialize ImageSurface' texture resource.");
		return false;
	}

	return true;
}

math::geometry::RectF graphics::resource::ImageSurface::GetUVRect() const
{
	return math::geometry::RectF{ 0, 0, 1, 1 };
}

void graphics::resource::ImageSurface::Bind() const
{
	texture->Bind();
}

bool graphics::resource::ImageSurface::CanBind() const
{
	return texture->CanBind();
}

const float graphics::resource::ImageSurface::GetWidth() const
{
	return static_cast<float>(texture->GetWidth());
}

const float graphics::resource::ImageSurface::GetHeight() const
{
	return static_cast<float>(texture->GetHeight());
}

const spatial::SizeF graphics::resource::ImageSurface::GetSize() const
{
	return spatial::SizeF
	{
		 static_cast<float>(texture->GetWidth()),
		  static_cast<float>(texture->GetHeight())
	};
}

void graphics::resource::ImageSurface::Reset()
{
	texture->Reset();
}
