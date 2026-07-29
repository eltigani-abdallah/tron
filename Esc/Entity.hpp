#include <cstdint>
using Entity = std::uint32_t;

const Entity MAX_ENTITIES = 5000;

class EntityManager{

private:
    Entity currentId;
    bool isALive(Entity& id);
    void Reset();
public:
    EntityManager();

    void GeneratedId(Entity& id);
    void CreatedEntity(Entity& id);
    void DestroyEntity(Entity&id);

};
