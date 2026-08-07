#include "Engine/Input/InputManager.hpp"

#include <Windows.h>

namespace Engine {

InputManager::InputManager() = default;

void InputManager::SetWindow(HWND hwnd) {
    m_hwnd = hwnd;
    m_hasLastMouse = false;
}

void InputManager::Update() {
    for (int i = 0; i < static_cast<int>(Key::Count); ++i) {
        m_previousKeys[i] = m_currentKeys[i];
        m_currentKeys[i] = PollKey(static_cast<Key>(i));
    }

    for (int i = 0; i < static_cast<int>(MouseButton::Count); ++i) {
        m_previousMouse[i] = m_currentMouse[i];
        m_currentMouse[i] = PollMouse(static_cast<MouseButton>(i));
    }

    POINT cursor = {};
    if (GetCursorPos(&cursor)) {
        if (m_hwnd != nullptr) {
            ScreenToClient(m_hwnd, &cursor);
        }

        m_mouseX = static_cast<float>(cursor.x);
        m_mouseY = static_cast<float>(cursor.y);

        if (m_hasLastMouse) {
            m_mouseDeltaX = m_mouseX - m_lastMouseX;
            m_mouseDeltaY = m_mouseY - m_lastMouseY;
        } else {
            m_mouseDeltaX = 0.0f;
            m_mouseDeltaY = 0.0f;
            m_hasLastMouse = true;
        }

        m_lastMouseX = m_mouseX;
        m_lastMouseY = m_mouseY;
    } else {
        m_mouseDeltaX = 0.0f;
        m_mouseDeltaY = 0.0f;
    }
}

bool InputManager::IsDown(Key key) const {
    return m_currentKeys[static_cast<int>(key)];
}

bool InputManager::WasPressed(Key key) const {
    const int index = static_cast<int>(key);
    return m_currentKeys[index] && !m_previousKeys[index];
}

bool InputManager::WasReleased(Key key) const {
    const int index = static_cast<int>(key);
    return !m_currentKeys[index] && m_previousKeys[index];
}

bool InputManager::IsMouseDown(MouseButton button) const {
    return m_currentMouse[static_cast<int>(button)];
}

bool InputManager::WasMousePressed(MouseButton button) const {
    const int index = static_cast<int>(button);
    return m_currentMouse[index] && !m_previousMouse[index];
}

bool InputManager::IsActionDown(Action action) const {
    switch (action) {
    case Action::MoveForward:
        return IsDown(Key::Z) || IsDown(Key::W) || IsDown(Key::Up);
    case Action::MoveBackward:
        return IsDown(Key::S) || IsDown(Key::Down);
    case Action::MoveLeft:
        return IsDown(Key::Q) || IsDown(Key::A) || IsDown(Key::Left);
    case Action::MoveRight:
        return IsDown(Key::D) || IsDown(Key::Right);
    case Action::MoveUp:
        return IsDown(Key::Space) || IsDown(Key::E);
    case Action::MoveDown:
        return IsDown(Key::Control) || IsDown(Key::C);
    case Action::Fire:
        return IsMouseDown(MouseButton::Left);
    case Action::Pause:
        return IsDown(Key::P) || IsDown(Key::Escape);
    case Action::Confirm:
        return IsDown(Key::Space) || IsDown(Key::Enter);
    default:
        return false;
    }
}

bool InputManager::WasActionPressed(Action action) const {
    switch (action) {
    case Action::MoveForward:
        return WasPressed(Key::Z) || WasPressed(Key::W) || WasPressed(Key::Up);
    case Action::MoveBackward:
        return WasPressed(Key::S) || WasPressed(Key::Down);
    case Action::MoveLeft:
        return WasPressed(Key::Q) || WasPressed(Key::A) || WasPressed(Key::Left);
    case Action::MoveRight:
        return WasPressed(Key::D) || WasPressed(Key::Right);
    case Action::MoveUp:
        return WasPressed(Key::Space) || WasPressed(Key::E);
    case Action::MoveDown:
        return WasPressed(Key::Control) || WasPressed(Key::C);
    case Action::Fire:
        return WasMousePressed(MouseButton::Left);
    case Action::Pause:
        return WasPressed(Key::P) || WasPressed(Key::Escape);
    case Action::Confirm:
        return WasPressed(Key::Space) || WasPressed(Key::Enter);
    default:
        return false;
    }
}

Vec3 InputManager::GetMoveAxis() const {
    Vec3 axis = Vec3::Zero();
    if (IsActionDown(Action::MoveForward)) {
        axis.z += 1.0f;
    }
    if (IsActionDown(Action::MoveBackward)) {
        axis.z -= 1.0f;
    }
    if (IsActionDown(Action::MoveRight)) {
        axis.x += 1.0f;
    }
    if (IsActionDown(Action::MoveLeft)) {
        axis.x -= 1.0f;
    }
    if (IsActionDown(Action::MoveUp)) {
        axis.y += 1.0f;
    }
    if (IsActionDown(Action::MoveDown)) {
        axis.y -= 1.0f;
    }
    return axis.Normalized();
}

int InputManager::ToVirtualKey(Key key) {
    switch (key) {
    case Key::A: return 'A';
    case Key::B: return 'B';
    case Key::C: return 'C';
    case Key::D: return 'D';
    case Key::E: return 'E';
    case Key::F: return 'F';
    case Key::G: return 'G';
    case Key::H: return 'H';
    case Key::I: return 'I';
    case Key::J: return 'J';
    case Key::K: return 'K';
    case Key::L: return 'L';
    case Key::M: return 'M';
    case Key::N: return 'N';
    case Key::O: return 'O';
    case Key::P: return 'P';
    case Key::Q: return 'Q';
    case Key::R: return 'R';
    case Key::S: return 'S';
    case Key::T: return 'T';
    case Key::U: return 'U';
    case Key::V: return 'V';
    case Key::W: return 'W';
    case Key::X: return 'X';
    case Key::Y: return 'Y';
    case Key::Z: return 'Z';
    case Key::Space: return VK_SPACE;
    case Key::Escape: return VK_ESCAPE;
    case Key::Enter: return VK_RETURN;
    case Key::Shift: return VK_SHIFT;
    case Key::Control: return VK_CONTROL;
    case Key::Left: return VK_LEFT;
    case Key::Right: return VK_RIGHT;
    case Key::Up: return VK_UP;
    case Key::Down: return VK_DOWN;
    case Key::Num1: return '1';
    case Key::Num2: return '2';
    case Key::Num3: return '3';
    default: return 0;
    }
}

bool InputManager::PollKey(Key key) const {
    const int vk = ToVirtualKey(key);
    if (vk == 0) {
        return false;
    }
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool InputManager::PollMouse(MouseButton button) const {
    int vk = 0;
    switch (button) {
    case MouseButton::Left:
        vk = VK_LBUTTON;
        break;
    case MouseButton::Right:
        vk = VK_RBUTTON;
        break;
    case MouseButton::Middle:
        vk = VK_MBUTTON;
        break;
    default:
        return false;
    }
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

} // namespace Engine
