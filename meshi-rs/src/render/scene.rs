use dashi::utils::Handle;
use glam::Mat4;

struct Drawable {
    handle: Handle<miso::Renderable>,
    transform: Mat4,
}

struct Scene {
    renderables: miso::ResourceList<Drawable>,
}
