use dashi::utils::Handle;
use tracing::{info, Level};
use dashi::Context;
use miso::{MaterialInfo, MisoScene};

use super::{json, Database};
use std::collections::HashMap;
use std::fs;

#[derive(Default)]
pub struct MaterialResource {
    pub cfg: json::MaterialEntry,
    pub loaded: Option<Handle<miso::Material>>,
}

impl MaterialResource {
    pub fn load(&mut self, scene: &mut MisoScene, db: &mut Database) {
        let base_color = if let Some(s) = self.cfg.base_color.as_ref() {
            db.fetch_texture(&s).unwrap_or_default()
        } else {
            Default::default()
        };

        let normal = if let Some(s) = self.cfg.normal.as_ref() {
            db.fetch_texture(&s).unwrap_or_default()
        } else {
            Default::default()
        };

        if base_color.valid() && normal.valid() {
            self.loaded = Some(scene.register_material(&MaterialInfo {
                name: self.cfg.name.clone(),
                passes: self.cfg.passes.clone(),
                base_color,
                normal,
                ..Default::default()
            }));
        }
    }

    pub fn unload(&mut self) {
        self.loaded = None;
    }
}

impl From<json::Materials> for HashMap<String, MaterialResource> {
    fn from(value: json::Materials) -> Self {
        let mut v = HashMap::new();
        for p in value.materials {
            v.insert(
                p.name.clone(),
                MaterialResource {
                    cfg: p,
                    loaded: None,
                },
            );
        }

        v
    }
}

pub fn load_db_materials(base_path: &str, cfg: &json::Database) -> Option<json::Materials> {
    match &cfg.materials {
        Some(path) => {
            let rpath = format!("{}/{}", base_path, path);
            let path = &path;
            info!("Found materials path {}", path);
            match fs::read_to_string(path) {
                Ok(json_data) => {
                    info!("Loaded materials database registry {}!", path);
                    let info: json::Materials = serde_json::from_str(&json_data).unwrap();
                    return Some(info);
                }
                Err(_) => return None,
            }
        }
        None => return None,
    };
}

