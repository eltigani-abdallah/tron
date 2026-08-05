#pragma once
#include "../Input/PlayerController.h" 
#include <cmath>
#include <cstdint>

// ============================================================
//  Collider.h
//  Définit les formes de collision (AABB et Sphère)
//  ainsi que toutes les fonctions de test d'intersection.
//
//  Aucun état global ici — ce sont de simples structs + fonctions.
// ============================================================

// ============================================================
//  AABB — Axis-Aligned Bounding Box
//  Définie par son centre et ses demi-dimensions (half-extents).
// ============================================================ Résumé ia

struct AABB
{
    Vec3 center;     // position 
    Vec3 halfExtent; // (largeur/2, hauteur/2, profondeur/2)

    Vec3 Min() const
    {
        return { center.x - halfExtent.x,
                 center.y - halfExtent.y,
                 center.z - halfExtent.z };
    }

    Vec3 Max() const
    {
        return { center.x + halfExtent.x,
                 center.y + halfExtent.y,
                 center.z + halfExtent.z };
    }
};


//  ------------Sphère englobante--------------------------

struct BoundingSphere
{
    Vec3  center;
    float radius;
};

//  Tests d'intersection

// AABB vs AABB — true si les deux boîtes se chevauchent.
inline bool Intersects(const AABB& a, const AABB& b)
{
    Vec3 aMin = a.Min(), aMax = a.Max();
    Vec3 bMin = b.Min(), bMax = b.Max();

    return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
           (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
           (aMin.z <= bMax.z && aMax.z >= bMin.z);
}

// Sphère vs Sphère — true si les deux sphères se chevauchent.
inline bool Intersects(const BoundingSphere& a, const BoundingSphere& b)
{
    float dx   = a.center.x - b.center.x;
    float dy   = a.center.y - b.center.y;
    float dz   = a.center.z - b.center.z;
    float dist2 = dx*dx + dy*dy + dz*dz;
    float rSum  = a.radius + b.radius;
    return dist2 <= rSum * rSum;
}

// AABB vs Sphère — true si l'AABB et la sphère se chevauchent.
inline bool Intersects(const AABB& box, const BoundingSphere& sphere)
{
    // Point le plus proche de la sphère dans (ou sur) la boîte
    Vec3 bMin = box.Min(), bMax = box.Max();

    float cx = sphere.center.x < bMin.x ? bMin.x
             : sphere.center.x > bMax.x ? bMax.x
             : sphere.center.x;

    float cy = sphere.center.y < bMin.y ? bMin.y
             : sphere.center.y > bMax.y ? bMax.y
             : sphere.center.y;

    float cz = sphere.center.z < bMin.z ? bMin.z
             : sphere.center.z > bMax.z ? bMax.z
             : sphere.center.z;

    float dx = cx - sphere.center.x;
    float dy = cy - sphere.center.y;
    float dz = cz - sphere.center.z;

    return (dx*dx + dy*dy + dz*dz) <= sphere.radius * sphere.radius;
}

// -----------------------------------------------------------------------
//  Calcul de la pénétration AABB vs AABB
//  Retourne le vecteur de résolution minimal (MTV) :
//  additionner ce vecteur à la position de A permet de l'expulser de B.
// ----------------------------------------------------------------------- Explication ia

inline Vec3 ComputeMTV(const AABB& a, const AABB& b)
{
    Vec3 aMin = a.Min(), aMax = a.Max();
    Vec3 bMin = b.Min(), bMax = b.Max();

    // Pénétration sur chaque axe
    float overlapX = std::fminf(aMax.x, bMax.x) - std::fmaxf(aMin.x, bMin.x);
    float overlapY = std::fminf(aMax.y, bMax.y) - std::fmaxf(aMin.y, bMin.y);
    float overlapZ = std::fminf(aMax.z, bMax.z) - std::fmaxf(aMin.z, bMin.z);

    // Signe : direction d'expulsion (centre de A par rapport au centre de B)
    float signX = (a.center.x < b.center.x) ? -1.f : 1.f;
    float signY = (a.center.y < b.center.y) ? -1.f : 1.f;
    float signZ = (a.center.z < b.center.z) ? -1.f : 1.f;

    // Axe de moindre pénétration = résolution minimale
    if (overlapX < overlapY && overlapX < overlapZ)
        return { signX * overlapX, 0.f, 0.f };
    else if (overlapY < overlapZ)
        return { 0.f, signY * overlapY, 0.f };
    else
        return { 0.f, 0.f, signZ * overlapZ };
}
