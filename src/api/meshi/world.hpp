#pragma once
#include <meshi/bits/base.hpp>
#include <meshi/meshi_c_api.h>
#include <meshi/bits/error.hpp>
namespace meshi {
struct WorldInfo {
  EngineBackend* backend = nullptr;
};

struct SpawnInfo {};
class World : private Object {
public:
  World() = default;
  template <typename T> inline auto spawn_object() -> T* {
    m_dirty = true;
    return this->add_component<T>();
  }
  
  inline auto update(float dt) -> void {
    if(m_dirty) {
      cache_world();
      m_dirty = false;
    }

    for(auto* actor : m_actors) {
      actor->update(dt);
    }
  }
private:

  inline auto cache_world() -> void {
    this->filter_components<Actor>(&m_actors);  
  }

  bool m_dirty = true;
  std::vector<Actor*> m_actors;
};
} // namespace meshi
