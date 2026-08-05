#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstring>

// ============================================================
//  InputManager
//  Gestion bas-niveau clavier + souris (Raw Input pour la souris).
//
//  Utilisation :
//    1. Appeler InputManager::Get().RegisterRawInput(hwnd) au démarrage.
//    2. Appeler InputManager::Get().ProcessMessage(msg) dans la boucle Win32.
//    3. En début de chaque frame : InputManager::Get().BeginFrame()
//    4. En fin   de chaque frame : InputManager::Get().EndFrame()
// ============================================================ Resumé ia

class InputManager
{
public:
    // -------------Singleton-------------------------------------------------
    static InputManager& Get()
    {
        static InputManager instance;
        return instance;
    }

    InputManager(const InputManager&)            = delete;
    InputManager& operator=(const InputManager&) = delete;

    // --------------Initialisation-----------------------------------------------------------
    void RegisterRawInput(HWND hwnd);
    void ProcessMessage(const MSG& msg);

    // --------------Cycle de frame-----------------------------------------------------------
    // À appeler au DÉBUT.
    void BeginFrame();

    // À appeler à la FIN.
    void EndFrame();

    // --------------Requêtes clavier-----------------------------------------------------------
    bool IsKeyHeld(int vkCode) const; // Touche maintenue (true tant qu'on appuie).
    bool IsKeyPressed(int vkCode) const; // Touche pressée.
    bool IsKeyReleased(int vkCode) const; // Touche relâchée.

    // ---------------------Requêtes souris----------------------------------------------------
    // Déplacement souris.
    float GetMouseDeltaX() const { return m_mouseDeltaX; }
    float GetMouseDeltaY() const { return m_mouseDeltaY; }

    // Bouton gauche/droit maintenu.
    bool IsMouseLeftHeld()  const { return m_mouseLeftHeld;  }
    bool IsMouseRightHeld() const { return m_mouseRightHeld; }

    // Bouton gauche cliqué.
    bool IsMouseLeftClicked()  const { return m_mouseLeftClicked;  }
    bool IsMouseRightClicked() const { return m_mouseRightClicked; }

    // -----------------Utilitaires--------------------------------------------------------
    void SetFPSMouseMode(HWND hwnd, bool enabled);

private:
    InputManager() { std::memset(this, 0, sizeof(*this)); }

    // État clavier sur 256 touches (BYTE = 0 ou 1)
    BYTE m_keysCurrentFrame[256] = {};
    BYTE m_keysPreviousFrame[256] = {};

    // Souris
    float m_mouseDeltaX       = 0.f;
    float m_mouseDeltaY       = 0.f;
    float m_mouseAccumX       = 0.f; 
    float m_mouseAccumY       = 0.f;

    bool  m_mouseLeftHeld     = false;
    bool  m_mouseRightHeld    = false;
    bool  m_mouseLeftClicked  = false;
    bool  m_mouseRightClicked = false;
    bool  m_prevMouseLeft     = false;
    bool  m_prevMouseRight    = false;

    bool  m_fpsModeEnabled    = false;
    HWND  m_hwnd              = nullptr;

    void HandleRawInput(LPARAM lParam);
};
