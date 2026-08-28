#pragma once

// GLFW includes
#include <GLFW/glfw3.h>

// GLM includes
#include "glm/glm.hpp"

// Engine includes
#include "../engine/utilities/primitive_types.hpp"

// Forward declarations
class Device;

// Compile time variables
constexpr i32 NUMBER_OF_KEYS = 350;
constexpr i32 NUMBER_OF_MOUSE_BUTTONS = 8;
constexpr f32 MOUSE_WHEEL_MINIMUM = -100.0f;
constexpr f32 MOUSE_WHEEL_START = 0.0f;
constexpr f32 MOUSE_WHEEL_MAXIMUM = 100.0f;

class Input
{
public:
    enum class KeyboardKey
    {
        None = -1,
        Space = 32,
        Apostrophe = 39,
        Comma = 44,
        Minus = 45,
        Period = 46,
        Slash = 47,
        Digit0 = 48,
        Digit1 = 49,
        Digit2 = 50,
        Digit3 = 51,
        Digit4 = 52,
        Digit5 = 53,
        Digit6 = 54,
        Digit7 = 55,
        Digit8 = 56,
        Digit9 = 57,
        Semicolon = 59,
        Equal = 61,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,
        Backslash = 92,
        RightBracket = 93,
        GraveAccent = 96,
        World1 = 161,
        World2 = 162,
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        ArrowRight = 262,
        ArrowLeft = 263,
        ArrowDown = 264,
        ArrowUp = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        Numpad0 = 320,
        Numpad1 = 321,
        Numpad2 = 322,
        Numpad3 = 323,
        Numpad4 = 324,
        Numpad5 = 325,
        Numpad6 = 326,
        Numpad7 = 327,
        Numpad8 = 328,
        Numpad9 = 329,
        NumpadDecimal = 330,
        NumpadDivide = 331,
        NumpadMultiply = 332,
        NumpadSubtract = 333,
        NumpadAdd = 334,
        NumpadEnter = 335,
        NumpadEqual = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
    };

    enum class MouseButton
    {
        Left = 0,
        Right = 1,
        Middle = 2
    };

    //enum class GamepadAxis
    //{
    //    // Represents the 2D axis of the gamepad sticks. With analog input values between -1.0 and +1.0, 0.0 when not pressed. 
    //    StickLeftX = 0,
    //    StickLeftY = 1,
    //    StickRightX = 2,
    //    StickRightY = 3,
    //    // Represents the triggers of the gamepad. With analog input values between 0.0 and 1.0, 0.0 when not pressed. 
    //    TriggerLeft = 4,
    //    TriggerRight = 5
    //};

    //enum class GamepadButton
    //{
    //    // Represents the four colorful buttons on the gamepad.
    //    South = 0,
    //    East = 1,
    //    West = 2,
    //    North = 3,
    //    // Represents the shoulder buttons (above the triggers) on a gamepad.
    //    ShoulderLeft = 4,
    //    ShoulderRight = 5,
    //    // Represents the left of the two menu related buttons on a gamepad.
    //    // This button has different names on different platforms, such as Share (Xbox) or - (Switch). The PS5 does not have such a button.
    //    MenuLeft = 6,
    //    // Represents the right of the two menu related buttons on a gamepad.
    //    // This button has different names on different platforms, such as Menu (Xbox), Start (PS5), or + (Switch).
    //    MenuRight = 7,
    //    // Button 8 is not used, so that we have a 1-on-1 mapping with the GLFW enum.
    //    // Represents the gamepad stick buttons. 
    //    StickLeftPress = 9,
    //    StickRightPress = 10,
    //    // Represent the four arrow buttons on the gamepad.
    //    DPadUp = 11,
    //    DPadRight = 12,
    //    DPadDown = 13,
    //    DPadLeft = 14
    //};

    enum class InputAction
    {
        Release = 0, // Ugly, but it has to be this way.
        Press = 1,
        None = 2
    };

    struct InputData
    {
        bool KeysDown[NUMBER_OF_KEYS] = { false };
        bool KeysDownPrevious[NUMBER_OF_KEYS] = { false };
        InputAction KeysAction[NUMBER_OF_KEYS] = { InputAction::None };

        bool MouseButtonsDown[NUMBER_OF_MOUSE_BUTTONS] = { false };
        bool MouseButtonsDownPrevious[NUMBER_OF_MOUSE_BUTTONS] = { false };
        InputAction MouseButtonsAction[NUMBER_OF_MOUSE_BUTTONS] = { InputAction::None };

        glm::vec2 RawMousePosition = {};
        glm::vec2 MousePositionDelta = {};
        glm::vec2 MousePosition = {};

        f32 MouseWheel = MOUSE_WHEEL_START;

        //bool GamepadConnected = false;
        //GLFWgamepadstate GamepadState = {};
        //GLFWgamepadstate GamepadStatePrevious = {};
    };

    bool GetKeyboardKey(KeyboardKey key) const;     // Returns whether a key is being pressed in the current frame.
    bool GetKeyboardKeyDown(KeyboardKey key) const; // Returns whether a key is being pressed in the current frame without having been pressed in the previous frame.
    bool GetKeyboardKeyUp(KeyboardKey key) const;   // Returns whether a key is no longer pressed in the current frame while in the previous frame it was.

    bool GetMouseButton(MouseButton button) const;     // Returns whether a mouse button is being pressed in the current frame.
    bool GetMouseButtonDown(MouseButton button) const; // Returns whether a mouse button is being pressed in the current frame without having been pressed in the previous frame.
    bool GetMouseButtonUp(MouseButton button) const;   // Returns whether a mouse button is no longer pressed in the current frame while in the previous frame it was.
    f32 GetMouseWheel() const;                         // Gets the mouse wheel, relative to the initial value when starting the game.
    f32 GetMouseWheelMax() const;                      // Gets the maximum value the mouse wheel can return.
    f32 GetMouseWheelMin() const;                      // Gets the minimum value the mouse wheel can return.
    glm::vec2 GetRawMousePosition() const;             // Returns screen position of the mouse, relative to the top-left corner of the screen.

    //f32 GetGamepadAxis(GamepadAxis axis) const;         // Returns the current floating-point value of a given gamepad axis.
    //f32 GetGamepadAxisPrevious(GamepadAxis axis) const; // Returns the previous floating-point value of a given gamepad axis. Use this together with GetGamepadAxis() to check if a value has crossed a certain threshold.

    //bool GetGamepadButton(GamepadButton button) const;     // Returns whether a given gamepad button is being held down in the current frame.
    //bool GetGamepadButtonDown(GamepadButton button) const; // Returns whether a given gamepad button is being pressed in the current frame without having been pressed in the previous frame.
    //bool GetGamepadButtonUp(GamepadButton button) const;   // Returns whether a given gamepad button is no longer pressed in the current frame while in the previous frame it was.

private:
    Input(Device* device);
    ~Input();

    static void KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
    static void MouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods);
    static void MouseWheelCallback(GLFWwindow* window, f64 xoffset, f64 yoffset);
    static void MousePositionCallback(GLFWwindow* window, f64 xposition, f64 yposition);

    void Update();

private:
    friend class Engine;
    Device* m_device = nullptr;
    InputData m_inputData;
};