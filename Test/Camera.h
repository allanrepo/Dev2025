#pragma once
#include "Pos.h"
#include <algorithm>

namespace spatial
{
	class Camera
	{
	private:
		spatial::PosF m_position;
		math::geometry::RectF m_viewport;
		spatial::SizeF m_worldSize;

		void ClampToBounds()
		{
			// camera position is in world space, so we need to clamp it to the map size minus the viewport size
			// also make sure we don't clamp to negative values if the map is smaller than the viewport
			// world space (0,0) is at top-left corner
			m_position.x = std::clamp(m_position.x, 0.0f, std::max<float>(0.0f, m_worldSize.width - m_viewport.GetWidth()));
			m_position.y = std::clamp(m_position.y, 0.0f, std::max<float>(0.0f, m_worldSize.height - m_viewport.GetHeight()));
		}

	public:
		Camera(math::geometry::RectF viewport) :
			m_viewport(viewport),
			m_position({ 0, 0 }),
			m_worldSize({ 0, 0 })
		{
		}

		void SetWorldSize(float width, float height)
		{
			m_worldSize = { width, height };
			ClampToBounds();
		}

		void SetViewport(math::geometry::RectF viewport)
		{
			m_viewport = viewport;
			ClampToBounds();
		}

		void MoveBy(math::VecF delta)
		{
			// why it's negative? because we are moving the camera (in world space), not the world
			m_position -= delta;
			ClampToBounds();
		}

		void SetPosition(spatial::PosF position)
		{
			m_position = position;
			ClampToBounds();
		}

		// set camera position so that the specified world position is at the center of the viewport
		void CenterOn(spatial::PosF worldPos)
		{
			m_position = worldPos - math::VecF(m_viewport.GetWidth() / 2, m_viewport.GetHeight() / 2);
			ClampToBounds();
		}

		spatial::PosF GetPosition() const
		{
			return m_position;
		}

		math::geometry::RectF GetViewport() const
		{
			return m_viewport;
		}

		// Converts a world position to screen-space
		spatial::PosF WorldToScreen(spatial::PosF worldPos) const
		{
			// translate the world position by viewport's top left position so the world position is now in viewport space
			// then offset it by camera's position to scroll the world to the correct position in the viewport
			return worldPos + spatial::PosF{ m_viewport.left, m_viewport.top } - m_position;
		}

		spatial::PosF ScreenToWorld(spatial::PosF screenPos) const
		{
			// translate the screen position by viewport's top left position so the screen position is now in viewport space
			// then offset it by camera's position to scroll the world to the correct position in the viewport
			return screenPos - spatial::PosF{ m_viewport.left, m_viewport.top } + m_position;
		}
	};

}
