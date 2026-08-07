#include "TestAssert.hpp"

#include "Engine/Physics/Collider.hpp"
#include "Engine/Physics/SpatialPartition.hpp"

void TestSpatialPartition() {
    Engine::SpatialPartition grid(2.0f);

    Engine::AABB nearA{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    Engine::AABB nearB{{0.5f, 0.5f, 0.5f}, {1.5f, 1.5f, 1.5f}};
    Engine::AABB farC{{20.0f, 0.0f, 0.0f}, {21.0f, 1.0f, 1.0f}};

    grid.Insert(1, nearA);
    grid.Insert(2, nearB);
    grid.Insert(3, farC);

    std::vector<std::pair<Engine::Entity, Engine::Entity>> pairs;
    grid.QueryPotentialPairs(pairs);

    bool found12 = false;
    bool found13 = false;
    bool found23 = false;
    for (const auto& [a, b] : pairs) {
        if ((a == 1 && b == 2) || (a == 2 && b == 1)) {
            found12 = true;
        }
        if ((a == 1 && b == 3) || (a == 3 && b == 1)) {
            found13 = true;
        }
        if ((a == 2 && b == 3) || (a == 3 && b == 2)) {
            found23 = true;
        }
    }

    EXPECT(found12);
    EXPECT(!found13);
    EXPECT(!found23);
}
