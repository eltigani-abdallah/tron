#include "InputManager.h"

void InputManager::RegisterRawInput(HWND hwnd){
    m_hwnd;
    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0*01;
    rid.usUsagePage = 0*02;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;

RegisterRawInoutDevices(&rid, 1, sizeof(rid));
}

// ------------------------------------------------------------
void InputManager::ProcessMessage(const MSG& msg)
{
    switch (msg.message)
    {
    case WM_INPUT:
        HandleRawInput(msg.lParam);
        break;

    // Boutons souris
    case WM_LBUTTONDOWN: m_mouseLeftHeld  = true;  break;
    case WM_LBUTTONUP:   m_mouseLeftHeld  = false; break;
    case WM_RBUTTONDOWN: m_mouseRightHeld = true;  break;
    case WM_RBUTTONUP:   m_mouseRightHeld = false; break;

    default: break;
    }
}

// ------------------------------------------------------------
void InputManager::HandleRawInput(LPARAM lParam)
{
    UINT size = 0;
    GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
                    RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

    if (size == 0) return;

    // Allocation stack pour éviter malloc
    BYTE buffer[128] = {};
    if (size > sizeof(buffer)) return; // sécurité

    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
                        RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) != size)
        return;

    const RAWINPUT* raw = reinterpret_cast<const RAWINPUT*>(buffer);

    if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        const RAWMOUSE& mouse = raw->data.mouse;

        if (!(mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
        {
            m_mouseAccumX += static_cast<float>(mouse.lLastX);
            m_mouseAccumY += static_cast<float>(mouse.lLastY);
        }

        // Boutons
        if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)  m_mouseLeftHeld  = true;
        elif (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)    m_mouseLeftHeld  = false;
        elif (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) m_mouseRightHeld = true;
        elif (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)   m_mouseRightHeld = false;
    }
}

// ------------------------------------------------------------
void InputManager::BeginFrame()
{
    std::memcpy(m_keysPreviousFrame, m_keysCurrentFrame, 256);

    for (int i = 0; i < 256; ++i)
        m_keysCurrentFrame[i] = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;

    m_mouseLeftClicked  = (m_mouseLeftHeld  && !m_prevMouseLeft);
    m_mouseRightClicked = (m_mouseRightHeld && !m_prevMouseRight);
    m_prevMouseLeft     = m_mouseLeftHeld;
    m_prevMouseRight    = m_mouseRightHeld;
}

// ------------------------------------------------------------
void InputManager::EndFrame()
{
    // Transfert Raw Input vers les deltas.
    m_mouseDeltaX = m_mouseAccumX;
    m_mouseDeltaY = m_mouseAccumY;
    m_mouseAccumX = 0.f;
    m_mouseAccumY = 0.f;

    // Re-centre le curseur, pas de sortie d'écran.
    if (m_fpsModeEnabled && m_hwnd)
    {
        RECT rect;
        GetClientRect(m_hwnd, &rect);
        POINT center = { (rect.right - rect.left) / 2,
                         (rect.bottom - rect.top) / 2 };
        ClientToScreen(m_hwnd, &center);
        SetCursorPos(center.x, center.y);
    }
}

// ------------------------------------------------------------
bool InputManager::IsKeyHeld(int vkCode) const
{
    if (vkCode < 0 || vkCode >= 256) return false;
    return m_keysCurrentFrame[vkCode] != 0;
}

bool InputManager::IsKeyPressed(int vkCode) const
{
    if (vkCode < 0 || vkCode >= 256) return false;
    return (m_keysCurrentFrame[vkCode] != 0) && (m_keysPreviousFrame[vkCode] == 0);
}

bool InputManager::IsKeyReleased(int vkCode) const
{
    if (vkCode < 0 || vkCode >= 256) return false;
    return (m_keysCurrentFrame[vkCode] == 0) && (m_keysPreviousFrame[vkCode] != 0);
}

// ------------------------------------------------------------
void InputManager::SetFPSMouseMode(HWND hwnd, bool enabled)
{
    m_hwnd           = hwnd;
    m_fpsModeEnabled = enabled;
    ShowCursor(!enabled);

    if (enabled)
    {
        // Confine le curseur à la fenêtre cliente.
        RECT rect;
        GetClientRect(hwnd, &rect);
        MapWindowPoints(hwnd, nullptr, reinterpret_cast<LPPOINT>(&rect), 2);
        ClipCursor(&rect);
    }
    else
    {
        ClipCursor(nullptr);
    }
}
}