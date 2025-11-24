#pragma once

namespace spatial
{
	class Transform
	{
	private:
		spatial::PosF m_position;
		spatial::SizeF m_scale;
		float m_rotation = 0; // in radians
		math::VecF m_translate;

	public:
		const spatial::PosF GetPosition() const
		{
			return m_position;
		}

		void SetPosition(const spatial::PosF position)
		{
			m_position = position;
		}
	};
}
