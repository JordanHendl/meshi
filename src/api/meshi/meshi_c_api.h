#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace meshi {
struct Event;

struct CHandle {
  std::uint16_t id;
  std::uint16_t gen;
};

struct RenderableCreateInfo {
  const char *identifier;
};

struct EngineBackend;

struct EngineBackendInfo {
  const char *application_name = "";
  const char *application_root = "";
};

struct MeshObjectInfo {
  const char *mesh = "";
  const char *material = "";
  glm::mat4 transform = glm::mat4(1.0);
};

template <typename T> struct Handle {
  std::uint16_t id = 0;
  std::uint16_t gen = 0;
};
struct FFIMeshObject;
} // namespace meshi

extern "C" auto meshi_make_engine(const meshi::EngineBackendInfo &info)
    -> meshi::EngineBackend *;
extern "C" auto
meshi_register_event_callback(meshi::EngineBackend *engine, void *user_data,
                              void (*callback)(meshi::Event &, void *)) -> void;

extern "C" auto meshi_update(meshi::EngineBackend *engine) -> float;
extern "C" auto meshi_register_mesh_object(meshi::EngineBackend *engine,
                                           meshi::MeshObjectInfo &info)
    -> meshi::Handle<meshi::FFIMeshObject>;
extern "C" auto
meshi_set_mesh_object_transform(meshi::EngineBackend &engine,
                                meshi::Handle<meshi::FFIMeshObject> &handle,
                                glm::mat4 &transform) -> void;
  
extern "C" auto 
meshi_set_render_camera(meshi::EngineBackend* engine, glm::mat4& transform) -> void;

extern "C" auto 
meshi_set_render_projection(meshi::EngineBackend* engine, glm::mat4& transform) -> void;

//
// extern "C" auto meshi_register_renderable(MeshiEngine* engine) -> CHandle;
// extern "C" auto meshi_update_renderable(MeshiEngine* engine) -> void;
