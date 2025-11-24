#pragma once
#include <vector>
#include <memory>
#include <cassert>
#include "Event.h"
#include "Timer.h"
#include "Logger.h"
#include "ISprite.h"

namespace graphics::animation
{
	struct Region
	{
		math::geometry::RectF contact;
	};

	template<typename T>
	struct Frame
	{
		T element;
		float duration;
		Region region;
	};

	template<typename T>
	struct Animation
	{
		std::string name;
		std::vector<Frame<T>> frames;
		bool loop = false;
	};

	template<typename T>
	class Animator
	{
	private:
		bool m_running = false;
		float m_elapsedTimeAccumulator = 0.0; 
		Animation<T>* m_animation;
		int m_currFrame = -1;

	public:

		event::Event<int, graphics::animation::Frame<T>> OnFrame;
		event::Event<> OnEnd;
		event::Event<graphics::animation::Animation<T>*> OnPlay;

		Animator():
			m_running(false),
			m_elapsedTimeAccumulator(0.0),
			m_currFrame(-1),
			m_animation(nullptr)
		{
		}

		void Play(graphics::animation::Animation<T>* animation) noexcept
		{
			// validate animation
			m_animation = animation;

			// if animation is valid and we can store it
			m_running = true;

			// reset to first frame
			m_currFrame = 0;

			OnPlay(m_animation);
		}
		 
		void Update(float delta) noexcept
		{
			if (!m_running || !m_animation)
			{
				return;
			}

			m_elapsedTimeAccumulator += delta;

			// get current duration
			assert(m_currFrame < m_animation->frames.size());
			float currFrameDuration = m_animation->frames[m_currFrame].duration;

			size_t numUpdate = 0;
			while (m_elapsedTimeAccumulator >= currFrameDuration)
			{
				m_elapsedTimeAccumulator -= currFrameDuration;

				m_currFrame++;

				if (m_currFrame >= m_animation->frames.size())
				{
					if (m_animation->loop)
					{
						// loop back to start frame
						m_currFrame = 0;
					}
					else
					{
						m_running = false;
						// clamp to max valid range so if it gets queried, it will return last valid frame
						m_currFrame--;

						OnEnd();
					}
				}
				OnFrame(m_currFrame, m_animation->frames[m_currFrame]);
			}
		}

		graphics::animation::Frame<T>* GetCurrentFrame() const noexcept
		{
			return m_animation? &m_animation->frames[m_currFrame] : nullptr;
		}

		const int GetCurrentFrameIndex() const noexcept
		{
			return m_currFrame;
		}

		bool IsFinished()
		{
			return (m_currFrame == m_animation->frames.size() - 1) && !m_animation->loop;
		}

		Animation<T>* GetCurrentAnimation()
		{
			return m_animation;
		}

	};


	namespace Test
	{
		class TestClass
		{
		private:
			timer::StopWatch stopwatch;
			animation::Animator<int> animator;
			animation::Animation<int> animation;

			// listener to game loop's interval (triggered by stopwatch' Lap)
			void OnLoop(float delta)
			{
				animator.Update(delta);
			}

			void OnFrame(int index, graphics::animation::Frame<int> frame)
			{
				//LOG("[" << std::to_string(stopwatch.Peek<timer::milliseconds>()) << "] <" << std::to_string(index) << 
				//	">  index: " << std::to_string(frame.index) << " duration: " << std::to_string(frame.duration));
			}
			void OnEnd()
			{
				LOG("Last frame happened");
			}

		public:
			// on constructor, build the two timers - one is persistent, another is one-shot
			TestClass() 
			{
				animation =
				{
					"test",
					{
						{1, 1000},
						{4, 1500},
						{3, 2000},
						{6, 1000},
						{2, 2500},
						{1, 1000},
						{0, 1500},
					},
					true // loop
				};
				animator.Play(&animation);

				animator.OnFrame += event::Handler(this, &TestClass::OnFrame);

				// listen to stopwatch' Lap(). timers will be updated by this event listener
				stopwatch.OnLap += event::Handler(this, &TestClass::OnLoop);

				// start the stopwatch
				stopwatch.Start();

				// simulate game loop
				while (true)
				{
					// measure elapsed time per loop. this will fire up OnLap event
					stopwatch.Lap<timer::milliseconds>();
				}
			}

		};

		static void Test()
		{
			TestClass test;
		}
	}
}
