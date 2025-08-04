#pragma once

#include <meshi/meshi_types.h>
#include <glm/glm.hpp>

namespace meshi {

using EngineBackendInfo = MeshiEngineInfo;
using RawEngineBackend = MeshiEngine;
using RawGraphicsSystem = MeshiRenderEngine;
using RawPhysicsSystem = MeshiPhysicsSimulation;

template <typename T> using Handle = MeshiHandle;

namespace gfx {
using Renderable = MeshiMeshObject;
struct RenderableCreateInfo {
  const char *mesh = "";
  const char *material = "";
  glm::mat4 transform = glm::mat4(1.0f);
};
using DirectionalLight = MeshiDirectionalLight;
struct DirectionalLightInfo {
  glm::vec4 direction{0.0f};
  glm::vec4 color{1.0f};
  float intensity = 1.0f;
};
} // namespace gfx

using PhysicsMaterial = MeshiMaterial;
using PhysicsMaterialCreateInfo = MeshiMaterialInfo;
using RigidBody = MeshiRigidBody;
struct RigidBodyCreateInfo {
  Handle<PhysicsMaterial> material{};
  glm::vec3 initial_position{0.0f};
  glm::vec3 initial_velocity{0.0f};
  glm::quat initial_rotation{1.0f, 0.0f, 0.0f, 0.0f};
  std::uint32_t has_gravity{0};
};
using ForceApplyInfo = MeshiForceApplyInfo;
struct PhysicsActorStatus {
  glm::vec3 position{0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
};

} // namespace meshi

