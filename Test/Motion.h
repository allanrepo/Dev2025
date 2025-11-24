#pragma once
#include "Transform.h"

namespace spatial
{
	class Motion
	{
	private:
		math::VecF m_velocity;
		float m_speed;

	public:
		void Update(spatial::Transform& transform, float delta) const
		{
			transform.SetPosition(transform.GetPosition() + (m_velocity * delta));
		}

		void MoveTo(const spatial::Transform& transform, const spatial::PosF target, float speed)
		{
			math::VecF direction = target - transform.GetPosition();
			direction = direction.Normalize();
			m_velocity = direction * speed;
		}

		const math::VecF GetVelocity() const
		{
			return m_velocity;
		}

		void SetVelocity(const math::VecF velocity)
		{
			m_velocity = velocity;
		}

		void Stop()
		{
			m_velocity = { 0, 0 };
		}
	};
}
