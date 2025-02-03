mod object;
mod physics;
mod render;
mod utils;
use dashi::utils::Handle;
use glam::Mat4;
use object::{FFIMeshObjectInfo, MeshObject};
use physics::{ForceApplyInfo, PhysicsSimulation};
use render::{RenderEngine, RenderEngineInfo};
use std::ffi::*;
use tracing::{info, Level};
use tracing_subscriber::FmtSubscriber;
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
    physics: Box<PhysicsSimulation>,
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
            physics: Box::new(PhysicsSimulation::new(&Default::default())),
            frame_timer: Timer::new(),
            name: appname.to_string(),
        })
    }

    fn update(&mut self) -> f32 {
        self.frame_timer.stop();
        let dt = self.frame_timer.elapsed_micro_f32();
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
        .with_max_level(Level::INFO)
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
pub extern "C" fn meshi_update(engine: *mut MeshiEngine) -> c_float {
    return unsafe { &mut *(engine) }.update() as c_float;
}

////////////////////////////////////////////
//////////////////GRAPHICS//////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
#[no_mangle]
pub extern "C" fn meshi_get_graphics_system(engine: *mut MeshiEngine) -> *mut RenderEngine {
    return &mut (unsafe { &mut *(engine) }.render);
}

#[no_mangle]
pub extern "C" fn meshi_gfx_create_renderable(
    render: &mut RenderEngine,
    info: &FFIMeshObjectInfo,
) -> Handle<MeshObject> {
    render.register_mesh_object(info)
}

#[no_mangle]
pub extern "C" fn meshi_gfx_set_renderable_transform(
    render: &mut RenderEngine,
    h: &Handle<MeshObject>,
    transform: &Mat4,
) {
    render.set_mesh_object_transform(*h, transform);
}

#[no_mangle]
pub extern "C" fn meshi_gfx_create_directional_light(
    render: &mut RenderEngine,
    info: &miso::DirectionalLightInfo,
) -> Handle<miso::DirectionalLight> {
    render.register_directional_light(info)
}

#[no_mangle]
pub extern "C" fn meshi_gfx_set_directional_light_transform(
    render: &mut RenderEngine,
    h: &Handle<miso::DirectionalLight>,
    transform: &Mat4,
) {
    //    render.register_directional_light(info)
}

#[no_mangle]
pub extern "C" fn meshi_gfx_set_camera(render: &mut RenderEngine, transform: &Mat4) {
    render.set_camera(transform);
}

#[no_mangle]
pub extern "C" fn meshi_gfx_set_projection(render: &mut RenderEngine, transform: &Mat4) {
    render.set_projection(transform);
}

#[no_mangle]
pub extern "C" fn meshi_gfx_capture_mouse(render: &mut RenderEngine, value: i32) {
    if value == 0 {
        render.set_capture_mouse(false);
    } else {
        render.set_capture_mouse(true);
    }
}


////////////////////////////////////////////
//////////////////PHYSICS///////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
#[no_mangle]
pub extern "C" fn meshi_get_physics_system(engine: *mut MeshiEngine) -> *mut PhysicsSimulation {
    return &mut *(unsafe { &mut *(engine) }.physics);
}

#[no_mangle]
pub extern "C" fn meshi_physx_create_material(
    physics: &mut PhysicsSimulation,
    info: &physics::MaterialInfo,
) -> Handle<physics::Material> {
    physics.create_material(info)
}

#[no_mangle]
pub extern "C" fn meshi_physx_release_material(
    physics: &mut PhysicsSimulation,
    h: &Handle<physics::Material>,
) {
    physics.release_material(*h);
}

#[no_mangle]
pub extern "C" fn meshi_physx_create_rigid_body(
    physics: &mut PhysicsSimulation,
    info: &physics::RigidBodyInfo,
) -> Handle<physics::RigidBody> {
    physics.create_rigid_body(info)
}

#[no_mangle]
pub extern "C" fn meshi_physx_release_rigid_body(
    physics: &mut PhysicsSimulation,
    h: &Handle<physics::RigidBody>,
) {
    physics.release_rigid_body(*h);
}

#[no_mangle]
pub extern "C" fn meshi_physx_apply_force_to_rigid_body(
    physics: &mut PhysicsSimulation,
    h: &Handle<physics::RigidBody>,
    info: &ForceApplyInfo,
) {
    physics.apply_rigid_body_force(*h, info);
}

#[no_mangle]
pub extern "C" fn meshi_physx_get_rigid_body_status(
    physics: &mut PhysicsSimulation,
    h: &Handle<physics::RigidBody>,
) -> *const physics::ActorStatus {
    &physics.get_rigid_body_info(*h)
}
