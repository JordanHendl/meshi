#include <iostream>
#include <meshi/meshi.hpp>

class MyObject : public meshi::Actor {
public:
  MyObject() : m_event(meshi::engine()->event().make_registry(this)) {
    auto comp = add_component<meshi::CubeMeshObject>();

    auto components = this->components<meshi::CubeMeshObject>();

    m_event.register_event(
        [](auto &event) { return event.event_type == meshi::EventType::QUIT; },
        [this](auto &event) {});
  }
  
  auto update(float dt) -> void override {
//    std::cout << "dt: " << dt << std::endl;
  }
private:
  meshi::EventRegister<MyObject> m_event;
};

////////////////////////////////////////////////////////////


class Application {
public:
  Application() : m_event(meshi::engine()->event().make_registry(this)) {
    m_event.register_event(
        [](auto &event) { return event.event_type == meshi::EventType::QUIT; },
        [this](auto &event) { m_running = false; });

    meshi::engine()->world().spawn_object<MyObject>();
  }

  auto run() -> void {
    while (m_running) {
      meshi::engine()->update();
    }
  }

private:
  bool m_running = true;
  meshi::EventRegister<Application> m_event;
};

auto main() -> int {
  meshi::initialize_meshi_engine(meshi::EngineInfo{
      .application_name = "Hello Engine!",
      .application_root = ".",
  });

  auto app = Application();
  app.run();

  return 0;
}
