#pragma once
#include "Pos.h"
#include "Color.h"

// forward declarations
namespace graphics
{
	namespace renderer
	{
		class IRenderer;
	}
}

namespace graphics
{
	namespace primitives
	{
		void DrawLineSegment(
			graphics::renderer::IRenderer& renderer,
			const spatial::PosF& start,
			const spatial::PosF& end,
			const ColorF& color = { 1, 1, 1, 1 },
			float thickness = 1.0f
		);

		void DrawCircleOutline(
			graphics::renderer::IRenderer& renderer,
			const spatial::PosF& center,
			float radius,
			const graphics::ColorF& color = { 1, 1, 1, 1 },
			float thickness = 1.0f,
			int segments = 32
		);
	}	
}

