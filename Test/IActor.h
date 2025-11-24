#pragma once
#include "IRenderable.h"
#include "ISizeable.h"
#include "State.h"
#include "Animation.h"
#include "Motion.h"
#include "Transform.h"
#include "Sprite.h"
#include "StateMachine.h"

namespace spatial
{

	namespace collision
	{
		struct Frame;
		struct Sequence;
		class Collision;
		class CollisionFactory;
	}
}

namespace component
{
	class IActor : public graphics::renderable::IRenderable, public spatial::ISizeable<float>
	{
	public:
		virtual ~IActor() = default;

		// state management
		virtual void Update(float delta) = 0;
		virtual void SetState(std::unique_ptr<state::State<component::IActor>> state) = 0;
		virtual void QueueState(std::unique_ptr<state::State<component::IActor>> state) = 0;
		virtual state::StateMachine<component::IActor>& GetStateMachine() = 0;

		// animation methods
		virtual void AddAnimation(const std::string& name, std::unique_ptr<graphics::animation::Animation<graphics::resource::Sprite>> animation) = 0;
		virtual void SetAnimation(const std::string& key) = 0;
		virtual graphics::animation::Animator<graphics::resource::Sprite>& GetAnimator() = 0;
		virtual const graphics::animation::Animator<graphics::resource::Sprite>& GetAnimator() const = 0;

		// motion methods
		virtual spatial::Motion& GetMotion() = 0;

		// transform methods
		virtual spatial::Transform& GetTransform() = 0;
		virtual const spatial::Transform& GetTransform() const = 0;
		
		// face property methods
		virtual math::VecF GetFaceDirection() const = 0;
		virtual void SetFaceTo(spatial::PosF pos) = 0;

		// collision methods
		virtual bool Contains(spatial::PosF position) = 0;
		virtual const math::geometry::RectF GetCollisionBox(bool absolute = true) const = 0;

		// targetable behavior
		event::Event<IActor*, bool> OnTargetableChanged;
		virtual void SetTargetable(bool enable) = 0;
	};
}

//namespace engine
//{
//	class IActor: public graphics::renderable::IRenderable, public spatial::ISizeable
//	{
//	public:
//		~IActor() = default;
//
//		virtual void Update(float delta) = 0;
//
//	};
//
//
//}
