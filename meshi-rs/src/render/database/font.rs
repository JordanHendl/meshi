use dashi::Rect2D;
use std::collections::HashMap;

#[derive(Debug)]
pub struct Glyph {
    pub bounds: Rect2D, // where this glyph is in the atlas
    pub advance: f32,
    pub bearing_x: f32,
    pub bearing_y: f32,
}

pub struct TTFont {
    pub glyphs: HashMap<char, Glyph>,
    pub atlas: Option<Vec<u8>>, // Cache for the entire glyph atlas as a single image
    pub atlas_width: u32,
    pub atlas_height: u32,
}

impl TTFont {
    /// Creates a new TTFont instance by loading a font from a specified file path.
    pub fn new(file_path: &str, width: u32, height: u32, font_size: f32, range: &[char]) -> Self {
        // Create a new bitmap (initialized to zero)
        let mut bitmap = vec![0u8; (width * height) as usize];

        let font_data = std::fs::read(file_path).unwrap();
        let font = fontdue::Font::from_bytes(
            font_data,
            fontdue::FontSettings {
                scale: 80.0,
                ..Default::default()
            },
        )
        .unwrap();

        // Coordinates to keep track of where to draw the next glyph
        let mut cursor_x: u32 = font_size as u32;
        let mut cursor_y: u32 = font_size as u32;
        let mut glyph_map = HashMap::new();
        let mut max_height_in_line: f32 = 0.0; // Track the tallest glyph in the line

        for ch in range {
            let (metrics, bitmap_data) =
                font.rasterize_indexed(font.lookup_glyph_index(*ch), font_size);
            {
                // Update the max height for the current line
                max_height_in_line =
                    max_height_in_line.max(metrics.height as f32 - metrics.ymin as f32);

                // Draw the glyph bitmap at the calculated position
                for (i, &pixel) in bitmap_data.iter().enumerate() {
                    let px = i % metrics.width;
                    let py = i / metrics.width;
                    let x = (cursor_x as usize) + px;
                    let y = (cursor_y  as i32) as usize + py;

                    bitmap[y * width as usize + x] = pixel;
                }

                // Store the glyph boundary
                glyph_map.insert(
                    *ch,
                    Glyph {
                        bounds: Rect2D {
                            x: cursor_x as u32,
                            y: cursor_y as u32,
                            w: metrics.width as u32,
                            h: metrics.height as u32,
                        },
                        advance: metrics.advance_width / (width as f32),
                        bearing_x: metrics.xmin as f32 / width as f32,
                        bearing_y: metrics.ymin as f32 / height as f32,
                    },
                );
                // Move the cursor forward by the advance width
                cursor_x += (metrics.advance_width + metrics.xmin as f32) as u32;

                // If the cursor_x exceeds the bitmap width, move to the next line
                if cursor_x >= width {
                    cursor_x = font_size as u32;
                    cursor_y += max_height_in_line as u32 + font_size as u32; // Move down by the tallest glyph in the line
                    max_height_in_line = 0.0; // Reset for the next line
                }
            }
        }

        Self {
            glyphs: glyph_map,
            atlas: Some(bitmap),
            atlas_width: width,
            atlas_height: height,
        }
    }
}
