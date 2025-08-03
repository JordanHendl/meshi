#pragma once

#include <stddef.h>
#include <stdint.h>
#include "meshi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct MeshiEngine;
struct MeshiEngineInfo;
struct RenderEngine;
struct FFIMeshObjectInfo;
struct MeshObject;
struct DirectionalLightInfo;
struct DirectionalLight;
struct Mat4;
struct Vec3;
struct PhysicsSimulation;
struct MaterialInfo;
struct RigidBodyInfo;
struct ForceApplyInfo;
struct ActorStatus;
struct CollisionShape;
struct ContactInfo;
struct Event;

typedef void (*MeshiEventCallback)(struct Event*, void*);

// Engine
struct MeshiEngine* meshi_make_engine(const struct MeshiEngineInfo* info);
struct MeshiEngine* meshi_make_engine_headless(const char* application_name, const char* application_location);
void meshi_destroy_engine(struct MeshiEngine* engine);
void meshi_register_event_callback(struct MeshiEngine* engine, void* user_data, MeshiEventCallback cb);
float meshi_update(struct MeshiEngine* engine);
struct RenderEngine* meshi_get_graphics_system(struct MeshiEngine* engine);

// Graphics
struct Handle meshi_gfx_create_renderable(struct RenderEngine* render, const struct FFIMeshObjectInfo* info);
struct Handle meshi_gfx_create_cube(struct RenderEngine* render);
struct Handle meshi_gfx_create_sphere(struct RenderEngine* render);
struct Handle meshi_gfx_create_triangle(struct RenderEngine* render);
void meshi_gfx_set_renderable_transform(struct RenderEngine* render, struct Handle h, const struct Mat4* transform);
struct Handle meshi_gfx_create_directional_light(struct RenderEngine* render, const struct DirectionalLightInfo* info);
void meshi_gfx_set_directional_light_transform(struct RenderEngine* render, struct Handle h, const struct Mat4* transform);
void meshi_gfx_set_directional_light_info(struct RenderEngine* render, struct Handle h, const struct DirectionalLightInfo* info);
void meshi_gfx_set_camera(struct RenderEngine* render, const struct Mat4* transform);
void meshi_gfx_set_projection(struct RenderEngine* render, const struct Mat4* transform);
void meshi_gfx_capture_mouse(struct RenderEngine* render, int32_t value);

// Physics
struct PhysicsSimulation* meshi_get_physics_system(struct MeshiEngine* engine);
void meshi_physx_set_gravity(struct PhysicsSimulation* physics, float gravity_mps);
struct Handle meshi_physx_create_material(struct PhysicsSimulation* physics, const struct MaterialInfo* info);
void meshi_physx_release_material(struct PhysicsSimulation* physics, const struct Handle* h);
struct Handle meshi_physx_create_rigid_body(struct PhysicsSimulation* physics, const struct RigidBodyInfo* info);
void meshi_physx_release_rigid_body(struct PhysicsSimulation* physics, const struct Handle* h);
void meshi_physx_apply_force_to_rigid_body(struct PhysicsSimulation* physics, const struct Handle* h, const struct ForceApplyInfo* info);
int32_t meshi_physx_set_rigid_body_transform(struct PhysicsSimulation* physics, const struct Handle* h, const struct ActorStatus* info);
int32_t meshi_physx_get_rigid_body_status(struct PhysicsSimulation* physics, const struct Handle* h, struct ActorStatus* out_status);
int32_t meshi_physx_set_collision_shape(struct PhysicsSimulation* physics, const struct Handle* h, const struct CollisionShape* shape);
size_t meshi_physx_get_contacts(struct PhysicsSimulation* physics, struct ContactInfo* out_contacts, size_t max);
struct CollisionShape meshi_physx_collision_shape_sphere(float radius);
struct CollisionShape meshi_physx_collision_shape_box(struct Vec3 dimensions);

#ifdef __cplusplus
} // extern "C"
#endif

