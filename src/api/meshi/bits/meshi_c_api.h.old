#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <meshi/bits/util/handle.hpp>

namespace meshi {
struct Event;

struct EngineBackendInfo {
  const char *application_name = "";
  const char *application_root = "";
};

struct RawEngineBackend;
struct RawPhysicsSystem;
struct RawGraphicsSystem;
} // namespace meshi

extern "C" auto meshi_make_engine(const meshi::EngineBackendInfo &info)
    -> meshi::RawEngineBackend *;
extern "C" auto
meshi_register_event_callback(meshi::RawEngineBackend *engine, void *user_data,
                              void (*callback)(meshi::Event &, void *)) -> void;

extern "C" auto meshi_update(meshi::RawEngineBackend *engine) -> float;

////////////////////////////////////////////
//////////////////GRAPHICS//////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////

namespace meshi::gfx {
struct DirectionalLightInfo {
  glm::vec4 direction;
  glm::vec4 color;
  float intensity;
  float padding;
};
struct RenderableCreateInfo {
  const char *mesh = "";
  const char *material = "";
  glm::mat4 transform = glm::mat4(1.0);
};

struct Renderable;
struct DirectionalLight;
} // namespace meshi::gfx

extern "C" auto meshi_get_graphics_system(meshi::RawEngineBackend *)
    -> meshi::RawGraphicsSystem *;

extern "C" auto
meshi_gfx_create_renderable(meshi::RawGraphicsSystem *,
                            const meshi::gfx::RenderableCreateInfo &)
    -> meshi::Handle<meshi::gfx::Renderable>;

extern "C" auto
meshi_gfx_destroy_renderable(meshi::RawGraphicsSystem *,
                             meshi::Handle<meshi::gfx::Renderable>) -> void;

extern "C" auto
meshi_gfx_set_renderable_transform(meshi::RawGraphicsSystem *,
                                   meshi::Handle<meshi::gfx::Renderable> &,
                                   const glm::mat4 &) -> void;

extern "C" auto
meshi_gfx_create_directional_light(meshi::RawGraphicsSystem *,
                                   const meshi::gfx::DirectionalLightInfo &)
    -> meshi::Handle<meshi::gfx::DirectionalLight>;

extern "C" auto meshi_gfx_destroy_directional_light(
    meshi::RawGraphicsSystem *,
    meshi::Handle<meshi::gfx::DirectionalLight>) -> void;

extern "C" auto meshi_gfx_set_camera(meshi::RawGraphicsSystem *,
                                     const glm::mat4 &) -> void;

extern "C" auto meshi_gfx_set_projection(meshi::RawGraphicsSystem *,
                                         const glm::mat4 &) -> void;

extern "C" auto meshi_gfx_capture_mouse(meshi::RawGraphicsSystem *,
                                         std::int32_t) -> void;


////////////////////////////////////////////
//////////////////PHYSICS///////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////

namespace meshi {
struct PhysicsMaterial;
struct RigidBody;

struct RigidBodyCreateInfo {
  Handle<PhysicsMaterial> material;
  glm::vec3 initial_position;
  glm::vec3 initial_velocity;
  glm::quat initial_rotation;
  std::uint32_t has_gravity = 0;
};

struct ForceApplyInfo {
  glm::vec3 amount;
};

struct PhysicsMaterialCreateInfo {
  float dynamic_friction = 5.0;
};
struct EnvironmentInfo {
  float gravity_mps = -9.8;
};

struct PhysicsActorStatus {
  glm::mat4 transform;
};
} // namespace meshi

extern "C" auto meshi_get_physics_system(meshi::RawEngineBackend *)
    -> meshi::RawPhysicsSystem *;

extern "C" auto meshi_physx_create_material(meshi::RawPhysicsSystem *,
                                            meshi::PhysicsMaterialCreateInfo &)
    -> meshi::Handle<meshi::PhysicsMaterial>;

extern "C" auto
meshi_physx_release_material(meshi::RawPhysicsSystem *,
                             meshi::Handle<meshi::PhysicsMaterial> &) -> void;

extern "C" auto meshi_physx_create_rigid_body(
    meshi::RawPhysicsSystem *, meshi::RawGraphicsSystem *,
    meshi::RigidBodyCreateInfo &) -> meshi::Handle<meshi::RigidBody>;

extern "C" auto
meshi_physx_release_rigid_body(meshi::RawPhysicsSystem *,
                               meshi::Handle<meshi::RigidBody> &) -> void;

extern "C" auto
meshi_physx_apply_force_to_rigid_body(meshi::RawPhysicsSystem *,
                                      meshi::Handle<meshi::RigidBody> &,
                                      meshi::ForceApplyInfo &) -> void;

extern "C" auto meshi_physx_get_rigid_body_status(
    meshi::RawPhysicsSystem *,
    meshi::Handle<meshi::RigidBody> &) -> meshi::PhysicsActorStatus &;
