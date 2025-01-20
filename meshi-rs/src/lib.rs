mod render;
use std::ffi::*;

#[repr(C)]
pub struct MeshiEngineInfo {
    pub application_name: *const c_char,
    pub application_location: *const c_char,
}

#[repr(C)]
#[derive(Default)]
pub struct MeshiEngine {}

#[repr(C)]
pub struct Renderable {}

#[no_mangle]
extern "C" fn meshi_make_engine(info: &MeshiEngineInfo) -> *mut MeshiEngine {
    let cstr = unsafe { CStr::from_ptr(info.application_name) }.to_str().unwrap();
    println!("Rust make engine: {}", cstr);
    let l = MeshiEngine::default();
    return Box::into_raw(Box::new(l));
}

#[no_mangle]
pub extern "C" fn meshi_register_renderable(engine: &mut MeshiEngine) {

}
