#pragma once
#include "meshi/bits/components/renderable_component.hpp"
#include "meshi/engine.hpp"
#include <meshi/bits/util/slice.hpp>
namespace meshi {
class PhysicsComponent : public ActorComponent {
public:
  using CreateInfo = RigidBodyCreateInfo;

  PhysicsComponent() {
    auto info = CreateInfo {
      .material = {},
      .has_gravity = false,
    };

    m_handle = engine()->backend().physics().create_rigid_body(info);
  }

  PhysicsComponent(CreateInfo info) {
    m_handle = engine()->backend().physics().create_rigid_body(info);
  }

  //////////////////////////////////////////////////////
  ////VIRTUAL FUNCITONS/////////////////////////////////
  //////////////////////////////////////////////////////

  virtual ~PhysicsComponent() = default;

  virtual auto update(float dt) -> void {
    ActorComponent::update(dt);

    auto root = this->get_root_component();
    if (root) {
      auto c = root->as_type<ActorComponent>();
      if (c) {
        c->set_transform(engine()
                             ->backend()
                             .physics()
                             .get_rigid_body_status(m_handle)
                             .transform);
      }
    }
  }

  //////////////////////////////////////////////////////
  ////NON-VIRTUAL FUNCTIONS/////////////////////////////
  //////////////////////////////////////////////////////

protected:
  Handle<RigidBody> m_handle;
};
} // namespace meshi
