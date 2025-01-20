#pragma once
#include <meshi/meshi_c_api.h>
#include <meshi/bits/error.hpp>
namespace meshi {
struct WorldInfo {
  EngineBackend* backend = nullptr;
};

struct SpawnInfo {};
class World {
public:
  static auto make(WorldInfo info) -> Result<World, Error> {
    return make_result<World, Error>(World{});
  };

  template <typename T> inline auto spawn_entity(SpawnInfo info) -> void {}

  template <typename T> inline auto spawn_denizen(SpawnInfo info) -> void {}

  template <typename T> inline auto get_denizen() -> Result<T *, Error> {}

  template <typename T> inline auto get_entity() -> Result<T *, Error> {}

private:
  EngineBackend* m_backend = nullptr;
};
} // namespace meshi
