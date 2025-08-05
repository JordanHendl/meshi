#pragma once

#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <meshi/bits/meshi.h>
#include <meshi/bits/meshi_loader.hpp>
#include <meshi/graphics.hpp>
#include <meshi/physics.hpp>
#include <mutex>
#include <string>
namespace meshi {

class EngineBackend {
public:
  explicit EngineBackend(const MeshiEngineInfo &info) {
    static std::once_flag flag;
    std::call_once(flag, [&]() {
      auto result = detail::load_meshi_api(info.application_location);
      if (result.is_err()) {
        throw std::runtime_error(result.err());
      }
    });

    auto &api = detail::api();
    engine_ = api.meshi_make_engine(&info);

    auto raw_phys = api.meshi_get_physics_system(engine_);
    auto raw_gfx = api.meshi_get_graphics_system(engine_);
    m_phys = PhysicsSystem(raw_phys);
    m_gfx = GraphicsSystem(raw_gfx);
  }

  ~EngineBackend() = default;

  void register_event_callback(void *user_data,
                               MeshiEventCallback callback) {
    detail::api().meshi_register_event_callback(engine_, user_data, callback);
  }

  float update() { return detail::api().meshi_update(engine_); }

  inline auto physics() -> PhysicsSystem & { return m_phys; }

  inline auto graphics() -> GraphicsSystem & { return m_gfx; }

private:
  PhysicsSystem m_phys;
  GraphicsSystem m_gfx;
  MeshiEngine *engine_;
};

} // namespace meshi
