use serde::{Deserialize, Serialize};

#[derive(Deserialize, Serialize, Clone)]
pub struct SpriteJSONEntry {
    pub name: String,
    pub image_path: String,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct SpriteJSON {
    pub sprites: Vec<SpriteJSONEntry>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct SpriteSheetJSONSprite {
    pub name: String,
    pub id: u32,
    pub bounds: dashi::Rect2D,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct SpriteSheetJSONAutoGen {
    pub name: String,
    pub bounds: dashi::Rect2D,
    pub stride: u32,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct SpriteSheetJSONEntry {
    pub name: String,
    pub image_path: String,
    pub sprites: Option<Vec<SpriteSheetJSONSprite>>,
    pub auto_gen: Option<SpriteSheetJSONAutoGen>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct SpriteSheetJSON {
    pub sprite_sheets: Vec<SpriteSheetJSONEntry>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct TTFJSONEntry {
    pub name: String,
    pub path: String,
    pub size: f64,
    pub glyphs: Option<String>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct TTFJSON {
    pub fonts: Vec<TTFJSONEntry>,
}

#[derive(Deserialize, Serialize, Clone)]
pub struct DatabaseJSON {
    pub sprite_cfg: Option<String>,
    pub sprite_sheet_cfg: Option<String>,
    pub ttf_cfg: Option<String>,
    pub particle_cfg: Option<String>,
}
