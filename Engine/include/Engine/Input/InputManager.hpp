#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Vec3.hpp"

struct HWND__;
typedef HWND__* HWND;

namespace Engine {

enum class Key {
    A,
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
    Space,
    Escape,
    Enter,
    Shift,
    Control,
    Left,
    Right,
    Up,
    Down,
    Num1,
    Num2,
    Num3,
    Count
};

enum class MouseButton {
    Left,
    Right,
    Middle,
    Count
};

enum class Action {
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Fire,
    Pause,
    Confirm,
    Count
};

class ENGINE_API InputManager {
public:
    InputManager();

    void SetWindow(HWND hwnd);
    void Update();

    bool IsDown(Key key) const;
    bool WasPressed(Key key) const;
    bool WasReleased(Key key) const;

    bool IsMouseDown(MouseButton button) const;
    bool WasMousePressed(MouseButton button) const;

    float MouseX() const { return m_mouseX; }
    float MouseY() const { return m_mouseY; }
    float MouseDeltaX() const { return m_mouseDeltaX; }
    float MouseDeltaY() const { return m_mouseDeltaY; }

    bool IsActionDown(Action action) const;
    bool WasActionPressed(Action action) const;

    Vec3 GetMoveAxis() const;

private:
    static int ToVirtualKey(Key key);
    bool PollKey(Key key) const;
    bool PollMouse(MouseButton button) const;

    HWND m_hwnd = nullptr;

    bool m_currentKeys[static_cast<int>(Key::Count)] = {};
    bool m_previousKeys[static_cast<int>(Key::Count)] = {};
    bool m_currentMouse[static_cast<int>(MouseButton::Count)] = {};
    bool m_previousMouse[static_cast<int>(MouseButton::Count)] = {};

    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
    float m_mouseDeltaX = 0.0f;
    float m_mouseDeltaY = 0.0f;
    bool m_hasLastMouse = false;
    float m_lastMouseX = 0.0f;
    float m_lastMouseY = 0.0f;
};

} // namespace Engine
