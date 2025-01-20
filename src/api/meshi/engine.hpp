#pragma once
#include <meshi/bits/error.hpp>
#include <meshi/meshi_c_api.h>
#include <meshi/world.hpp>
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
    const auto world = MESHI_CHECK_ERROR(World::make({}));
    const auto backend_info = EngineBackendInfo{
        .application_name = info.application_name.c_str(),
        .application_root = info.application_root.c_str(),
    };

    return make_result<Engine, Error>(Engine(std::move(world), meshi_make_engine(backend_info)));
  };

  inline auto world() -> World & { return m_world; }

private:
  Engine(World w, EngineBackend *e) : m_world(w), m_backend(e){};

  World m_world;
  EngineBackend *m_backend = nullptr;
};
} // namespace meshi
