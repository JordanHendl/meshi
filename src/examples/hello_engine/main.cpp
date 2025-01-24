#include <iostream>
#include <meshi/meshi.hpp>
auto main() -> int {
  auto info = meshi::EngineInfo{
      .application_name = "hello-engine ffi",
  };

  auto engine = meshi::Engine::make(info).unwrap();

  while(true) {
   engine.update(); 
  }
  return 0;
}
