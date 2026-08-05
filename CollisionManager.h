#pragma once
#include "Collider.h"
#include <vector>
#include <cstdint>

// ============================================================
//  CollisionManager
//  Registre de tous les objets collidables.
//  Chaque objet est un CollisionObject : forme + type de réaction.
//
//  Utilisation :
//    1. CollisionManager::Get().Register(...)   → ajoute un objet
//    2. CollisionManager::Get().Unregister(id)  → retire un objet
//    3. CollisionManager::Get().MovePlayer(...)  → déplace le joueur
//       avec résolution automatique (bloquage)
//    4. CollisionManager::Get().TestAll()        → détecte les paires
//       actives et applique les réactions (destroy, etc.)
// ============================================================ Résumé ia

using ColliderID = uint32_t;
static constexpr ColliderID INVALID_COLLIDER = 0;

//------------Type de forme---------------------------------------------------
enum class ColliderShape { AABB, Sphere };

//------------Réaction à une collision----------------------------------------
enum class CollisionResponse
{
    Block,    // Bloque le passage (mur, obstacle)
    Destroy,  // Objet supprimé au contact (projectile, ennemi, bonus)
    Trigger,  // Détecte sans réagir physiquement (zone de déclenchement)
};

//----------CollisionObject — Objet enregistré dans manager------------------------

struct CollisionObject
{
    ColliderID       id       = INVALID_COLLIDER;
    ColliderShape    shape    = ColliderShape::AABB;
    CollisionResponse response = CollisionResponse::Block;

    // Une seule des deux formes est utilisée selon `shape`
    AABB            aabb     = {};
    BoundingSphere  sphere   = {};

    bool  active   = true;  // false = ignoré lors des tests
    bool  isPlayer = false; // l'objet joueur est traité à part
};

//----------Résultat d'une collision------------------------------------------------

struct CollisionResult
{
    ColliderID idA = INVALID_COLLIDER;
    ColliderID idB = INVALID_COLLIDER;
    bool       hit = false;
};

//----------CollisionManager--------------------------------------------------------

class CollisionManager
{
public:
    static CollisionManager& Get()
    {
        static CollisionManager instance;
        return instance;
    }

    CollisionManager(const CollisionManager&)            = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

    //-----------Enregistrement--------------------------------------------------------------

    // Ajoute un objet AABB. Retourne son ID.
    ColliderID Register(const AABB& box,
                        CollisionResponse response = CollisionResponse::Block,
                        bool isPlayer = false);

    // Ajoute un objet Sphère. Retourne son ID.
    ColliderID Register(const BoundingSphere& sphere,
                        CollisionResponse response = CollisionResponse::Block,
                        bool isPlayer = false);

    void Unregister(ColliderID id); // Retire un objet par son ID.
    void SetActive(ColliderID id, bool active); // Active ou désactive un objet.

    // -----------Mise à jour de position--------------------------------------------------------------

    /// Met à jour le centre d'un collider AABB.
    void SetPosition(ColliderID id, const Vec3& newCenter);

    /// Met à jour le centre d'un collider Sphère.
    void SetSpherePosition(ColliderID id, const Vec3& newCenter);

    // -----------Déplacement joueur avec résolution de collision---------------------------------------------
    Vec3 MoveAndResolve(ColliderID playerID, const Vec3& currentPos, const Vec3& delta);

    // ----------Détection globale (hors joueur)---------------------------------------------------------------
    void TestAll(std::vector<CollisionResult>& outResults,
                 std::vector<ColliderID>&      outDestroyed);

    CollisionObject* Find(ColliderID id);

private:
    CollisionManager() = default;

    std::vector<CollisionObject> m_objects;
    ColliderID                   m_nextID = 1;

    // Test d'intersection générique entre deux objets 
    bool TestPair(const CollisionObject& a, const CollisionObject& b) const;
};
