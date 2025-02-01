use dashi::utils::{Handle, Pool};
use glam::*;

#[repr(C)]
#[derive(Clone, Copy)]
pub struct EnvironmentInfo {
    gravity_mps: f32,
}

impl Default for EnvironmentInfo {
    fn default() -> Self {
        Self { gravity_mps: -9.8 }
    }
}

#[repr(C)]
#[derive(Default, Clone, Copy)]
pub struct SimulationInfo {
    pub environment: EnvironmentInfo,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct MaterialInfo {
    pub dynamic_friction_m: f32,
}

impl Default for MaterialInfo {
    fn default() -> Self {
        Self {
            dynamic_friction_m: 5.0,
        }
    }
}

#[repr(C)]
#[derive(Default, Clone, Copy)]
pub struct ForceApplyInfo {
    amt: Vec3,
}

#[repr(C)]
#[derive(Default, Clone, Copy)]
pub struct RigidBodyInfo {
    pub material: Handle<Material>,
    pub initial_position: Vec3,
    pub initial_velocity: Vec3,
    pub initial_rotation: glam::Quat,
    pub has_gravity: u32,
}

#[repr(packed)]
#[derive(Default)]
pub struct Material {
    info: MaterialInfo,
}

impl From<&MaterialInfo> for Material {
    fn from(value: &MaterialInfo) -> Self {
        Self {
            info: value.clone(),
        }
    }
}
#[repr(packed)]
#[derive(Default)]
pub struct RigidBody {
    position: Vec3,
    velocity: Vec3,
    rotation: Quat,
    has_gravity: u32,
    material: Handle<Material>,
}

impl RigidBody {
    pub fn apply_force(&mut self, force: Vec3) {
        self.velocity = self.velocity + force;
    }

    pub fn dampen_velocity(&mut self, mat: &Material, dt: &Vec3) {
        let dfc = mat.info.dynamic_friction_m;
        self.velocity = self.velocity - (vec3(dfc, dfc, dfc) * dt);
    }
}

#[repr(C)]
#[derive(Default)]
pub struct ActorStatus {
    position: Vec3,
    rotation: Quat,
}

impl From<&RigidBodyInfo> for RigidBody {
    fn from(value: &RigidBodyInfo) -> Self {
        RigidBody {
            position: value.initial_position,
            velocity: Default::default(),
            rotation: value.initial_rotation,
            has_gravity: value.has_gravity,
            material: value.material,
        }
    }
}
impl From<&RigidBody> for ActorStatus {
    fn from(value: &RigidBody) -> Self {
        Self {
            position: value.position,
            rotation: value.rotation,
        }
    }
}
pub struct PhysicsSimulation {
    info: SimulationInfo,
    materials: Pool<Material>,
    rigid_bodies: Pool<RigidBody>,
    default_material: Handle<Material>,
}

impl PhysicsSimulation {
    pub fn new(info: &SimulationInfo) -> Self {
        let mut s = Self {
            info: info.clone(),
            materials: Default::default(),
            rigid_bodies: Default::default(),
            default_material: Default::default(),
        };

        let default = s.materials.insert(Default::default()).unwrap();
        s.default_material = default;
        s
    }

    pub fn update(&mut self, dt: f32) {
        let dt = vec3(dt, dt, dt);
        self.rigid_bodies.for_each_occupied_mut(|r| {
            let mat = self.materials.get_ref(r.material).unwrap();
            if r.has_gravity == 1 {
                r.apply_force(vec3(0.0, self.info.environment.gravity_mps, 0.0) * dt);
            }

            let adj_velocity = r.velocity * dt;
            let pos = r.position;
            r.position = pos + adj_velocity;

            r.dampen_velocity(mat, &dt);
        });
    }

    pub fn create_material(&mut self, info: &MaterialInfo) -> Handle<Material> {
        self.materials.insert(info.into()).unwrap()
    }

    pub fn create_rigid_body(&mut self, info: &RigidBodyInfo) -> Handle<RigidBody> {
        let mut info = info.clone();
        if !info.material.valid() {
            info.material = self.default_material;
        }

        self.rigid_bodies.insert((&info).into()).unwrap()
    }

    pub fn release_material(&mut self, h: Handle<Material>) {
        self.materials.release(h);
    }

    pub fn release_rigid_body(&mut self, h: Handle<RigidBody>) {
        self.rigid_bodies.release(h);
    }

    pub fn apply_rigid_body_force(&mut self, h: Handle<RigidBody>, info: &ForceApplyInfo) {
        self.rigid_bodies
            .get_mut_ref(h)
            .unwrap()
            .apply_force(info.amt);
    }

    pub fn get_rigid_body_info(&mut self, h: Handle<RigidBody>) -> ActorStatus {
        assert!(h.valid());
        self.rigid_bodies.get_ref(h).unwrap().into()
    }
}
