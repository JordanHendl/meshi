#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"
#include "meshi/world.hpp"
#include <cassert>
#include <meshi/backend.hpp>
#include <meshi/bits/action.hpp>
#include <meshi/bits/components/camera_component.hpp>
#include <meshi/bits/error.hpp>
#include <meshi/bits/event.hpp>
#include <string>
#include <string_view>

namespace meshi {
struct EngineInfo {
  std::string application_name = std::string("MESHI APPLICATION");
  std::string application_root = std::string("");
};
class Engine {
public:
  static auto make(EngineInfo info) -> Result<Engine, Error> {
    const auto backend_info = EngineBackendInfo{
        .application_name = info.application_name.c_str(),
        .application_location = info.application_root.c_str(),
        .headless = 0,
    };

    return make_result<Engine, Error>(Engine(backend_info));
  };

  inline auto world() -> World & { return m_world; }
  inline auto delta_time() -> float {
    return m_dt;
  }

  inline auto update() -> void {
    apply_camera();
    m_dt = m_backend.update();
    m_world.update(m_dt);
  }

  inline auto event() -> EventHandler & { return *m_event; }
  inline auto action() -> ActionHandler & { return *m_action; }
  inline auto backend() -> EngineBackend & { return m_backend; }

private:
  inline auto apply_camera() -> void {
    auto c = detail::world_camera;
    if (c) {
      auto v = c->view_matrix();
      auto p = c->projection();
      
      m_backend.graphics().set_camera(v);
      m_backend.graphics().set_projection(p);
    }
  }
  Engine(const EngineBackendInfo &info)
      : m_backend(info), m_event(std::make_shared<EventHandler>(&m_backend)),
        m_action(std::make_shared<ActionHandler>(*m_event)){};

  EngineBackend m_backend;
  std::shared_ptr<EventHandler> m_event;
  std::shared_ptr<ActionHandler> m_action;
  float m_dt = 0.0f;
  World m_world;
};

namespace detail {
static auto get_raw_engine() -> std::unique_ptr<Engine> * {
  static std::unique_ptr<Engine> ptr = nullptr;
  return &ptr;
}
} // namespace detail

static auto engine() -> Engine * { return detail::get_raw_engine()->get(); }

static auto initialize_meshi_engine(EngineInfo info) -> void {
  *detail::get_raw_engine() =
      std::make_unique<Engine>(Engine::make(info).unwrap());
}

} // namespace meshi
