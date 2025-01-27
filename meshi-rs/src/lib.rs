mod object;
mod render;
mod utils;
use dashi::utils::Handle;
use object::{FFIMeshObjectInfo, MeshObject};
use std::ffi::*;
use tracing::{info, Level};
use tracing_subscriber::FmtSubscriber;

use render::{RenderEngine, RenderEngineInfo};
use utils::timer::Timer;

#[repr(C)]
pub struct MeshiEngineInfo {
    pub application_name: *const c_char,
    pub application_location: *const c_char,
}

#[repr(C)]
pub struct MeshiEngine {
    name: String,
    render: RenderEngine,
    frame_timer: Timer,
}

impl MeshiEngine {
    fn new(info: &MeshiEngineInfo) -> Box<MeshiEngine> {
        assert!(!info.application_name.is_null());
        assert!(!info.application_location.is_null());
        let appname = unsafe { CStr::from_ptr(info.application_name) }
            .to_str()
            .unwrap();
        let mut appdir = unsafe { CStr::from_ptr(info.application_location) }
            .to_str()
            .unwrap();

        if appdir.is_empty() {
            appdir = ".";
        }

        info!("--INITIALIZING ENGINE--");
        info!("Application Name: '{}'", appname);
        info!("Application Dir: '{}'", appdir);

        Box::new(MeshiEngine {
            render: RenderEngine::new(&RenderEngineInfo {
                application_path: appdir.to_string(),
                scene_info: None,
            }),
            frame_timer: Timer::new(),
            name: appname.to_string(),
        })
    }

    fn update(&mut self) -> f32 {
        let dt = self.frame_timer.elapsed_ms_f32() / 1000.0;
        self.frame_timer.start();
        self.render.update(dt);

        return dt;
    }
}

#[repr(C)]
pub struct Renderable {}

#[no_mangle]
extern "C" fn meshi_make_engine(info: &MeshiEngineInfo) -> *mut MeshiEngine {
    // a builder for `FmtSubscriber`.
    let subscriber = FmtSubscriber::builder()
        // all spans/events with a level higher than TRACE (e.g, debug, info, warn, etc.)
        // will be written to stdout.
        .with_max_level(Level::TRACE)
        // completes the builder.
        .finish();

    tracing::subscriber::set_global_default(subscriber).expect("setting default subscriber failed");

    let mut e = MeshiEngine::new(&info);
    e.frame_timer.start();
    return Box::into_raw(e);
}
#[no_mangle]
pub extern "C" fn meshi_register_event_callback(
    engine: &mut MeshiEngine,
    user_data: *mut c_void,
    cb: extern "C" fn(*mut render::event::Event, *mut c_void),
) {
    engine.render.set_event_cb(cb, user_data);
}

#[no_mangle]
pub extern "C" fn meshi_register_mesh_object(
    engine: &mut MeshiEngine,
    info: &FFIMeshObjectInfo,
) -> Handle<MeshObject> {
    engine.render.register_mesh_object(info)
}

#[no_mangle]
pub extern "C" fn meshi_set_mesh_object_transform(
    engine: &mut MeshiEngine,
    handle: &Handle<MeshObject>,
    transform: &glam::Mat4,
) {
    engine
        .render
        .set_mesh_object_transform(handle.clone(), transform);
}

#[no_mangle]
pub extern "C" fn meshi_set_render_camera(
    engine: &mut MeshiEngine,
    transform: &glam::Mat4,
) {
    engine
        .render
        .set_mesh_object_transform(handle.clone(), transform);
}

#[no_mangle]
pub extern "C" fn meshi_update(engine: *mut MeshiEngine) -> c_float {
    return unsafe { &mut *(engine) }.update() as c_float;
}
