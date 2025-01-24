#pragma once
#include <cstdint>
#include <glm/glm.hpp>
struct CHandle {
  std::uint16_t id;
  std::uint16_t gen;
};

struct RenderableCreateInfo {
  const char* identifier;
  
};

namespace meshi {
  struct EngineBackend;

  struct EngineBackendInfo {
    const char* application_name = "";
    const char* application_root = "";
  };
}

extern "C" auto meshi_make_engine(const meshi::EngineBackendInfo& info) -> meshi::EngineBackend*;
extern "C" auto meshi_update(meshi::EngineBackend& engine) -> void;
//extern "C" auto meshi_register_renderable(MeshiEngine* engine) -> CHandle;
//extern "C" auto meshi_update_renderable(MeshiEngine* engine) -> void;
