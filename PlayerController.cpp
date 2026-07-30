#include "PlayerController.h"
#include "InputManager.h"

// ============================================================
//  PlayerController — implémentation
// ============================================================
// Codes virtuels pour ZQSD (AZERTY).
// GetAsyncKeyState utilise les VK codes, pas les caractères affichés.
// Sur un clavier AZERTY : la touche physique 'Z' envoie VK_W, etc.
// Pour couvrir les deux layouts on teste les deux codes.
//============================================================= Resumé ia

void PlayerController::Update(float deltaTime)
{
    InputManager& im = InputManager::Get();

    // ------------Rotation souris------------------------------------------------------
    float dx = im.GetMouseDeltaX();
    float dy = im.GetMouseDeltaY();

    m_yaw   += dx * m_settings.mouseSensivity;
    m_pitch += dy * m_settings.mouseSensivity * (m_settings.invertPitchAxis ? -1.f : 1.f);

    if (m_pitch >  m_settings.pitchClamp) m_pitch =  m_settings.pitchClamp;
    if (m_pitch < -m_settings.pitchClamp) m_pitch = -m_settings.pitchClamp; // Clamp pitch pour éviter le "flip" de caméra
    if (m_yaw >= 360.f) m_yaw -= 360.f; // Normalise yaw dans [0, 360]
    if (m_yaw <    0.f) m_yaw += 360.f;

    // ----------------Déplacement clavier AZERTY------------------------
    //     Z = avancer   S = reculer   Q = gauche   D = droite
    // ------------------------------------------------------------------

    Vec3 forward = GetForward();
    Vec3 right   = GetRight();

    Vec3 moveDir = {0.f, 0.f, 0.f};

    // Avancer : Z
    if (im.IsKeyHeld('Z'))
        moveDir += forward;

    // Reculer : S
    if (im.IsKeyHeld('S'))
        moveDir += forward * -1.f;

    // Gauche : Q
    if (im.IsKeyHeld('Q'))
        moveDir += right * -1.f;

    // Droite : D
    if (im.IsKeyHeld('D'))
        moveDir += right;

    // Montée / descente (optionnel)
    if (im.IsKeyHeld(VK_SPACE))
        moveDir += GetUp();

    if (im.IsKeyHeld(VK_LSHIFT) || im.IsKeyHeld(VK_RSHIFT))
    {
        Vec3 down = GetUp() * -1.f;
        moveDir += down;
    }

    // Normalise le vecteur de déplacement.
    // Diagonale.
    float len = std::sqrtf(moveDir.x * moveDir.x +
                           moveDir.y * moveDir.y +
                           moveDir.z * moveDir.z);
    if (len > 0.001f)
    {
        float inv = 1.f / len;
        moveDir = moveDir * inv;
    }

    m_position += moveDir * (m_settings.moveSpeed * deltaTime);

    // -------------Tir — clic gauche---------------------------------------------------
    m_wantsToShoot = im.IsMouseLeftClicked();
}

// -------------Helpers géométrie-----------------------------------------------------
Vec3 PlayerController::GetForward() const
{
    // Convention :  +Z = avant dans l'espace monde.
    // La caméra regarde en +Z par défaut, puis on applique yaw (Y) et pitch (X).
    float yawRad   = m_yaw   * DEG2RAD;
    float pitchRad = m_pitch * DEG2RAD;

    return {
        std::sinf(yawRad) * std::cosf(pitchRad),   // X
        -std::sinf(pitchRad),                      // Y (Vers le haut = Y négatif)
        std::cosf(yawRad) * std::cosf(pitchRad)    // Z
    };
}

Vec3 PlayerController::GetRight() const
{
    // Produit vectoriel(forward, up)
    Vec3 f = GetForward();
    Vec3 u = GetUp();

    return {
        u.y * f.z - u.z * f.y,
        u.z * f.x - u.x * f.z,
        u.x * f.y - u.y * f.x
    };
}
