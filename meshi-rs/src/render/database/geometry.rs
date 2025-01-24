use dashi::{utils::Handle, BufferInfo, Context};
use glam::{IVec4, Vec2, Vec3, Vec4};
use gltf::Gltf;
use miso::{MisoScene, Vertex};
use serde::{Deserialize, Serialize};

use super::{json, Database, ImageResource};
use std::{collections::HashMap, fs, path::Path};

pub struct MeshInfo {
    name: String,
    vertices: Vec<miso::Vertex>,
    indices: Vec<u32>,
}

pub struct GeometryInfo {
    name: String,
    meshes: Vec<MeshInfo>,
}

#[derive(Default, Clone)]
pub struct MeshResource {
    name: String,
    m: Handle<miso::Mesh>,
    mat: Handle<miso::Material>,
}
pub struct GeometryResource {
    pub cfg: json::GeometryEntry,
    pub loaded: Option<Vec<MeshResource>>,
}

impl GeometryResource {
    pub fn load(&mut self, base_path: &str, ctx: &mut Context, scene: &mut MisoScene, db: &mut Database) {
        let file = format!("{}/{}", base_path, self.cfg.path);
        let mut meshes = Vec::new();
        if let Some(gltf) = load_gltf_model(&file) {
            for mesh in gltf.meshes {
                let vertices = ctx
                    .make_buffer(&BufferInfo {
                        debug_name: &format!("{} Vertices", self.cfg.name),
                        byte_size: (std::mem::size_of::<miso::Vertex>() * mesh.vertices.len())
                            as u32,
                        visibility: dashi::MemoryVisibility::Gpu,
                        usage: dashi::BufferUsage::VERTEX,
                        initial_data: Some(unsafe { mesh.vertices.as_slice().align_to::<u8>().1 }),
                    })
                    .unwrap();

                let indices = ctx
                    .make_buffer(&BufferInfo {
                        debug_name: &format!("{} Indices", self.cfg.name),
                        byte_size: (std::mem::size_of::<u32>() * mesh.indices.len()) as u32,
                        visibility: dashi::MemoryVisibility::Gpu,
                        usage: dashi::BufferUsage::INDEX,
                        initial_data: Some(unsafe { mesh.indices.as_slice().align_to::<u8>().1 }),
                    })
                    .unwrap();

                let  base_color = mesh.material.textures.get(&TextureType::Diffuse).unwrap_or(&"".to_string()).clone();
                let  normal = mesh.material.textures.get(&TextureType::Normal).unwrap_or(&"".to_string()).clone();
                let mat_info = miso::MaterialInfo {
                    name: mesh.material.name.clone(),
                    passes: vec!["ALL".to_string()],
                    base_color: db.fetch_texture(&base_color).unwrap_or_default(),
                    normal: db.fetch_texture(&normal).unwrap_or_default(),
                };

                let mat = scene.register_material(&mat_info);

                let m = scene.register_mesh(&miso::MeshInfo {
                    name: self.cfg.name.clone(),
                    vertices,
                    num_vertices: mesh.vertices.len(),
                    indices,
                    num_indices: mesh.indices.len(),
                });

                meshes.push(MeshResource {
                    name: mesh.name.clone(),
                    m,
                    mat,
                });
            }
            self.loaded = Some(meshes);
        }
    }

    pub fn unload(&mut self) {
        self.loaded = None;
    }
}

impl From<json::Geometry> for HashMap<String, GeometryResource> {
    fn from(value: json::Geometry) -> Self {
        let mut v = HashMap::new();
        for geometry in value.geometry {
            v.insert(
                geometry.name.clone(),
                GeometryResource {
                    cfg: geometry,
                    loaded: None,
                },
            );
        }

        v
    }
}

pub fn load_db_geometries(cfg: &json::Database) -> Option<json::Geometry> {
    match &cfg.geometry {
        Some(path) => match fs::read_to_string(path) {
            Ok(json_data) => {
                let info: json::Geometry = serde_json::from_str(&json_data).unwrap();
                return Some(info);
            }
            Err(_) => return None,
        },
        None => return None,
    };
}

