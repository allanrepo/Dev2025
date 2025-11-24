#pragma once
#include "Rect.h"
#include "Size.h"

namespace text
{
	struct Glyph
	{
		math::geometry::RectF normUV;
		spatial::SizeF size;
	};
}