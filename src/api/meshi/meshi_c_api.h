#include <cstdint>
#include <glm/glm.hpp>
struct CHandle {
  std::uint16_t id;
  std::uint16_t gen;
};

struct MeshiEngine;
struct Renderable;


struct RenderableCreateInfo {
  const char* identifier;
  
};

namespace meshi {
  struct EngineInfo {
    const char* application_name = "";
    const char* application_location = "";
  };
}

extern "C" auto meshi_make_engine(meshi::EngineInfo& info) -> MeshiEngine*;
//extern "C" auto meshi_register_renderable(MeshiEngine* engine) -> CHandle;
//extern "C" auto meshi_update_renderable(MeshiEngine* engine) -> void;
