use serde::{Deserialize, Serialize};

#[derive(Deserialize, Serialize, Clone, Default)]
pub struct ImageEntry {
    pub name: String,
    pub path: String,
}

#[derive(Deserialize, Serialize, Clone, Default)]
pub struct Image {
    pub images: Vec<ImageEntry>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct MaterialEntry {
   pub base_color: Option<String>,
   pub normal: Option<String>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct Materials {
    pub materials: Vec<MaterialEntry>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct GeometryEntry {
    pub name: String,
    pub path: String,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct Geometry {
    pub geometry: Vec<GeometryEntry>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct TTFEntry {
    pub name: String,
    pub path: String,
    pub size: f64,
    pub glyphs: Option<String>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct TTF {
    pub fonts: Vec<TTFEntry>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct Database {
    pub images: Option<String>,
    pub materials: Option<String>,
    pub geometry: Option<String>,
    pub ttf: Option<String>,
}
