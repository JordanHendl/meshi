use dashi::*;
use database::Database;

mod database;
mod scene;
mod config;

pub struct SceneInfo<'a> {
    models: &'a [&'a str],
    images: &'a [&'a str],
}

#[derive(Default)]
pub struct RenderEngineInfo<'a> {
    pub application_path: String,
    pub scene_info: Option<SceneInfo<'a>>,
}

pub struct RenderEngine {
    ctx: dashi::Context,
    scene: miso::MisoScene,
    database: Database,
}

impl RenderEngine {
    pub fn new(info: &RenderEngineInfo) -> Self {
        let device = DeviceSelector::new()
            .unwrap()
            .select(DeviceFilter::default().add_required_type(DeviceType::Dedicated))
            .unwrap_or_default();

        println!("[MESHI] Initializing Render Engine with device {}", device);

        let cfg = config::RenderEngineConfig {
            scene_cfg_path: Some(format!("{}/miso_scene.json", info.application_path)),
            database_path: Some(format!("{}/database/", info.application_path)),
        };

        // The GPU context that holds all the data.
        let mut ctx = gpu::Context::new(&ContextInfo { device }).unwrap();
        let scene = miso::MisoScene::new(
            &mut ctx,
            &miso::MisoSceneInfo {
                cfg: cfg.scene_cfg_path,
            },
        );
        
        let database = Database::new(cfg.database_path, &mut ctx, & mut scene);
        let mut s = Self { ctx, scene, database};

        s
    }

    pub fn set_scene(&mut self, info: &SceneInfo) {
        todo!()
    }
}
