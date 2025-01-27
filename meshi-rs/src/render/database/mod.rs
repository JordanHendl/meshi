pub mod error;
use dashi::utils::Handle;
use miso::MaterialInfo;
use tracing::{info, Level};
use tracing_subscriber::FmtSubscriber;

pub use error::*;
pub mod json;
use std::collections::HashMap;
use std::fs;
mod images;
use images::*;
mod material;
use material::*;
mod geometry;
use geometry::*;
pub mod font;
mod geometry_primitives;
pub use font::*;

struct Defaults {
    image: Handle<miso::Texture>,
    material: Handle<miso::Material>,
}

pub struct Database {
    ctx: *mut dashi::Context,
    scene: *mut miso::MisoScene,
    base_path: String,
    geometry: HashMap<String, Handle<miso::Mesh>>,
    images: HashMap<String, ImageResource>,
    materials: HashMap<String, Handle<miso::Material>>,
    fonts: HashMap<String, FontResource>,
    defaults: Defaults,
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
        info!("Loading Database {}", format!("{}/db.json", base_path));
        let json_data = fs::read_to_string(format!("{}/db.json", base_path))?;
        let info: json::Database = serde_json::from_str(&json_data)?;

        let images_cfg = load_db_images(&info);
        let fonts_cfg = load_db_ttfs(&info);
        let geometry_cfg = load_db_geometries(&info);
        let material_cfg = load_db_materials(&info);

        let images = match images_cfg {
            Some(cfg) => cfg.into(),
            None => HashMap::default(),
        };

        let materials = match material_cfg {
            Some(cfg) => cfg.into(),
            None => HashMap::default(),
        };

        let fonts = match fonts_cfg {
            Some(cfg) => cfg.into(),
            None => HashMap::default(),
        };

        let mut geometry = match geometry_cfg {
            Some(cfg) => cfg.into(),
            None => HashMap::default(),
        };

//        geometry.insert(
//            "MESHI_TRIANGLE".to_string(),
//            GeometryResource {
//                cfg: json::GeometryEntry {
//                    name: "MESHI".to_string(),
//                    path: "".to_string(),
//                },
//                loaded: Some(geometry_primitives::make_triangle(
//                    &Default::default(),
//                    ctx,
//                    scene,
//                )),
//            },
//        );

        geometry.insert(
            "MESHI_CUBE".to_string(),
            GeometryResource {
                cfg: json::GeometryEntry {
                    name: "MESHI".to_string(),
                    path: "".to_string(),
                },
                loaded: Some(geometry_primitives::make_cube(
                    &Default::default(),
                    ctx,
                    scene,
                )),
            },
        );

//        geometry.insert(
//            "MESHI_SPHERE".to_string(),
//            GeometryResource {
//                cfg: json::GeometryEntry {
//                    name: "MESHI".to_string(),
//                    path: "".to_string(),
//                },
//                loaded: Some(geometry_primitives::make_sphere(
//                    &Default::default(),
//                    ctx,
//                    scene,
//                )),
//            },
//        );

        let default_texture = ImageResource::load_default_image(ctx, scene);

        info!("Registering default material..");
        let default_material = scene.register_material(&MaterialInfo {
            name: "DEFAULT".to_string(),
            //passes: vec!["ALL".to_string()],
            passes: vec!["non-transparent".to_string()],
            base_color: default_texture,
            normal: Default::default(),
        });

        let mut db = Database {
            base_path: base_path.to_string(),
            ctx,
            scene,
            images,
            fonts,
            geometry: Default::default(),
            materials: Default::default(),
            defaults: Defaults {
                image: default_texture,
                material: default_material,
            },
        };

        let ptr: *mut Database = &mut db;

        // Models HAVE to be loaded before materials, as they add materials.
        for (name, mut model) in geometry {
            if model.loaded.is_none() {
                model.load(base_path, ctx, scene, unsafe { &mut *ptr });
            }

            if let Some(m) = model.loaded {
                for mesh in m.meshes {
                    info!("Making mesh {}.{} available", model.cfg.name, mesh.name);
                    db.geometry
                        .insert(format!("{}.{}", model.cfg.name, mesh.name), mesh.m);
                }
            }
        }

        // Images MUST be parsed before materials, as this loads images if they are used.
        for (name, mut m) in materials {
            m.load(scene, unsafe { &mut *ptr });
            if let Some(mat) = m.loaded {
                db.materials.insert(name, mat);
            }
        }

        Ok(db)
    }

    fn insert_material(&mut self, name: &str, mat: Handle<miso::Material>) {
        if self.materials.get(name).is_none() {
            self.materials.insert(name.to_string(), mat);
        }
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

    pub fn fetch_material(&mut self, name: &str) -> Result<Handle<miso::Material>, Error> {
        if let Some(thing) = self.materials.get(name) {
            return Ok(*thing);
        } else {
            info!("Unable to fetch material {}. Returning default...", name);
            return Ok(self.defaults.material);
        }
    }

    pub fn fetch_mesh(&mut self, name: &str) -> Result<Handle<miso::Mesh>, Error> {
        let db: *mut Database = self;
        if let Some(thing) = self.geometry.get_mut(name) {
            return Ok(*thing);
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
