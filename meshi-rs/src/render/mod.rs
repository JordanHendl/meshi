use dashi::*;

//mod database;
mod config;
pub struct SceneInfo<'a> {
    models: &'a [&'a str],
    images: &'a [&'a str],
}

#[derive(Default)]
pub struct RenderEngineInfo<'a> {
    config_path: Option<String>,
    scene_info: Option<SceneInfo<'a>>,
}

pub struct RenderEngine {
    ctx: dashi::Context,
    scene: miso::MisoScene,
}

impl RenderEngine {
    pub fn new(info: &RenderEngineInfo) -> Self {
        let device = DeviceSelector::new()
            .unwrap()
            .select(DeviceFilter::default().add_required_type(DeviceType::Dedicated))
            .unwrap_or_default();

        println!("[MESHI] Initializing Render Engine with device {}", device);
        
        let cfg = config::RenderEngineConfig::default();
        todo!();
        // The GPU context that holds all the data.
        let mut ctx = gpu::Context::new(&ContextInfo { device }).unwrap();
        let mut scene = miso::MisoScene::new(&mut ctx, &miso::MisoSceneInfo { cfg: cfg.scene_cfg_path });

        todo!()
    }

    pub fn set_scene(&mut self, info: &SceneInfo) {}
}
