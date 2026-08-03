#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// #include <d3d11.h> 
#include "InputManager.h"
#include "PlayerController.h"

static HWND             g_hwnd       = nullptr;
static bool             g_running    = true;
static PlayerController g_player;
static LARGE_INTEGER    g_lastTime;
static LARGE_INTEGER    g_frequency;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        g_running = false;
        PostQuitMessage(0);
        return 0;

    case WM_KILLFOCUS:
        // Quand la fenêtre perd le focus, libère le curseur.
        InputManager::Get().SetFPSMouseMode(hwnd, false);
        return 0;

    case WM_SETFOCUS:
        InputManager::Get().SetFPSMouseMode(hwnd, true);
        return 0;

    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---------------Boucle de jeu------------------------

static float ComputeDeltaTime()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    float dt = static_cast<float>(now.QuadPart - g_lastTime.QuadPart)
             / static_cast<float>(g_frequency.QuadPart);

    g_lastTime = now;

    // Sécurité.
    if (dt > 0.1f) dt = 0.1f;
    return dt;
}

// ------------------WinMain--------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // --------1.Création de la fenêtre (simplifié)----------------------------

    WNDCLASSEX wc   = {};
    wc.cbSize       = sizeof(wc);
    wc.lpfnWndProc  = WndProc;
    wc.hInstance    = hInstance;
    wc.lpszClassName = L"TRONWindow";
    RegisterClassEx(&wc);

    g_hwnd = CreateWindowEx(
        0, L"TRONWindow", L"TRON — First Person Spaceship",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(g_hwnd, nCmdShow);

    // ----------2.Init InputManager--------------------------------------------

    InputManager::Get().RegisterRawInput(g_hwnd);
    InputManager::Get().SetFPSMouseMode(g_hwnd, true); // cache + confine le curseur

    // -------3.Config PlayerController--------------------------------------

    PlayerController::Settings settings;
    settings.moveSpeed       = 15.f; 
    settings.mouseSensivity  = 0.15f; //A ajuste pendant les tests finaux
    settings.pitchClamp      = 89.f;
    settings.invertPitchAxis = false;
    g_player.Configure(settings);

    g_player.SetPosition({0.f, 0.f, -5.f}); //Position de départ

    // ----------4.Timer--------------------------------------------------------

    QueryPerformanceFrequency(&g_frequency);
    QueryPerformanceCounter(&g_lastTime);

    // ---------5.Boucle principale--------------------------------------------

    while (g_running)
    {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            InputManager::Get().ProcessMessage(msg);

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                g_running = false;
        }

        if (!g_running) break;

        //Début de frame.
        float dt = ComputeDeltaTime();
        InputManager::Get().BeginFrame();

        //Mise à jour du joueur.
        g_player.Update(dt);

        // Echap = quitter.
        if (InputManager::Get().IsKeyPressed(VK_ESCAPE))
            g_running = false;

        // Exemple : tir
        if (g_player.WantsToShoot())
        {
            // Spawn un projectile à g_player.GetPosition()
            // dans la direction g_player.GetForward()
            // Fichier de P2
        }

        // --Position/orientation pour la caméra de P1--
        //
        //  P1 a besoin de :
        //    g_player.GetPosition()   → vec3 position
        //    g_player.GetYaw()        → rotation Y (degrés)
        //    g_player.GetPitch()      → rotation X (degrés)
        //    g_player.GetForward()    → vecteur "regard" pour la view matrix
        //
        //  Exemple d'appel vers le renderer de P1 (pseudo-code) :
        //    g_renderer.SetCameraTransform(
        //        g_player.GetPosition(),
        //        g_player.GetForward(),
        //        PlayerController::GetUp());

        // --Rendu D3D (P1)--
        // g_renderer.Render();

        InputManager::Get().EndFrame();
    }

    // ------6.Nettoyage----------------------------------------------------

    InputManager::Get().SetFPSMouseMode(g_hwnd, false);
    DestroyWindow(g_hwnd);

    return 0;
}
