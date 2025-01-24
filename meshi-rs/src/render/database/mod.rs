pub mod error;
use dashi::utils::Handle;
pub use error::*;
pub mod json;
use std::collections::HashMap;
use std::fs;
mod images;
use images::*;
mod geometry;
use geometry::*;
pub mod font;
pub use font::*;

pub struct Database {
    ctx: *mut dashi::Context,
    scene: *mut miso::MisoScene,
    base_path: String,
    geometry: HashMap<String, GeometryResource>,
    images: HashMap<String, ImageResource>,
    fonts: HashMap<String, FontResource>,
}

impl Database {
    pub fn base_path(&self) -> &str {
        &self.base_path
    }

    pub fn new(
        base_path: &str,
        ctx: &mut dashi::Context,
        scene: &mut miso::MisoScene,
    ) -> Result<Self, Error> {
        println!("Loading Database {}", format!("{}/db.json", base_path));
        let json_data = fs::read_to_string(format!("{}/db.json", base_path))?;
        let info: json::Database = serde_json::from_str(&json_data)?;

        let images_cfg = load_db_images(&info);
        let fonts_cfg = load_db_ttfs(&info);
        let geometry_cfg = load_db_geometries(&info);

        let images = match images_cfg {
            Some(cfg) => cfg.into(),
            None => HashMap::default(),
        };

        let fonts = match fonts_cfg {
            Some(cfg) => cfg.into(),
            None => HashMap::default(),
        };

        let geometry = match geometry_cfg {
            Some(cfg) => cfg.into(),
            None => HashMap::default(),
        };

        Ok(Database {
            base_path: base_path.to_string(),
            ctx,
            scene,
            geometry,
            images,
            fonts,
        })
    }

    pub fn fetch_texture(&mut self, name: &str) -> Result<Handle<miso::Texture>, Error> {
        if let Some(thing) = self.images.get_mut(name) {
            if thing.loaded.is_none() {
                unsafe { thing.load_rgba8(&self.base_path, &mut *self.ctx, &mut *self.scene) };
            }

            if thing.loaded.is_none() {
                return Err(Error::LoadingError(LoadingError {
                    entry: thing.cfg.name.clone(),
                    path: thing.cfg.path.clone(),
                }));
            } else {
                return Ok(thing.loaded.as_ref().unwrap().clone());
            }
        }

        return Err(Error::LookupError(LookupError {
            entry: name.to_string(),
        }));
    }

    pub fn fetch_mesh(&mut self, name: &str) -> Result<Vec<MeshResource>, Error> {
        let db: *mut Database = self;
        if let Some(thing) = self.geometry.get_mut(name) {
            if thing.loaded.is_none() {
                unsafe { thing.load(&self.base_path, &mut *self.ctx, &mut *self.scene, &mut *db) };
            }

            if thing.loaded.is_none() {
                return Err(Error::LoadingError(LoadingError {
                    entry: thing.cfg.name.clone(),
                    path: thing.cfg.path.clone(),
                }));
            } else {
                return Ok(thing.loaded.as_ref().unwrap().clone());
            }
        }

        return Err(Error::LookupError(LookupError {
            entry: name.to_string(),
        }));
    }
}

#[test]
fn test_database() {
    //  let res = Database::new("/wksp/database");
    //  assert!(res.is_ok());
}
