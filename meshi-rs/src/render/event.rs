use std::os::raw::{c_char, c_int};

#[repr(C)]
#[derive(Clone, Copy)]
pub enum EventType {
    Quit,
    KeyDown,
    KeyUp,
    MouseButtonDown,
    MouseButtonUp,
    MouseMotion,
    MouseWheel,
    Unknown,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct KeyEvent {
    pub scancode: c_int,
    pub keycode: c_int,
    pub state: c_int, // 0 = released, 1 = pressed
    pub repeat: c_int,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct MouseButtonEvent {
    pub button: c_int,
    pub state: c_int, // 0 = released, 1 = pressed
    pub clicks: c_int,
    pub x: c_int,
    pub y: c_int,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct MouseMotionEvent {
    pub x: c_int,
    pub y: c_int,
    pub xrel: c_int,
    pub yrel: c_int,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct MouseWheelEvent {
    pub x: c_int,
    pub y: c_int,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub union EventData {
    pub key: KeyEvent,
    pub mouse_button: MouseButtonEvent,
    pub mouse_motion: MouseMotionEvent,
    pub mouse_wheel: MouseWheelEvent,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct Event {
    pub event_type: EventType,
    pub timestamp: u32,
    pub data: EventData,
}

impl Default for Event {
    fn default() -> Self {
        Event {
            event_type: EventType::Unknown,
            timestamp: 0,
            data: EventData {
                key: KeyEvent {
                    scancode: 0,
                    keycode: 0,
                    state: 0,
                    repeat: 0,
                },
            },
        }
    }
}

impl Event {
    pub fn from_sdl2_event(event: sdl2::event::Event) -> Self {
        match event {
            sdl2::event::Event::Quit { timestamp } => Event {
                event_type: EventType::Quit,
                timestamp,
                ..Default::default()
            },
            sdl2::event::Event::KeyDown {
                timestamp,
                scancode,
                keycode,
                repeat,
                ..
            } => Event {
                event_type: EventType::KeyDown,
                timestamp,
                data: EventData {
                    key: KeyEvent {
                        scancode: scancode.map_or(0, |s| s as c_int),
                        keycode: keycode.map_or(0, |k| k.into_i32() as c_int),
                        state: 1,
                        repeat: repeat as c_int,
                    },
                },
            },
            sdl2::event::Event::KeyUp { timestamp, scancode, keycode, .. } => Event {
                event_type: EventType::KeyUp,
                timestamp,
                data: EventData {
                    key: KeyEvent {
                        scancode: scancode.map_or(0, |s| s as c_int),
                        keycode: keycode.map_or(0, |k| k.into_i32() as c_int),
                        state: 0,
                        repeat: 0,
                    },
                },
            },
            sdl2::event::Event::MouseButtonDown { timestamp, mouse_btn, clicks, x, y, window_id, which } => Event {
                event_type: EventType::MouseButtonDown,
                timestamp,
                data: EventData {
                    mouse_button: MouseButtonEvent {
                        button: mouse_btn as c_int,
                        state: 1,
                        clicks: clicks as c_int,
                        x,
                        y,
                    },
                },
            },
            sdl2::event::Event::MouseButtonUp { timestamp, mouse_btn, clicks, x, y, window_id, which } => Event {
                event_type: EventType::MouseButtonUp,
                timestamp,
                data: EventData {
                    mouse_button: MouseButtonEvent {
                        button: mouse_btn as c_int,
                        state: 0,
                        clicks: clicks as c_int,
                        x,
                        y,
                    },
                },
            },
            sdl2::event::Event::MouseMotion { timestamp, x, y, xrel, yrel, .. } => Event {
                event_type: EventType::MouseMotion,
                timestamp,
                data: EventData {
                    mouse_motion: MouseMotionEvent {
                        x,
                        y,
                        xrel,
                        yrel,
                    },
                },
            },
            sdl2::event::Event::MouseWheel { timestamp, x, y, .. } => Event {
                event_type: EventType::MouseWheel,
                timestamp,
                data: EventData {
                    mouse_wheel: MouseWheelEvent { x, y },
                },
            },
            _ => Event::default(),
        }
    }
}