#[derive(Eq, PartialEq, Hash, Clone, Debug, Serialize, Deserialize)]
pub enum TextureType {
    Diffuse,
    Specular,
    Roughness,
    Normal,
    Occlusion,
    Emissive,
    Albedo,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
struct Material {
    pub name: String,
    pub textures: HashMap<TextureType, String>,
}

type Index = u32;
#[derive(Debug, Clone)]
struct Mesh {
    pub name: String,
    pub vertices: Vec<Vertex>,
    pub indices: Vec<Index>,
    pub material: Material,
}

#[derive(Debug, Clone)]
struct Model {
    pub meshes: Vec<Mesh>,
}

fn load_gltf_model<P: AsRef<Path>>(path: P) -> Option<Model> {
    let (gltf, buffers, _) = gltf::import(path).expect("Failed to load glTF file");
    let mut meshes = Vec::new();

    for mesh in gltf.meshes() {
        let mut vertices = Vec::new();
        let mut indices = Vec::new();
        let mut material = None;
        for primitive in mesh.primitives() {
            let reader = primitive.reader(|buffer| Some(&buffers[buffer.index()]));

            let mut bone_ids: Vec<[u16; 4]> = Vec::new();
            let mut bone_weights: Vec<[f32; 4]> = Vec::new();

            // Extract Positions
            let positions: Vec<[f32; 3]> = reader.read_positions()?.collect();

            // Extract Normals
            let normals: Vec<[f32; 3]> = reader.read_normals()?.collect();

            // Extract Texture Coordinates
            let tex_coords: Vec<[f32; 2]> = reader.read_tex_coords(0)?.into_f32().collect();

            // Extract Bone Weights
            if let Some(joints) = reader.read_joints(0) {
                bone_ids = joints.into_u16().collect();
            } else {
                bone_ids.resize(positions.len(), [0, 0, 0, 0]);
            }

            if let Some(weights) = reader.read_weights(0) {
                bone_weights = weights.into_f32().collect();
            } else {
                bone_weights.resize(positions.len(), [0.0, 0.0, 0.0, 0.0]);
            }

            let joint_ids: Vec<[i32; 4]> = bone_ids
                .iter()
                .map(|a| return [a[0] as i32, a[1] as i32, a[2] as i32, a[3] as i32])
                .collect();

            // Collect vertex data
            for i in 0..positions.len() {
                vertices.push(Vertex {
                    position: (Vec3::from(positions[i]), 1.0).into(),
                    normal: (Vec3::from(normals[i]), 1.0).into(),
                    tex_coords: Vec2::from(tex_coords[i]),
                    joint_ids: IVec4::from(joint_ids[i]),
                    joints: Vec4::from(bone_weights[i]),
                });
            }

            // Extract Indices
            if let Some(indices_data) = reader.read_indices() {
                indices.extend(indices_data.into_u32());
            }

            let mut mat_name = "Unknown".to_string();
            // Extract Material Information
            let mat = primitive.material();
            {
                if let Some(name) = mat.name() {
                    mat_name = name.to_string();
                }
                let mut textures = HashMap::new();

                if let Some(info) = mat.pbr_metallic_roughness().base_color_texture() {
                    if let Some(name) = info.texture().name() {
                        textures.insert(TextureType::Diffuse, name.to_string());
                    }
                }

                if let Some(info) = mat.normal_texture() {
                    if let Some(name) = info.texture().name() {
                        textures.insert(TextureType::Normal, name.to_string());
                    }
                }

                if let Some(info) = mat.occlusion_texture() {
                    if let Some(name) = info.texture().name() {
                        textures.insert(TextureType::Occlusion, name.to_string());
                    }
                }

                if let Some(info) = mat.emissive_texture() {
                    if let Some(name) = info.texture().name() {
                        textures.insert(TextureType::Emissive, name.to_string());
                    }
                }

                material = Some(Material {
                    name: mat_name,
                    textures,
                });
            }
        }

        meshes.push(Mesh {
            name: mesh.name().unwrap_or("None").to_string(),
            vertices,
            indices,
            material: material.unwrap(),
        });
    }

    Some(Model { meshes })
}
