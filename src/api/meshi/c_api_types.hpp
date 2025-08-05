#pragma once
#include <cstdint>

struct Handle { std::uint32_t index; std::uint32_t generation; };
struct Mat4 { float m[4][4]; };
struct Vec3 { float x, y, z; };
struct Vec4 { float x, y, z, w; };
struct Quat { float x, y, z, w; };
struct FFIMeshObjectInfo { const char* mesh; const char* material; Mat4 transform; };
struct DirectionalLightInfo { Vec4 direction; Vec4 color; float intensity; };
struct MaterialInfo { float dynamic_friction_m; };
struct ForceApplyInfo { Vec3 amt; };
struct CollisionShape { Vec3 dimensions; float radius; std::uint32_t shape_type; };
struct RigidBodyInfo { Handle material; Vec3 initial_position; Vec3 initial_velocity; Quat initial_rotation; std::uint32_t has_gravity; CollisionShape collision_shape; };
struct ActorStatus { Vec3 position; Quat rotation; };
struct PhysicsSimulation;
struct RenderEngine;
