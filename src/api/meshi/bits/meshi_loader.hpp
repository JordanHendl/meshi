#pragma once

#include <filesystem>
#include <string>

#include <meshi/bits/util/result.hpp>
#include <meshi/bits/meshi.h>

namespace meshi {
namespace detail {

struct MeshiApi {
    // Engine
    decltype(&meshi_make_engine) meshi_make_engine{};
    decltype(&meshi_make_engine_headless) meshi_make_engine_headless{};
    decltype(&meshi_destroy_engine) meshi_destroy_engine{};
    decltype(&meshi_register_event_callback) meshi_register_event_callback{};
    decltype(&meshi_update) meshi_update{};
    decltype(&meshi_get_graphics_system) meshi_get_graphics_system{};

    // Graphics
    decltype(&meshi_gfx_create_renderable) meshi_gfx_create_renderable{};
    decltype(&meshi_gfx_create_cube) meshi_gfx_create_cube{};
    decltype(&meshi_gfx_create_sphere) meshi_gfx_create_sphere{};
    decltype(&meshi_gfx_create_triangle) meshi_gfx_create_triangle{};
    decltype(&meshi_gfx_set_renderable_transform) meshi_gfx_set_renderable_transform{};
    decltype(&meshi_gfx_create_directional_light) meshi_gfx_create_directional_light{};
    decltype(&meshi_gfx_set_directional_light_transform) meshi_gfx_set_directional_light_transform{};
    decltype(&meshi_gfx_set_directional_light_info) meshi_gfx_set_directional_light_info{};
    decltype(&meshi_gfx_set_camera) meshi_gfx_set_camera{};
    decltype(&meshi_gfx_set_projection) meshi_gfx_set_projection{};
    decltype(&meshi_gfx_capture_mouse) meshi_gfx_capture_mouse{};

    // Physics
    decltype(&meshi_get_physics_system) meshi_get_physics_system{};
    decltype(&meshi_physx_set_gravity) meshi_physx_set_gravity{};
    decltype(&meshi_physx_create_material) meshi_physx_create_material{};
    decltype(&meshi_physx_release_material) meshi_physx_release_material{};
    decltype(&meshi_physx_create_rigid_body) meshi_physx_create_rigid_body{};
    decltype(&meshi_physx_release_rigid_body) meshi_physx_release_rigid_body{};
    decltype(&meshi_physx_apply_force_to_rigid_body) meshi_physx_apply_force_to_rigid_body{};
    decltype(&meshi_physx_set_rigid_body_transform) meshi_physx_set_rigid_body_transform{};
    decltype(&meshi_physx_get_rigid_body_status) meshi_physx_get_rigid_body_status{};
    decltype(&meshi_physx_set_collision_shape) meshi_physx_set_collision_shape{};
    decltype(&meshi_physx_get_contacts) meshi_physx_get_contacts{};
    decltype(&meshi_physx_collision_shape_sphere) meshi_physx_collision_shape_sphere{};
    decltype(&meshi_physx_collision_shape_box) meshi_physx_collision_shape_box{};
};

auto load_meshi_api(const std::filesystem::path& lib_path)
    -> meshi::Result<int, std::string>;

MeshiApi& api();

} // namespace detail
} // namespace meshi

