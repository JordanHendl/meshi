#pragma once

#include <cstdint>

struct MeshiHandle {
    std::uint32_t index;
    std::uint32_t generation;
};

struct MeshiVec2 {
    float x, y;
};

struct alignas(16) MeshiVec3 {
    float x, y, z;
};

struct alignas(16) MeshiVec4 {
    float x, y, z, w;
};

struct alignas(16) MeshiQuat {
    float x, y, z, w;
};

struct MeshiMat4 {
    float m[4][4];
};

struct MeshiEngine;
struct MeshiRenderEngine;
struct MeshiPhysicsSimulation;
struct MeshiMeshObject;
struct MeshiDirectionalLight;
struct MeshiMaterial;
struct MeshiRigidBody;

struct MeshiEngineInfo {
    const char* application_name;
    const char* application_location;
    std::int32_t headless;
};

struct MeshiFFIMeshObjectInfo {
    const char* mesh;
    const char* material;
    MeshiMat4 transform;
};

struct MeshiDirectionalLightInfo {
    MeshiVec4 direction;
    MeshiVec4 color;
    float intensity;
};

enum class MeshiEventType : std::uint32_t {
    Unknown = 0,
    Quit = 1,
    Pressed = 2,
    Released = 3,
    Joystick = 4,
    Motion2D = 5,
};

enum class MeshiEventSource : std::uint32_t {
    Unknown = 0,
    Key = 1,
    Mouse = 2,
    MouseButton = 3,
    Gamepad = 4,
    Window = 5,
};

enum class MeshiKeyCode : std::uint32_t {
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

    Shift,
    Control,
    Alt,
    Meta,

    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,

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

    Minus,
    Equals,
    LeftBracket,
    RightBracket,
    Backslash,
    Semicolon,
    Apostrophe,
    Comma,
    Period,
    Slash,
    GraveAccent,

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
    NumpadAdd,
    NumpadSubtract,
    NumpadMultiply,
    NumpadDivide,
    NumpadDecimal,
    NumpadEnter,

    CapsLock,
    NumLock,
    ScrollLock,

    PrintScreen,
    Pause,
    Menu,

    Undefined,
};

enum class MeshiMouseButton : std::uint32_t {
    Left,
    Right,
};

struct MeshiPressPayload {
    MeshiKeyCode key;
    MeshiEventType previous;
};

struct MeshiMotion2DPayload {
    MeshiVec2 motion;
};

struct MeshiMouseButtonPayload {
    MeshiMouseButton button;
    MeshiVec2 pos;
};

union MeshiPayload {
    MeshiPressPayload press;
    MeshiMotion2DPayload motion2d;
    MeshiMouseButtonPayload mouse_button;
};

struct MeshiEvent {
    MeshiEventType event_type;
    MeshiEventSource source;
    MeshiPayload payload;
    std::uint32_t timestamp;
};

struct MeshiMaterialInfo {
    float dynamic_friction_m;
};

struct MeshiForceApplyInfo {
    MeshiVec3 amt;
};

enum class MeshiCollisionShapeType : std::uint32_t {
    Sphere = 0,
    Box = 1,
};

struct alignas(16) MeshiCollisionShape {
    MeshiVec3 dimensions;
    float radius;
    MeshiCollisionShapeType shape_type;
};

struct MeshiRigidBodyInfo {
    MeshiHandle material;
    MeshiVec3 initial_position;
    MeshiVec3 initial_velocity;
    MeshiQuat initial_rotation;
    std::uint32_t has_gravity;
    MeshiCollisionShape collision_shape;
};

struct MeshiActorStatus {
    MeshiVec3 position;
    MeshiQuat rotation;
};

struct MeshiContactInfo {
    MeshiHandle a;
    MeshiHandle b;
    MeshiVec3 normal;
    float penetration;
};

using MeshiMeshObjectHandle = MeshiHandle;
using MeshiDirectionalLightHandle = MeshiHandle;
using MeshiMaterialHandle = MeshiHandle;
using MeshiRigidBodyHandle = MeshiHandle;

