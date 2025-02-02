use std::os::raw::c_uint;

#[allow(dead_code)]
#[repr(C)]
#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum EventType {
    Unknown = 0,
    Quit = 1,
    Pressed = 2,
    Released = 3,
    Joystick = 4,
    Motion2D = 5,
}

#[allow(dead_code)]
#[repr(C)]
#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum EventSource {
    Unknown = 0,
    Key = 1,
    Mouse = 2,
    Gamepad = 3,
}

#[allow(dead_code)]
#[repr(C)]
#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum KeyCode {
    // Alphanumeric keys
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // Number keys (top row)
    Digit0,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    Digit5,
    Digit6,
    Digit7,
    Digit8,
    Digit9,

    // Function keys
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    // Modifier keys
    Shift,
    Control,
    Alt,
    Meta, // Windows key or Command key (Mac)

    // Navigation keys
    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,

    // Special keys
    Escape,
    Enter,
    Space,
    Tab,
    Backspace,
    Delete,
    Insert,
    Home,
    End,
    PageUp,
    PageDown,

    // Punctuation and symbols
    Minus,        // -
    Equals,       // =
    LeftBracket,  // [
    RightBracket, // ]
    Backslash,    // \
    Semicolon,    // ;
    Apostrophe,   // '
    Comma,        // ,
    Period,       // .
    Slash,        // /
    GraveAccent,  // `

    // Numpad keys
    Numpad0,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,
    NumpadAdd,      // +
    NumpadSubtract, // -
    NumpadMultiply, // *
    NumpadDivide,   // /
    NumpadDecimal,  // .
    NumpadEnter,

    // Lock keys
    CapsLock,
    NumLock,
    ScrollLock,

    // Miscellaneous keys
    PrintScreen,
    Pause,
    Menu,

    // Undefined or custom keys
    Undefined,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Vec2 {
    x: f32,
    y: f32,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct PressPayload {
    key: KeyCode,
    previous: EventType,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Motion2DPayload {
    motion: Vec2,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub union Payload {
    press: PressPayload,
    motion2d: Motion2DPayload,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct Event {
    event_type: EventType,
    source: EventSource,
    payload: Payload,
    timestamp: c_uint,
}

use sdl2::event::Event as SdlEvent;
use sdl2::keyboard::Keycode as SdlKeycode;

impl From<SdlEvent> for Event {
    fn from(sdl_event: SdlEvent) -> Self {
        match sdl_event {
            SdlEvent::Quit { timestamp } => Event {
                event_type: EventType::Quit,
                source: EventSource::Unknown, 
                payload: Payload {
                    press: PressPayload {
                        key: KeyCode::Undefined,
                        previous: EventType::Unknown,
                    },
                },
                timestamp,
            },

            SdlEvent::KeyDown {
                timestamp,
                keycode: Some(sdl_keycode),
                ..
            } => Event {
                event_type: EventType::Pressed,
                source: EventSource::Key,
                payload: Payload {
                    press: PressPayload {
                        key: map_sdl_keycode(sdl_keycode),
                        previous: EventType::Unknown,
                    },
                },
                timestamp,
            },

            SdlEvent::KeyUp {
                timestamp,
                keycode: Some(sdl_keycode),
                ..
            } => Event {
                event_type: EventType::Released,
                source: EventSource::Key,
                payload: Payload {
                    press: PressPayload {
                        key: map_sdl_keycode(sdl_keycode),
                        previous: EventType::Unknown,
                    },
                },
                timestamp,
            },

            SdlEvent::MouseMotion {
                timestamp,
                xrel,
                yrel,
                ..
            } => Event {
                event_type: EventType::Motion2D,
                source: EventSource::Mouse,
                payload: Payload {
                    motion2d: Motion2DPayload {
                        motion: Vec2 {
                            x: xrel as f32,
                            y: yrel as f32,
                        },
                    },
                },
                timestamp,
            },

            _ => Event {
                event_type: EventType::Unknown,
                source: EventSource::Key,
                payload: Payload {
                    press: PressPayload {
                        key: KeyCode::Undefined,
                        previous: EventType::Unknown,
                    },
                },
                timestamp: 0,
            },
        }
    }
}

/// Helper function to map `sdl2::keyboard::Keycode` to `KeyCode`
fn map_sdl_keycode(sdl_keycode: SdlKeycode) -> KeyCode {
    match sdl_keycode {
        SdlKeycode::A => KeyCode::A,
        SdlKeycode::B => KeyCode::B,
        SdlKeycode::C => KeyCode::C,
        SdlKeycode::D => KeyCode::D,
        SdlKeycode::E => KeyCode::E,
        SdlKeycode::F => KeyCode::F,
        SdlKeycode::G => KeyCode::G,
        SdlKeycode::H => KeyCode::H,
        SdlKeycode::I => KeyCode::I,
        SdlKeycode::J => KeyCode::J,
        SdlKeycode::K => KeyCode::K,
        SdlKeycode::L => KeyCode::L,
        SdlKeycode::M => KeyCode::M,
        SdlKeycode::N => KeyCode::N,
        SdlKeycode::O => KeyCode::O,
        SdlKeycode::P => KeyCode::P,
        SdlKeycode::Q => KeyCode::Q,
        SdlKeycode::R => KeyCode::R,
        SdlKeycode::S => KeyCode::S,
        SdlKeycode::T => KeyCode::T,
        SdlKeycode::U => KeyCode::U,
        SdlKeycode::V => KeyCode::V,
        SdlKeycode::W => KeyCode::W,
        SdlKeycode::X => KeyCode::X,
        SdlKeycode::Y => KeyCode::Y,
        SdlKeycode::Z => KeyCode::Z,
        SdlKeycode::Num0 => KeyCode::Digit0,
        SdlKeycode::Num1 => KeyCode::Digit1,
        SdlKeycode::Num2 => KeyCode::Digit2,
        SdlKeycode::Num3 => KeyCode::Digit3,
        SdlKeycode::Num4 => KeyCode::Digit4,
        SdlKeycode::Num5 => KeyCode::Digit5,
        SdlKeycode::Num6 => KeyCode::Digit6,
        SdlKeycode::Num7 => KeyCode::Digit7,
        SdlKeycode::Num8 => KeyCode::Digit8,
        SdlKeycode::Num9 => KeyCode::Digit9,
        SdlKeycode::F1 => KeyCode::F1,
        SdlKeycode::F2 => KeyCode::F2,
        SdlKeycode::F3 => KeyCode::F3,
        SdlKeycode::F4 => KeyCode::F4,
        SdlKeycode::F5 => KeyCode::F5,
        SdlKeycode::F6 => KeyCode::F6,
        SdlKeycode::F7 => KeyCode::F7,
        SdlKeycode::F8 => KeyCode::F8,
        SdlKeycode::F9 => KeyCode::F9,
        SdlKeycode::F10 => KeyCode::F10,
        SdlKeycode::F11 => KeyCode::F11,
        SdlKeycode::F12 => KeyCode::F12,
        SdlKeycode::Left => KeyCode::ArrowLeft,
        SdlKeycode::Right => KeyCode::ArrowRight,
        SdlKeycode::Up => KeyCode::ArrowUp,
        SdlKeycode::Down => KeyCode::ArrowDown,
        SdlKeycode::Escape => KeyCode::Escape,
        SdlKeycode::Return => KeyCode::Enter,
        SdlKeycode::Space => KeyCode::Space,
        SdlKeycode::Tab => KeyCode::Tab,
        SdlKeycode::Backspace => KeyCode::Backspace,
        SdlKeycode::Delete => KeyCode::Delete,
        SdlKeycode::Insert => KeyCode::Insert,
        SdlKeycode::Home => KeyCode::Home,
        SdlKeycode::End => KeyCode::End,
        SdlKeycode::PageUp => KeyCode::PageUp,
        SdlKeycode::PageDown => KeyCode::PageDown,
        _ => KeyCode::Undefined,
    }
}

