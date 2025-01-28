#pragma once
#include <meshi/bits/base.hpp>
#include <meshi/bits/error.hpp>
#include <meshi/meshi_c_api.h>
namespace meshi {
struct WorldInfo {
  EngineBackend *backend = nullptr;
};

struct SpawnInfo {};
class World : private Object {
public:
  World() = default;
  template <typename T> inline auto spawn_object() -> T * {
    m_dirty = true;
    return this->add_subobject<T>();
  }

  inline auto update(float dt) -> void {
    if (m_dirty) {
      cache_world();
      m_dirty = false;
    }

    for (auto *actor : m_actors) {
      if (actor->active()) {
        actor->update(dt);
      }


    }
  }

private:
  inline auto cache_world() -> void {
    this->filter_subobjects<Actor>(&m_actors);
  }

  bool m_dirty = true;
  std::vector<Actor *> m_actors;
  std::vector<Actor *> m_active_actors;
};
} // namespace meshi
