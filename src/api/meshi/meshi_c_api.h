#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace meshi {
// Enum representing the type of the event.
enum class EventType {
  QUIT,
  KEYDOWN,
  KEYUP,
  MOUSEBUTTONDOWN,
  MOUSEBUTTONUP,
  MOUSEMOTION,
  MOUSEWHEEL,
  UNKNOWN
};

// Struct representing a key event.
struct KeyEvent {
  int32_t scancode;
  int32_t keycode;
  int32_t state; // 0 = released, 1 = pressed
  int32_t repeat;
};

// Struct representing a mouse button event.
struct MouseButtonEvent {
  int32_t button;
  int32_t state; // 0 = released, 1 = pressed
  int32_t clicks;
  int32_t x;
  int32_t y;
};

// Struct representing mouse motion.
struct MouseMotionEvent {
  int32_t x;
  int32_t y;
  int32_t xrel;
  int32_t yrel;
};

// Struct representing a mouse wheel event.
struct MouseWheelEvent {
  int32_t x;
  int32_t y;
};

// Union containing the event data.
union EventData {
  KeyEvent key;
  MouseButtonEvent mouse_button;
  MouseMotionEvent mouse_motion;
  MouseWheelEvent mouse_wheel;
};

// Struct representing an SDL-style event.
struct Event {
  EventType event_type;
  uint32_t timestamp;
  EventData data;
};

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
// extern "C" auto meshi_register_renderable(MeshiEngine* engine) -> CHandle;
// extern "C" auto meshi_update_renderable(MeshiEngine* engine) -> void;
