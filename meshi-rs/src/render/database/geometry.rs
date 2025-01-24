use dashi::{utils::Handle, Context};
use miso::MisoScene;

use super::json;
use std::{collections::HashMap, fs};

pub struct MeshInfo {
    name: String,
    vertices: Vec<miso::Vertex>,
    indices: Vec<u32>,
}

pub struct GeometryInfo {
    name: String,
    meshes: Vec<MeshInfo>,
}

pub struct GeometryResource {
    pub cfg: json::GeometryEntry,
    pub loaded: Option<Handle<miso::Mesh>>,
}

impl GeometryResource {
    pub fn load(&mut self, base_path: &str, ctx: &mut Context, scene: &mut MisoScene) {}

    pub fn unload(&mut self) {
        self.loaded = None;
    }
}

impl From<json::Geometry> for HashMap<String, GeometryResource> {
    fn from(value: json::Geometry) -> Self {
        let mut v = HashMap::new();
        for geometry in value.geometry {
            v.insert(
                geometry.name.clone(),
                GeometryResource {
                    cfg: geometry,
                    loaded: None,
                },
            );
        }

        v
    }
}

pub fn load_db_geometries(cfg: &json::Database) -> Option<json::Geometry> {
    match &cfg.geometry {
        Some(path) => match fs::read_to_string(path) {
            Ok(json_data) => {
                let info: json::Geometry = serde_json::from_str(&json_data).unwrap();
                return Some(info);
            }
            Err(_) => return None,
        },
        None => return None,
    };
}
