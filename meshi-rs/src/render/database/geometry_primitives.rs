use super::ModelResource;
use crate::render::database::MeshResource;
use dashi::*;
use glam::*;
use miso::MeshInfo;
use tracing::{info, Level};

pub struct CubePrimitiveInfo {
    size: f32,
}

impl Default for CubePrimitiveInfo {
    fn default() -> Self {
        Self { size: 1.0 }
    }
}

pub fn make_cube(
    info: &CubePrimitiveInfo,
    ctx: &mut dashi::Context,
    scene: &mut miso::MisoScene,
) -> ModelResource {
    let size = info.size;

    // Cube vertices with corrected texture coordinates
    let cvertices: [miso::Vertex; 8] = [
        // Front face
        miso::Vertex {
            position: Vec4::new(-size, -size, size, 1.0),
            normal: Vec4::new(0.0, 0.0, 1.0, 0.0),
            tex_coords: Vec2::new(0.0, 1.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        miso::Vertex {
            position: Vec4::new(size, -size, size, 1.0),
            normal: Vec4::new(0.0, 0.0, 1.0, 0.0),
            tex_coords: Vec2::new(1.0, 1.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        miso::Vertex {
            position: Vec4::new(size, size, size, 1.0),
            normal: Vec4::new(0.0, 0.0, 1.0, 0.0),
            tex_coords: Vec2::new(1.0, 0.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        miso::Vertex {
            position: Vec4::new(-size, size, size, 1.0),
            normal: Vec4::new(0.0, 0.0, 1.0, 0.0),
            tex_coords: Vec2::new(0.0, 0.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        // Back face
        miso::Vertex {
            position: Vec4::new(-size, -size, -size, 1.0),
            normal: Vec4::new(0.0, 0.0, -1.0, 0.0),
            tex_coords: Vec2::new(1.0, 1.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        miso::Vertex {
            position: Vec4::new(size, -size, -size, 1.0),
            normal: Vec4::new(0.0, 0.0, -1.0, 0.0),
            tex_coords: Vec2::new(0.0, 1.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        miso::Vertex {
            position: Vec4::new(size, size, -size, 1.0),
            normal: Vec4::new(0.0, 0.0, -1.0, 0.0),
            tex_coords: Vec2::new(0.0, 0.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        miso::Vertex {
            position: Vec4::new(-size, size, -size, 1.0),
            normal: Vec4::new(0.0, 0.0, -1.0, 0.0),
            tex_coords: Vec2::new(1.0, 0.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
    ];

    // Cube indices
    const INDICES: [u32; 36] = [
        // Front face
        0, 1, 2, 2, 3, 0, // Back face
        4, 5, 6, 6, 7, 4, // Left face
        4, 0, 3, 3, 7, 4, // Right face
        1, 5, 6, 6, 2, 1, // Top face
        3, 2, 6, 6, 7, 3, // Bottom face
        4, 5, 1, 1, 0, 4,
    ];

    let vertices = ctx
        .make_buffer(&BufferInfo {
            debug_name: &format!("Cube Vertices"),
            byte_size: (std::mem::size_of::<miso::Vertex>() * cvertices.len()) as u32,
            visibility: dashi::MemoryVisibility::Gpu,
            usage: dashi::BufferUsage::VERTEX,
            initial_data: Some(unsafe { cvertices.as_slice().align_to::<u8>().1 }),
        })
        .unwrap();

    let indices = ctx
        .make_buffer(&BufferInfo {
            debug_name: &format!("Cube Indices"),
            byte_size: (std::mem::size_of::<u32>() * INDICES.len()) as u32,
            visibility: dashi::MemoryVisibility::Gpu,
            usage: dashi::BufferUsage::INDEX,
            initial_data: Some(unsafe { INDICES.as_slice().align_to::<u8>().1 }),
        })
        .unwrap();

    info!("Registering Default Cube Mesh..");
    let m = scene.register_mesh(&MeshInfo {
        name: "Cube".to_string(),
        vertices,
        num_vertices: cvertices.len(),
        indices,
        num_indices: INDICES.len(),
    });

    return ModelResource {
        meshes: vec![MeshResource {
            name: "CUBE".to_string(),
            m,
            mat: Default::default(),
        }],
    };
}

pub struct TrianglePrimitiveInfo {
    size: f32,
}

impl Default for TrianglePrimitiveInfo {
    fn default() -> Self {
        Self { size: 1.0 }
    }
}

pub fn make_triangle(
    info: &TrianglePrimitiveInfo,
    ctx: &mut dashi::Context,
    scene: &mut miso::MisoScene,
) -> ModelResource {
    let size = info.size;

    // Triangle vertices
    let tvertices: [miso::Vertex; 3] = [
        miso::Vertex {
            position: Vec4::new(0.0, size, 0.0, 1.0), // Top vertex
            normal: Vec4::new(0.0, 0.0, 1.0, 0.0),
            tex_coords: Vec2::new(0.5, 1.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        miso::Vertex {
            position: Vec4::new(-size, -size, 0.0, 1.0), // Bottom-left vertex
            normal: Vec4::new(0.0, 0.0, 1.0, 0.0),
            tex_coords: Vec2::new(0.0, 0.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
        miso::Vertex {
            position: Vec4::new(size, -size, 0.0, 1.0), // Bottom-right vertex
            normal: Vec4::new(0.0, 0.0, 1.0, 0.0),
            tex_coords: Vec2::new(1.0, 0.0),
            joint_ids: IVec4::new(0, 0, 0, 0),
            joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
            color: Default::default(),
        },
    ];

    // Triangle indices
    const INDICES: [u32; 3] = [
        0, 1, 2, // One face
    ];

    let vertices = ctx
        .make_buffer(&BufferInfo {
            debug_name: &format!("Triangle Vertices"),
            byte_size: (std::mem::size_of::<miso::Vertex>() * tvertices.len()) as u32,
            visibility: dashi::MemoryVisibility::Gpu,
            usage: dashi::BufferUsage::VERTEX,
            initial_data: Some(unsafe { tvertices.as_slice().align_to::<u8>().1 }),
        })
        .unwrap();

    let indices = ctx
        .make_buffer(&BufferInfo {
            debug_name: &format!("Triangle Indices"),
            byte_size: (std::mem::size_of::<u32>() * INDICES.len()) as u32,
            visibility: dashi::MemoryVisibility::Gpu,
            usage: dashi::BufferUsage::INDEX,
            initial_data: Some(unsafe { INDICES.as_slice().align_to::<u8>().1 }),
        })
        .unwrap();

    info!("Registering Default Triangle Mesh..");
    let m = scene.register_mesh(&MeshInfo {
        name: "Triangle".to_string(),
        vertices,
        num_vertices: tvertices.len(),
        indices,
        num_indices: INDICES.len(),
    });

    return ModelResource {
        meshes: vec![MeshResource {
            name: "TRIANGLE".to_string(),
            m,
            mat: Default::default(),
        }],
    };
}

pub struct SpherePrimitiveInfo {
    pub radius: f32,
    pub segments: u32,
    pub rings: u32,
}

impl Default for SpherePrimitiveInfo {
    fn default() -> Self {
        Self {
            radius: 1.0,
            segments: 32,
            rings: 16,
        }
    }
}

pub fn make_sphere(
    info: &SpherePrimitiveInfo,
    ctx: &mut dashi::Context,
    scene: &mut miso::MisoScene,
) -> ModelResource {
    let SpherePrimitiveInfo {
        radius,
        segments,
        rings,
    } = *info;

    let mut vertices = Vec::new();
    let mut indices = Vec::new();

    for ring in 0..=rings {
        let theta = (ring as f32) * std::f32::consts::PI / (rings as f32);
        let y = radius * theta.cos();
        let ring_radius = radius * theta.sin();

        for segment in 0..=segments {
            let phi = (segment as f32) * 2.0 * std::f32::consts::PI / (segments as f32);
            let x = ring_radius * phi.cos();
            let z = ring_radius * phi.sin();

            vertices.push(miso::Vertex {
                position: Vec4::new(x, y, z, 1.0),
                normal: Vec4::new(x / radius, y / radius, z / radius, 0.0),
                tex_coords: Vec2::new(segment as f32 / segments as f32, ring as f32 / rings as f32),
                joint_ids: IVec4::new(0, 0, 0, 0),
                joints: Vec4::new(0.0, 0.0, 0.0, 0.0),
                color: Default::default(),
            });

            if ring < rings && segment < segments {
                let current = ring * (segments + 1) + segment;
                let next = current + segments + 1;

                indices.push(current);
                indices.push(next);
                indices.push(current + 1);

                indices.push(current + 1);
                indices.push(next);
                indices.push(next + 1);
            }
        }
    }

    let vertex_buffer = ctx
        .make_buffer(&BufferInfo {
            debug_name: &"Sphere Vertices".to_string(),
            byte_size: (std::mem::size_of::<miso::Vertex>() * vertices.len()) as u32,
            visibility: dashi::MemoryVisibility::Gpu,
            usage: dashi::BufferUsage::VERTEX,
            initial_data: Some(unsafe { vertices.as_slice().align_to::<u8>().1 }),
        })
        .unwrap();

    let index_buffer = ctx
        .make_buffer(&BufferInfo {
            debug_name: &"Sphere Indices".to_string(),
            byte_size: (std::mem::size_of::<u32>() * indices.len()) as u32,
            visibility: dashi::MemoryVisibility::Gpu,
            usage: dashi::BufferUsage::INDEX,
            initial_data: Some(unsafe { indices.as_slice().align_to::<u8>().1 }),
        })
        .unwrap();

    info!("Registering Default Sphere Mesh..");
    let mesh = scene.register_mesh(&MeshInfo {
        name: "Sphere".to_string(),
        vertices: vertex_buffer,
        num_vertices: vertices.len(),
        indices: index_buffer,
        num_indices: indices.len(),
    });

    ModelResource {
        meshes: vec![MeshResource {
            name: "SPHERE".to_string(),
            m: mesh,
            mat: Default::default(),
        }],
    }
}
