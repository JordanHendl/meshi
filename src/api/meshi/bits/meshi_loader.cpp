#include "meshi_loader.hpp"

#include <filesystem>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace meshi {
namespace detail {
namespace {

#ifdef _WIN32
using LibraryHandle = HMODULE;
inline LibraryHandle open_library(const std::filesystem::path &p) {
  return LoadLibraryW(p.wstring().c_str());
}
inline void *load_symbol(LibraryHandle lib, const char *name) {
  return reinterpret_cast<void *>(GetProcAddress(lib, name));
}
#else
using LibraryHandle = void *;
inline LibraryHandle open_library(const std::filesystem::path &p) {
  return dlopen(p.c_str(), RTLD_LAZY | RTLD_LOCAL);
}
inline void *load_symbol(LibraryHandle lib, const char *name) {
  return dlsym(lib, name);
}
#endif

LibraryHandle g_lib{};
MeshiApi g_api{};

template <typename T>
bool load_func(const char *name, T &out) {
  auto sym = load_symbol(g_lib, name);
  if (!sym)
    return false;
  out = reinterpret_cast<T>(sym);
  return true;
}

} // namespace

static auto library_name() -> const char * {
#ifdef _WIN32
  return "meshi.dll";
#elif __APPLE__
  return "libmeshi.dylib";
#else
  return "libmeshi.so";
#endif
}

auto load_meshi_api(const std::filesystem::path &dir)
    -> meshi::Result<int, std::string> {
  auto path = dir / library_name();
  g_lib = open_library(path);
  if (!g_lib) {
    return meshi::make_error<std::string>(
        std::string("Failed to open library: ") + path.string());
  }

#define LOAD_SYM(name)                                                          \
  if (!load_func(#name, g_api.name))                                            \
    return meshi::make_error<std::string>("Missing symbol: " #name)

  // Engine
  LOAD_SYM(meshi_make_engine);
  LOAD_SYM(meshi_make_engine_headless);
  LOAD_SYM(meshi_destroy_engine);
  LOAD_SYM(meshi_register_event_callback);
  LOAD_SYM(meshi_update);
  LOAD_SYM(meshi_get_graphics_system);

  // Graphics
  LOAD_SYM(meshi_gfx_create_renderable);
  LOAD_SYM(meshi_gfx_create_cube);
  LOAD_SYM(meshi_gfx_create_sphere);
  LOAD_SYM(meshi_gfx_create_triangle);
  LOAD_SYM(meshi_gfx_set_renderable_transform);
  LOAD_SYM(meshi_gfx_create_directional_light);
  LOAD_SYM(meshi_gfx_set_directional_light_transform);
  LOAD_SYM(meshi_gfx_set_directional_light_info);
  LOAD_SYM(meshi_gfx_set_camera);
  LOAD_SYM(meshi_gfx_set_projection);
  LOAD_SYM(meshi_gfx_capture_mouse);

  // Physics
  LOAD_SYM(meshi_get_physics_system);
  LOAD_SYM(meshi_physx_set_gravity);
  LOAD_SYM(meshi_physx_create_material);
  LOAD_SYM(meshi_physx_release_material);
  LOAD_SYM(meshi_physx_create_rigid_body);
  LOAD_SYM(meshi_physx_release_rigid_body);
  LOAD_SYM(meshi_physx_apply_force_to_rigid_body);
  LOAD_SYM(meshi_physx_set_rigid_body_transform);
  LOAD_SYM(meshi_physx_get_rigid_body_status);
  LOAD_SYM(meshi_physx_set_collision_shape);
  LOAD_SYM(meshi_physx_get_contacts);
  LOAD_SYM(meshi_physx_collision_shape_sphere);
  LOAD_SYM(meshi_physx_collision_shape_box);

#undef LOAD_SYM

  return meshi::make_result<int, std::string>(0);
}

MeshiApi &api() { return g_api; }

} // namespace detail
} // namespace meshi

