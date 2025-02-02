use dashi::utils::Handle;
use dashi::Context;
use dashi::ImageInfo;
use dashi::ImageViewInfo;
use dashi::SamplerInfo;
use image::ImageBuffer;
use image::Rgba;
use miso::Scene;
use miso::TextureInfo;
use tracing::debug;
use tracing::info;

use super::json;
use super::TTFont;
use std::collections::HashMap;
use std::fs;

#[derive(Default)]
pub struct ImageResource {
    pub cfg: json::ImageEntry,
    pub loaded: Option<Handle<miso::Texture>>,
}

impl ImageResource {
    pub fn load_default_image(ctx: &mut Context, scene: &mut Scene) -> Handle<miso::Texture> {
        // Define the size of the image
        const WIDTH: u32 = 512;
        const HEIGHT: u32 = 512;
        const CHECKER_SIZE: u32 = 64;

        // Create an ImageBuffer for the checkered pattern
        let mut img = ImageBuffer::new(WIDTH, HEIGHT);

        for y in 0..HEIGHT {
            for x in 0..WIDTH {
                // Determine the color based on the checker pattern
                let is_light = ((x / CHECKER_SIZE) + (y / CHECKER_SIZE)) % 2 == 0;
                let color = if is_light {
                    Rgba([200, 50, 200, 255]) // Light gray
                } else {
                    Rgba([50, 15, 50, 255]) // Dark gray
                };

                img.put_pixel(x, y, color);
            }
        }

        // Convert the image to RGBA8 format
        let rgba_image = img;

        let (width, height) = rgba_image.dimensions();
        let bytes = rgba_image.into_raw();
        assert!((width * height * 4) as usize == bytes.len());

        let img = ctx
            .make_image(&ImageInfo {
                debug_name: "default_checkered",
                dim: [width, height, 1],
                layers: 1,
                format: dashi::Format::RGBA8,
                mip_levels: 1,
                initial_data: Some(&bytes),
            })
            .unwrap();

        let view = ctx
            .make_image_view(&ImageViewInfo {
                debug_name: "default_checkered",
                img,
                ..Default::default()
            })
            .unwrap();

        let sampler = ctx
            .make_sampler(&SamplerInfo {
                ..Default::default()
            })
            .unwrap();

        info!("Registering default texture..");
        return scene.register_texture(&TextureInfo {
            image: img,
            view,
            sampler,
            dim: [width, height],
        });
    }

    pub fn load_from_uri(
        name: &str,
        bytes: &[u8],
        ctx: &mut Context,
        scene: &mut Scene,
    ) -> Handle<miso::Texture> {
        let img = image::load_from_memory(bytes).unwrap();
        // Convert the image to RGBA8 format
        let rgba_image = img.to_rgba8();

        let (width, height) = rgba_image.dimensions();
        let bytes = rgba_image.into_raw();
        assert!((width * height * 4) as usize == bytes.len());

        let img = ctx
            .make_image(&ImageInfo {
                debug_name: name,
                dim: [width, height, 1],
                layers: 1,
                format: dashi::Format::RGBA8,
                mip_levels: 1,
                initial_data: Some(&bytes),
            })
            .unwrap();

        let view = ctx
            .make_image_view(&ImageViewInfo {
                debug_name: name,
                img,
                ..Default::default()
            })
            .unwrap();

        let sampler = ctx
            .make_sampler(&SamplerInfo {
                ..Default::default()
            })
            .unwrap();

        debug!("Registering URI texture {}..", name);
        return scene.register_texture(&TextureInfo {
            image: img,
            view,
            sampler,
            dim: [width, height],
        });
    }

    pub fn load_from_gltf(
        name: &str,
        data: &gltf::image::Data,
        ctx: &mut Context,
        scene: &mut Scene,
    ) -> Handle<miso::Texture> {
        // Define the size of the image

        let width = data.width;
        let height = data.height;
        let bytes = &data.pixels;
        let format = match data.format {
            gltf::image::Format::R8 => dashi::Format::R8Sint,
            gltf::image::Format::R8G8B8A8 => dashi::Format::RGBA8,
            gltf::image::Format::R32G32B32A32FLOAT => dashi::Format::RGBA32F,
            _ => todo!(),
        };

        let img = ctx
            .make_image(&ImageInfo {
                debug_name: name,
                dim: [width, height, 1],
                layers: 1,
                format,
                mip_levels: 1,
                initial_data: Some(&bytes),
            })
            .unwrap();

        let view = ctx
            .make_image_view(&ImageViewInfo {
                debug_name: name,
                img,
                ..Default::default()
            })
            .unwrap();

        let sampler = ctx
            .make_sampler(&SamplerInfo {
                ..Default::default()
            })
            .unwrap();

        debug!("Registering embedded GLTF model texture {}..", name);
        return scene.register_texture(&TextureInfo {
            image: img,
            view,
            sampler,
            dim: [width, height],
        });
    }

    pub fn load_rgba8(&mut self, base_path: &str, ctx: &mut Context, scene: &mut Scene) {
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

        info!("Registering texture {}", self.cfg.name);
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

pub fn load_db_images(base_path: &str, cfg: &json::Database) -> Option<json::Image> {
    match &cfg.images {
        Some(path) => {
            let rpath = format!("{}/{}", base_path, path);
            let path = &rpath;
            debug!("Found image path {}", path);
            match fs::read_to_string(path) {
                Ok(json_data) => {
                    debug!("Loaded image database registry {}!", path);
                    let info: json::Image = serde_json::from_str(&json_data).unwrap();
                    return Some(info);
                }
                Err(_) => return None,
            }
        }
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
