use crate::render::database::Database;
use dashi::utils::Handle;
use glam::Mat4;
use tracing::{info, Level};
use tracing_subscriber::FmtSubscriber;

use std::ffi::{c_char, c_int, CStr};

#[repr(C)]
pub struct FFIMeshObjectInfo {
    pub mesh: *const c_char,
    pub material: *const c_char,
    pub transform: glam::Mat4,
}

pub struct MeshObjectInfo {
    pub mesh: &'static str,
    pub material: &'static str,
    pub transform: glam::Mat4,
}

impl From<&FFIMeshObjectInfo> for MeshObjectInfo {
    fn from(value: &FFIMeshObjectInfo) -> Self {
        unsafe {
            let mesh = CStr::from_ptr(value.mesh).to_str().unwrap_or_default();
            let mut material = CStr::from_ptr(value.material).to_str().unwrap_or_default();

            if material.is_empty() {
                material = mesh;
            }
            Self {
                mesh,
                material,
                transform: value.transform,
            }
        }
    }
}

impl MeshObjectInfo {
    pub fn make_object(&self, db: &mut Database, scene: &mut miso::MisoScene) -> MeshObject {
        info!(
            "Registering Mesh Renderable {}||{}",
            self.mesh, self.material
        );
        if let Ok(mesh) = db.fetch_mesh(self.mesh) {
            if let Ok(material) = db.fetch_material(self.material) {
                return MeshObject {
                    handle: scene.register_object(&miso::ObjectInfo {
                        mesh,
                        material,
                        transform: self.transform,
                    }),
                    transform: self.transform,
                };
            }
        }

        Default::default()
    }
}

#[derive(Default)]
pub struct MeshObject {
    pub handle: Handle<miso::Renderable>,
    pub transform: Mat4,
}
