use dashi::utils::Handle;
use dashi::Context;
use dashi::ImageInfo;
use dashi::ImageViewInfo;
use dashi::SamplerInfo;
use miso::MisoScene;
use miso::TextureInfo;

use super::json;
use super::TTFont;
use std::collections::HashMap;
use std::fs;

pub struct ImageResource {
    pub cfg: json::ImageEntry,
    pub loaded: Option<Handle<miso::Texture>>,
}

impl ImageResource {
    pub fn load_rgba8(&mut self, base_path: &str, ctx: &mut Context, scene: &mut MisoScene) {
        let path = &format!("{}/{}", base_path, self.cfg.path.as_str());
        let img = image::open(&path).unwrap_or_default();

        // Convert the image to RGBA8 format
        let rgba_image = img.to_rgba8();

        // Flip the image vertically (upside down)
        //    let rgba_image = image::imageops::flip_vertical(&rgba_image);

        let (width, height) = rgba_image.dimensions();
        let bytes = rgba_image.into_raw();
        assert!((width * height * 4) as usize == bytes.len());

        let img = ctx
            .make_image(&ImageInfo {
                debug_name: &self.cfg.name,
                dim: [width, height, 1],
                layers: 1,
                format: dashi::Format::RGBA8,
                mip_levels: 1,
                initial_data: Some(&bytes),
            })
            .unwrap();

        let view = ctx
            .make_image_view(&ImageViewInfo {
                debug_name: &self.cfg.name,
                img,
                ..Default::default()
            })
            .unwrap();

        let sampler = ctx
            .make_sampler(&SamplerInfo {
                ..Default::default()
            })
            .unwrap();

        self.loaded = Some(scene.register_texture(&TextureInfo {
            image: img,
            view,
            sampler,
            dim: [width, height],
        }));
    }

    pub fn unload(&mut self) {
        self.loaded = None;
    }
}

impl From<json::Image> for HashMap<String, ImageResource> {
    fn from(value: json::Image) -> Self {
        let mut v = HashMap::new();
        for p in value.images {
            v.insert(
                p.name.clone(),
                ImageResource {
                    cfg: p,
                    loaded: None,
                },
            );
        }

        v
    }
}

pub fn load_db_images(cfg: &json::Database) -> Option<json::Image> {
    match &cfg.images {
        Some(path) => match fs::read_to_string(path) {
            Ok(json_data) => {
                let info: json::Image = serde_json::from_str(&json_data).unwrap();
                return Some(info);
            }
            Err(_) => return None,
        },
        None => return None,
    };
}

pub struct FontResource {
    pub cfg: json::TTFEntry,
    pub loaded: Option<TTFont>,
}

impl FontResource {
    pub fn load(&mut self, base_path: &str, typeset: &[char]) {
        self.loaded = Some(TTFont::new(
            &format!("{}/{}", base_path, self.cfg.path.as_str()),
            1280,
            1024,
            self.cfg.size as f32,
            typeset,
        ));
    }

    pub fn unload(&mut self) {
        self.loaded = None;
    }
}

impl From<json::TTF> for HashMap<String, FontResource> {
    fn from(value: json::TTF) -> Self {
        let mut v = HashMap::new();
        for p in value.fonts {
            v.insert(
                p.name.clone(),
                FontResource {
                    cfg: p,
                    loaded: None,
                },
            );
        }

        v
    }
}

pub fn load_db_ttfs(cfg: &json::Database) -> Option<json::TTF> {
    match &cfg.ttf {
        Some(path) => match fs::read_to_string(path) {
            Ok(json_data) => {
                let info: json::TTF = serde_json::from_str(&json_data).unwrap();
                return Some(info);
            }
            Err(_) => return None,
        },
        None => return None,
    };
}
