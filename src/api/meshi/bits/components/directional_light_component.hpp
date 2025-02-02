#pragma once
#include <meshi/bits/components/mesh_component.hpp>
#include <meshi/bits/util/slice.hpp>
namespace meshi {
class DirectionalLightComponent : public ActorComponent {
public:
  using CreateInfo = meshi::gfx::DirectionalLightInfo;
  DirectionalLightComponent(CreateInfo info = {}) {
    m_handle = engine()->backend().graphics().create_directional_light(info);
  }

  //////////////////////////////////////////////////////
  ////VIRTUAL FUNCTIONS/////////////////////////////////
  //////////////////////////////////////////////////////

  virtual auto update(float dt) -> void {
    auto transform = this->world_transform();
    //    engine()->backend().graphics().set_transform(m_handle, transform);
  }

  virtual ~DirectionalLightComponent() = default;

  //////////////////////////////////////////////////////
  ////NON-VIRTUAL FUNCTIONS/////////////////////////////
  //////////////////////////////////////////////////////

private:
  Handle<gfx::DirectionalLight> m_handle;
};
} // namespace meshi
