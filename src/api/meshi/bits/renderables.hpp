#pragma once
#include <meshi/bits/util/slice.hpp>
#include <meshi/bits/base.hpp>
namespace meshi {
  class Renderable : public Object {
    public:
      inline auto get_components() -> Slice<Component> {}; 
  };


  class Entity : public Renderable {
    public:

  };

  class Actor : public Renderable {
    public:

  };
}
