#include <meshi/meshi.hpp>
#include <iostream>
auto main() -> int {
  auto info = meshi::EngineInfo {
        .application_name = "hello-engine ffi",
      };

  auto engine = meshi_make_engine(info);
  return 0;
}
