#include "TestAssert.hpp"

#include "Engine/ECS/Components.hpp"
#include "Engine/ECS/World.hpp"

void TestWorld() {
    Engine::World world;

    const Engine::Entity a = world.CreateEntity();
    const Engine::Entity b = world.CreateEntity();
    EXPECT(a != b);
    EXPECT(world.IsAlive(a));
    EXPECT(world.IsAlive(b));

    Engine::TransformComponent transform;
    transform.local.position = {1.0f, 2.0f, 3.0f};
    world.AddComponent<Engine::TransformComponent>(a, transform);
    EXPECT(world.HasComponent<Engine::TransformComponent>(a));
    EXPECT(!world.HasComponent<Engine::TransformComponent>(b));

    const Engine::TransformComponent* read =
        world.GetComponent<Engine::TransformComponent>(a);
    EXPECT(read != nullptr);
    EXPECT_NEAR(read->local.position.x, 1.0f, 0.0001f);

    world.AddComponent<Engine::TagComponent>(a, Engine::TagComponent{Engine::EntityTag::Player});
    world.AddComponent<Engine::TagComponent>(b, Engine::TagComponent{Engine::EntityTag::Enemy});
    EXPECT(world.FindFirstWithTag(Engine::EntityTag::Player) == a);
    EXPECT(world.FindFirstWithTag(Engine::EntityTag::Enemy) == b);
    EXPECT(world.FindFirstWithTag(Engine::EntityTag::Projectile) == Engine::kInvalidEntity);

    world.QueueDestroy(a);
    EXPECT(world.IsAlive(a));
    world.FlushDestroyed();
    EXPECT(!world.IsAlive(a));
    EXPECT(world.GetComponent<Engine::TransformComponent>(a) == nullptr);
    EXPECT(world.FindFirstWithTag(Engine::EntityTag::Player) == Engine::kInvalidEntity);
    EXPECT(world.IsAlive(b));
}
