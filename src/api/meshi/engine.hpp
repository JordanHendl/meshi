#pragma once
#include "meshi/world.hpp"
#include <cassert>
#include <meshi/bits/action.hpp>
#include <meshi/bits/camera.hpp>
#include <meshi/bits/error.hpp>
#include <meshi/bits/event.hpp>
#include <meshi/meshi_c_api.h>
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
        .application_root = info.application_root.c_str(),
    };

    auto backend = meshi_make_engine(backend_info);
    return make_result<Engine, Error>(Engine(backend));
  };

  inline auto world() -> World & { return m_world; }

  inline auto update() -> void {
    apply_camera();
    auto dt = meshi_update(m_backend);
    m_world.update(dt);
  }

  inline auto event() -> EventHandler & { return *m_event; }
  inline auto action() -> ActionHandler & { return *m_action; }
  inline auto backend() -> EngineBackend * {
    assert(m_backend != nullptr);
    return m_backend;
  }

private:
  inline auto apply_camera() -> void {
    auto c = detail::world_camera;
    if (c) {
      auto v = c->view_matrix();
      auto p = c->projection();
      meshi_set_render_camera(backend(), v);
      meshi_set_render_projection(backend(), p);
    }
  }
  Engine(EngineBackend *e)
      : m_backend(e), m_event(std::make_shared<EventHandler>(e)),
        m_action(std::make_shared<ActionHandler>(*m_event)){};

  World m_world;
  std::shared_ptr<EventHandler> m_event;
  std::shared_ptr<ActionHandler> m_action;
  EngineBackend *m_backend = nullptr;
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
