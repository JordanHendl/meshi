#pragma once
#include "meshi/bits/components/actor_component.hpp"
#include "meshi/bits/components/physics_component.hpp"
#include "meshi/engine.hpp"
#include <meshi/bits/util/slice.hpp>
namespace meshi {
class RenderableComponent : public PhysicsComponent {
public:
  RenderableComponent(RigidBodyCreateInfo info) : PhysicsComponent(info) {}
  virtual ~RenderableComponent() = default;
};
} // namespace meshi
