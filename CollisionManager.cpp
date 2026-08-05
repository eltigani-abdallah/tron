#include "CollisionManager.h"
#include <algorithm>

//-----------Enregistrement--------------------------------------------------------------
ColliderID CollisionManager::Register(const AABB& box,
                                       CollisionResponse response,
                                       bool isPlayer)
{
    CollisionObject obj;
    obj.id       = m_nextID++;
    obj.shape    = ColliderShape::AABB;
    obj.response = response;
    obj.aabb     = box;
    obj.active   = true;
    obj.isPlayer = isPlayer;
    m_objects.push_back(obj);
    return obj.id;
}

ColliderID CollisionManager::Register(const BoundingSphere& sphere,
                                       CollisionResponse response,
                                       bool isPlayer)
{
    CollisionObject obj;
    obj.id       = m_nextID++;
    obj.shape    = ColliderShape::Sphere;
    obj.response = response;
    obj.sphere   = sphere;
    obj.active   = true;
    obj.isPlayer = isPlayer;
    m_objects.push_back(obj);
    return obj.id;
}

//-------------------------------------------------------------------------

void CollisionManager::Unregister(ColliderID id)
{
    m_objects.erase(
        std::remove_if(m_objects.begin(), m_objects.end(),
                       [id](const CollisionObject& o){ return o.id == id; }),
        m_objects.end());
}

void CollisionManager::SetActive(ColliderID id, bool active)
{
    if (CollisionObject* obj = Find(id))
        obj->active = active;
}

// -------------Mise à jour de position------------------------------------------------------------
void CollisionManager::SetPosition(ColliderID id, const Vec3& newCenter)
{
    if (CollisionObject* obj = Find(id))
        obj->aabb.center = newCenter;
}

void CollisionManager::SetSpherePosition(ColliderID id, const Vec3& newCenter)
{
    if (CollisionObject* obj = Find(id))
        obj->sphere.center = newCenter;
}

//--------------Déplacement joueur avec résolution de collision-----------------------------------------------------------
Vec3 CollisionManager::MoveAndResolve(ColliderID playerID,
                                       const Vec3& currentPos,
                                       const Vec3& delta)
{
    CollisionObject* player = Find(playerID);
    if (!player || !player->active) return currentPos + delta;

    //Applique le déplacement désiré
    Vec3 newPos = currentPos + delta;

    if (player->shape == ColliderShape::AABB)
    {
        player->aabb.center = newPos;

        //Teste contre tous les objets Block actifs
        for (CollisionObject& other : m_objects)
        {
            if (other.id == playerID) continue;
            if (!other.active)        continue;
            if (other.response != CollisionResponse::Block) continue;

            bool hit = false;

            if (other.shape == ColliderShape::AABB)
                hit = Intersects(player->aabb, other.aabb);
            else
                hit = Intersects(other.aabb, player->sphere); //AABB(player) vs Sphere

            if (hit && other.shape == ColliderShape::AABB)
            {
                //Expulsion minimale : MTV
                Vec3 mtv = ComputeMTV(player->aabb, other.aabb);
                newPos.x += mtv.x;
                newPos.y += mtv.y;
                newPos.z += mtv.z;
                player->aabb.center = newPos;
            }
            else if (hit)
            {
                //Sphère obstacle vs joueur AABB : recule à la position précédente
                newPos   = currentPos;
                player->aabb.center = newPos;
                break; 
            }
        }
    }
    else 
    {
        player->sphere.center = newPos;

        for (CollisionObject& other : m_objects)
        {
            if (other.id == playerID) continue;
            if (!other.active)        continue;
            if (other.response != CollisionResponse::Block) continue;

            bool hit = false;

            if (other.shape == ColliderShape::Sphere)
                hit = Intersects(player->sphere, other.sphere);
            else
                hit = Intersects(other.aabb, player->sphere);

            if (hit)
            {
                //Recule à la position précédente
                newPos = currentPos;
                player->sphere.center = newPos;
                break;
            }
        }
    }

    return newPos;
}

// --------------Détection globale-----------------------------------------------------------
void CollisionManager::TestAll(std::vector<CollisionResult>& outResults,
                                std::vector<ColliderID>&      outDestroyed)
{
    outResults.clear();

    for (size_t i = 0; i < m_objects.size(); ++i)
    {
        const CollisionObject& a = m_objects[i];
        if (!a.active) continue;

        for (size_t j = i + 1; j < m_objects.size(); ++j)
        {
            CollisionObject& b = m_objects[j];
            if (!b.active) continue;

            if (!TestPair(a, b)) continue;

            //Collision détectée
            CollisionResult result;
            result.idA = a.id;
            result.idB = b.id;
            result.hit = true;
            outResults.push_back(result);

            //Réaction Destroy : désactive l'objet
            if (b.response == CollisionResponse::Destroy)
            {
                b.active = false;
                outDestroyed.push_back(b.id);
            }
            if (a.response == CollisionResponse::Destroy)
            {
                if (CollisionObject* aMut = Find(a.id))
                {
                    aMut->active = false;
                    outDestroyed.push_back(a.id);
                }
            }
        }
    }
}

// --------------Helpers privés-----------------------------------------------------------
bool CollisionManager::TestPair(const CollisionObject& a,
                                 const CollisionObject& b) const
{
    if (a.shape == ColliderShape::AABB && b.shape == ColliderShape::AABB)
        return Intersects(a.aabb, b.aabb);

    if (a.shape == ColliderShape::Sphere && b.shape == ColliderShape::Sphere)
        return Intersects(a.sphere, b.sphere);

    // AABB vs Sphère (ordre indifférent)
    if (a.shape == ColliderShape::AABB)
        return Intersects(a.aabb, b.sphere);
    else
        return Intersects(b.aabb, a.sphere);
}

CollisionObject* CollisionManager::Find(ColliderID id)
{
    for (CollisionObject& obj : m_objects)
        if (obj.id == id) return &obj;
    return nullptr;
}
