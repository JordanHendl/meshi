#pragma once
#include "meshi/bits/components/renderable_component.hpp"
#include "meshi/engine.hpp"
#include <meshi/bits/util/slice.hpp>
namespace meshi {
struct MeshComponentCreateInfo {
  gfx::RenderableCreateInfo render_info;
  RigidBodyCreateInfo rigid_body_info;
};
class MeshComponent : public RenderableComponent {
public:
  using CreateInfo = MeshComponentCreateInfo;
  MeshComponent(CreateInfo info) : RenderableComponent(info.rigid_body_info) {
    m_handle = engine()->backend().graphics().create_renderable(info.render_info);
  }

  //////////////////////////////////////////////////////
  ////VIRTUAL FUNCTIONS/////////////////////////////////
  //////////////////////////////////////////////////////

  virtual auto name() -> std::string_view { return m_name; }

  virtual auto update(float dt) -> void {
    auto transform = this->world_transform();
    engine()->backend().graphics().set_transform(m_handle, transform);
  }

  virtual ~MeshComponent() = default;

  //////////////////////////////////////////////////////
  ////NON-VIRTUAL FUNCTIONS/////////////////////////////
  //////////////////////////////////////////////////////

protected:
  std::string m_name;
  Handle<gfx::Renderable> m_handle;
};
} // namespace meshi
