use dashi::{utils::Handle, BufferInfo, Context};
use glam::{IVec4, Mat4, Quat, Vec2, Vec3, Vec4};
use miso::{Scene, Vertex};
use serde::{Deserialize, Serialize};
use tracing::debug;

use super::{json, Database};
use std::{collections::HashMap, fs};

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
pub struct SubmeshResource {
    pub m: Handle<miso::Mesh>,
    pub mat: Handle<miso::Material>,
}

#[derive(Default, Clone)]
pub struct MeshResource {
    pub name: String,
    pub submeshes: Vec<SubmeshResource>,
}

#[derive(Default, Clone)]
pub struct ModelResource {
    pub meshes: Vec<MeshResource>,
}
pub struct GeometryResource {
    pub cfg: json::GeometryEntry,
    pub loaded: Option<ModelResource>,
}

impl GeometryResource {
    pub fn load(
        &mut self,
        base_path: &str,
        ctx: &mut Context,
        scene: &mut Scene,
        db: &mut Database,
    ) {
        let file = format!("{}/{}", base_path, self.cfg.path);

        let mut meshes = Vec::new();
        if let Some(gltf) = load_gltf_model(&file, db) {
            for mesh in gltf.meshes {
                let mut submeshes = Vec::new();
                for (idx, submesh) in mesh.sub_meshes.iter().enumerate() {
                    let name = format!("{}.{}[{}]", self.cfg.name, mesh.name, idx);
                    debug!("Loading Mesh {}...", &name);
                    let vertices = ctx
                        .make_buffer(&BufferInfo {
                            debug_name: &format!("{} Vertices", name),
                            byte_size: (std::mem::size_of::<miso::Vertex>()
                                * submesh.vertices.len())
                                as u32,
                            visibility: dashi::MemoryVisibility::Gpu,
                            usage: dashi::BufferUsage::VERTEX,
                            initial_data: Some(unsafe {
                                submesh.vertices.as_slice().align_to::<u8>().1
                            }),
                        })
                        .unwrap();

                    let indices = ctx
                        .make_buffer(&BufferInfo {
                            debug_name: &format!("{} Indices", name),
                            byte_size: (std::mem::size_of::<u32>() * submesh.indices.len()) as u32,
                            visibility: dashi::MemoryVisibility::Gpu,
                            usage: dashi::BufferUsage::INDEX,
                            initial_data: Some(unsafe {
                                submesh.indices.as_slice().align_to::<u8>().1
                            }),
                        })
                        .unwrap();

                    let base_color = submesh
                        .material
                        .textures
                        .get(&TextureType::Diffuse)
                        .unwrap_or(&"[NOT FOUND]".to_string())
                        .clone();
                    let normal = submesh
                        .material
                        .textures
                        .get(&TextureType::Normal)
                        .unwrap_or(&"[NOT FOUND]".to_string())
                        .clone();
                    let emissive = submesh
                        .material
                        .textures
                        .get(&TextureType::Emissive)
                        .unwrap_or(&"[NOT FOUND]".to_string())
                        .clone();

                    debug!(
                        "Attempting to load material {}.{} Textures:
                      -- Base Color : {}
                      -- Normal: {},
                      -- Emissive: {}",
                        name, submesh.material.name, base_color, normal, emissive
                    );

                    let mat_info = miso::MaterialInfo {
                        name: submesh.material.name.clone(),
                        passes: vec!["ALL".to_string()],
                        base_color: db.fetch_texture(&base_color).unwrap_or_default(),
                        normal: db.fetch_texture(&normal).unwrap_or_default(),
                        emissive: db.fetch_texture(&emissive).unwrap_or_default(),
                        base_color_factor: submesh.material.base_color_factor,
                        emissive_factor: submesh.material.emissive_factor,
                    };

                    debug!("Registering Mesh Material {}", &name);
                    let mat = scene.register_material(&mat_info);
                    db.insert_material(&name, mat);

                    debug!("Registering Mesh {}", &name);
                    let m = scene.register_mesh(&miso::MeshInfo {
                        name: self.cfg.name.clone(),
                        vertices,
                        num_vertices: submesh.vertices.len(),
                        indices,
                        num_indices: submesh.indices.len(),
                    });

                    submeshes.push(SubmeshResource { m, mat });
                }
                meshes.push(MeshResource {
                    name: mesh.name.clone(),
                    submeshes,
                });
            }
            self.loaded = Some(ModelResource { meshes });
        } else {
            debug!("Failed to load {}!", self.cfg.name);
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

pub fn load_db_geometries(base_path: &str, cfg: &json::Database) -> Option<json::Geometry> {
    match &cfg.geometry {
        Some(path) => {
            let rpath = format!("{}/{}", base_path, path);
            debug!("Found geometry path {}", &rpath);
            match fs::read_to_string(&rpath) {
                Ok(json_data) => {
                    debug!("Loaded geometry database registry {}!", &rpath);
                    let info: json::Geometry = serde_json::from_str(&json_data).unwrap();
                    return Some(info);
                }
                Err(_) => return None,
            }
        }
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

#[derive(Debug, Clone)]
struct Material {
    pub name: String,
    pub emissive_factor: Vec4,
    pub base_color_factor: Vec4,
    pub textures: HashMap<TextureType, String>,
}

#[derive(Debug, Clone)]
struct Submesh {
    pub vertices: Vec<Vertex>,
    pub indices: Vec<Index>,
    pub material: Material,
}

type Index = u32;
#[derive(Debug, Clone)]
struct Mesh {
    pub name: String,
    pub sub_meshes: Vec<Submesh>,
}

#[derive(Debug, Clone)]
struct Model {
    pub meshes: Vec<Mesh>,
}

fn build_parent_map(gltf: &gltf::Document) -> HashMap<usize, usize> {
    let mut parent_map = HashMap::new();

    for scene in gltf.scenes() {
        for node in scene.nodes() {
            for child in node.children() {
                parent_map.insert(child.index(), node.index());
            }
        }
    }

    parent_map
}

fn compute_node_transform(node: &gltf::Node) -> Mat4 {
    let transform = node.transform();
    match transform {
        gltf::scene::Transform::Matrix { matrix } => {
            Mat4::from_cols_array_2d(&matrix) // Directly use provided matrix
        }
        gltf::scene::Transform::Decomposed {
            translation,
            rotation,
            scale,
        } => {
            let translation_matrix = Mat4::from_translation(Vec3::from(translation));
            let rotation_matrix = Mat4::from_quat(Quat::from_array(rotation));
            let scale_matrix = Mat4::from_scale(Vec3::from(scale));

            translation_matrix * rotation_matrix * scale_matrix
        }
    }
}

fn compute_global_transform(
    node: &gltf::Node,
    parent_map: &HashMap<usize, usize>,
    gltf: &gltf::Document,
) -> Mat4 {
    let mut transform = compute_node_transform(node);
    let mut current = node.index();

    // Walk up the hierarchy using the parent map
    while let Some(&parent_index) = parent_map.get(&current) {
        if let Some(parent_node) = gltf.nodes().nth(parent_index) {
            transform = compute_node_transform(&parent_node) * transform;
            current = parent_index;
        } else {
            break;
        }
    }

    transform
}

fn transform_vertex(position: [f32; 3], transform: &Mat4) -> [f32; 3] {
    let pos_vec = *transform * Vec4::new(position[0], position[1], position[2], 1.0);
    [pos_vec.x, pos_vec.y, pos_vec.z]
}

fn load_gltf_model(path: &str, db: &mut Database) -> Option<Model> {
    debug!("Loading Model {}", path);
    let (gltf, buffers, images) = gltf::import(path).expect("Failed to load glTF file");
    let mut meshes = Vec::new();
    let mut mesh_name = String::new();
    let parent_map = build_parent_map(&gltf);
    for node in gltf.nodes() {
        let global_transform = compute_node_transform(&node);
        if let Some(mesh) = node.mesh() {
            let mut submeshes = Vec::new();
            let global_transform = compute_global_transform(&node, &parent_map, &gltf);
            mesh_name = mesh.name().unwrap_or("[UNKNOWN]").to_string();
            debug!("Loading Mesh {}", mesh_name);
            for primitive in mesh.primitives() {
                let mut vertices = Vec::new();
                let mut indices = Vec::new();
                let mut material = None;

                let reader = primitive.reader(|buffer| Some(&buffers[buffer.index()]));

                let mut bone_ids: Vec<[u16; 4]> = Vec::new();
                let mut bone_weights: Vec<[f32; 4]> = Vec::new();

                // Extract Positions
                let positions: Vec<[f32; 3]> = reader
                    .read_positions()?
                    .map(|pos| transform_vertex(pos, &global_transform))
                    .collect();

                // Extract Normals
                let normals: Vec<[f32; 3]> = reader.read_normals()?.collect();

                // Extract Texture Coordinates
                let tex_coords: Vec<[f32; 2]> = reader.read_tex_coords(0)?.into_f32().collect();

                let colors: Vec<[f32; 4]> = if let Some(colors) = reader.read_colors(0) {
                    colors.into_rgba_f32().collect()
                } else {
                    let f = [0.0, 0.0, 0.0, 1.0];
                    vec![f; tex_coords.len()]
                };

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
                        color: (Vec4::from(colors[i])),
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

                    let mut process_texture_func = |tex: gltf::texture::Texture, name, kind| {
                        let tex_name = format!("{}.{}[{}]", mesh_name, name, submeshes.len());
                        match tex.source().source() {
                            gltf::image::Source::View { view, mime_type } => {
                                let buffer = &buffers[view.buffer().index()];
                                let start = view.offset();
                                let end = start + view.length();
                                let img_bytes = &buffer[start..end];

                                db.register_texture_from_bytes(&tex_name, &img_bytes);
                            }
                            gltf::image::Source::Uri { uri, mime_type } => {
                                if uri.starts_with("data:") {
                                    if let Some((_, base64_data)) = uri.split_once(";base64,") {
                                        let data = base64::decode(base64_data).unwrap();
                                        db.register_texture_from_bytes(&tex_name, &data);
                                    }
                                } else {
                                    tex.source().source();
                                    let path = format!("{}/{}", db.base_path(), uri);
                                    let data = std::fs::read(path).unwrap();
                                    db.register_texture_from_bytes(&tex_name, &data);
                                }
                            }
                        }

                        textures.insert(kind, tex_name);
                    };

                    if let Some(info) = mat.pbr_metallic_roughness().base_color_texture() {
                        process_texture_func(info.texture(), "BASE_COLOR", TextureType::Diffuse);
                    }

                    if let Some(info) = mat.emissive_texture() {
                        process_texture_func(info.texture(), "EMISSIVE", TextureType::Emissive);
                    }

                    if let Some(info) = mat.normal_texture() {
                        process_texture_func(info.texture(), "NORMAL", TextureType::Normal);
                    }

                    if let Some(info) = mat.occlusion_texture() {
                        process_texture_func(info.texture(), "OCCLUSION", TextureType::Occlusion);
                    }

                    debug!(
                        "REGISTERING MATERIAL NAME : {}.{} with texture len [{}]",
                        mesh_name,
                        mat_name,
                        textures.len()
                    );
                    material = Some(Material {
                        name: mat_name,
                        textures,
                        emissive_factor: Vec4::from((Vec3::from_array(mat.emissive_factor()), 1.0)),
                        base_color_factor: Vec4::from_array(
                            mat.pbr_metallic_roughness().base_color_factor(),
                        ),
                    });

                    submeshes.push(Submesh {
                        vertices,
                        indices,
                        material: material.unwrap(),
                    });
                }
            }

            meshes.push(Mesh {
                name: mesh.name().unwrap_or("None").to_string(),
                sub_meshes: submeshes,
            });
        }
    }

    Some(Model { meshes })
}
