#pragma once
#include <cmath>

// ============================================================
//  PlayerController
//  Traduit les entrées (InputManager) en mouvements caméra/vaisseau.
//  Clavier AZERTY : Z=avancer  S=reculer  Q=gauche  D=droite
//
//  Interface volontairement découplée de l'ECS de P2 :
//    • Stocke Position et orientation en interne (Vec3 + angles).
//    • P2 peut brancher son propre Transform en remplaçant Vec3
//      par sa classe, ou en lisant GetPosition() / GetYaw() etc.
//
//  Axes :
//    X → droite   Y → haut   Z → avant (profondeur)
// ============================================================ Resumé ia

struct Vec3
{
    float x = 0.f, y = 0.f, z = 0.f;

    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator*(float s)       const { return {x*s,   y*s,   z*s};   }
    Vec3& operator+=(const Vec3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
};

class PlayerController
{
public:
    PlayerController() = default;

    // ------------Configuration-------------------------------------------

    struct Settings
    {
        float moveSpeed      = 10.f;   // unités/seconde
        float mouseSensivity = 0.10f;  // degrés par pixel
        float pitchClamp     = 89.f;   // degrés max haut/bas

        bool  invertPitchAxis = false; // true = mode "vol inversé"
    };

    void Configure(const Settings& s) { m_settings = s; }

    // ------------Mise à jour (appeler chaque frame)---------------------------
    //DeltaTime en secondes.
    void Update(float deltaTime);

    // ------------------Tir-------------------------------------------------------
    /// Retourne true le frame où le joueur a tiré (clic gauche).
    bool WantsToShoot() const { return m_wantsToShoot; }

    // -----------------osition / orientation--------------------------------------

    const Vec3& GetPosition() const { return m_position; }
    void        SetPosition(const Vec3& p) { m_position = p; }

    float GetYaw()   const { return m_yaw;   }
    float GetPitch() const { return m_pitch; } 

    //Vecteur "avant" (direction du regard).
    Vec3 GetForward() const;

    //Vecteur "droite"(perpendiculaire au regard, plan XZ).
    Vec3 GetRight() const;

    //Vecteur "haut" monde (0, 1, 0) (Fixe).
    static Vec3 GetUp() { return {0.f, 1.f, 0.f}; }

private:
    Settings m_settings;

    Vec3  m_position = {0.f, 0.f, 0.f};
    float m_yaw      = 0.f; 
    float m_pitch    = 0.f;

    bool  m_wantsToShoot = false;

    static constexpr float DEG2RAD = 3.14159265f / 180.f;
};
