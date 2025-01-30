#pragma once

#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <meshi/bits/meshi_c_api.h>
#include <meshi/graphics.hpp>
#include <meshi/physics.hpp>
#include <string>
namespace meshi {

class EngineBackend {
public:
  explicit EngineBackend(const EngineBackendInfo &info) {
    engine_ = meshi_make_engine(info);

    auto raw_phys = meshi_get_physics_system(engine_);
    auto raw_gfx = meshi_get_graphics_system(engine_);
    m_phys = PhysicsSystem(raw_phys, raw_gfx);
    m_gfx = GraphicsSystem(raw_gfx);
  }

  ~EngineBackend() = default;

  void register_event_callback(void *user_data,
                               void (*callback)(meshi::Event &, void *)) {
    meshi_register_event_callback(engine_, user_data, callback);
  }

  float update() { return meshi_update(engine_); }

  inline auto physics() -> PhysicsSystem & { return m_phys; }

  inline auto graphics() -> GraphicsSystem & { return m_gfx; }

private:
  PhysicsSystem m_phys;
  GraphicsSystem m_gfx;
  RawEngineBackend *engine_;
};

} // namespace meshi
