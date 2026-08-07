#pragma once

#include "Engine/Core/Application.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/ECS/World.hpp"
#include "Engine/Physics/Collider.hpp"
#include "Engine/Render/Mesh.hpp"
#include "Engine/States/State.hpp"
#include "Engine/States/StateManager.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/UI/UISystem.hpp"
#include "CollisionFlashScript.hpp"
#include "EnemyScript.hpp"
#include "GameScore.hpp"
#include "RotateCubeScript.hpp"
#include "ShipControllerScript.hpp"

#include <cmath>
#include <cstdio>
#include <memory>

class GameState : public Engine::State {
public:
    explicit GameState(Engine::Application& app)
        : m_app(app) {
    }

    void Enter() override {
        m_timer = 0.0f;
        m_hudTimer = 0.0f;
        m_spawnTimer = 1.5f;
        m_enemyCount = 0;
        m_seed = 0xC0FFEEu;
        GameSession::Reset();
        m_app.SetClearColor(0.01f, 0.03f, 0.09f);
        m_app.SetWindowTitle("TRON - Game");
        m_app.ResetPhysics();
        m_app.DiscardNextFrameDelta();
        m_controlLock = 0.2f;

        Engine::UISystem& ui = m_app.GetUI();
        ui.Clear();
        ui.SetVisible(true);
        ui.SetText("score", "SCORE:0", 24.0f, 20.0f, 3.5f, {1.0f, 0.92f, 0.35f});
        ui.SetText("hp", "HP:100", 24.0f, 56.0f, 3.0f, {0.35f, 1.0f, 0.65f});
        ui.SetText("goal", "KILL 6 OR SCORE 80", 24.0f, 92.0f, 2.0f, {0.55f, 0.95f, 1.0f});
        ui.SetText("help", "ZQSD MOVE  RMB LOOK  LMB FIRE", 24.0f, 124.0f, 2.0f, {0.45f, 0.75f, 0.95f});
        ui.SetText("pp", "POST 1/2/3   TIME T", 24.0f, 152.0f, 2.0f, {0.7f, 0.55f, 1.0f});
        ui.SetText("stats", "DRAW:0  CULL:0", 24.0f, 180.0f, 2.0f, {0.5f, 0.7f, 0.85f});

        m_app.SetPostProcess(1.22f, 0.03f, 1.12f);
        m_app.GetTime().SetTimeScale(1.0f);
        m_app.GetTime().ClearSlowMotion();

        Engine::World& world = m_app.GetWorld();
        world.Clear();

        const Engine::Entity player = world.CreateEntity();
        world.AddComponent<Engine::TransformComponent>(player);
        Engine::ColliderComponent playerCollider;
        playerCollider.shape = Engine::ColliderShape::Sphere;
        playerCollider.radius = 0.35f;
        playerCollider.isStatic = false;
        playerCollider.isTrigger = false;
        world.AddComponent<Engine::ColliderComponent>(player, playerCollider);
        world.AddComponent<Engine::TagComponent>(player, Engine::TagComponent{Engine::EntityTag::Player});

        Engine::ParticleEmitterComponent gunEmitter;
        gunEmitter.maxParticles = 256;
        gunEmitter.emissionRate = 0.0f;
        gunEmitter.startColor = {0.4f, 0.9f, 1.0f};
        gunEmitter.endColor = {0.1f, 0.3f, 1.0f};
        gunEmitter.minSpeed = 4.0f;
        gunEmitter.maxSpeed = 10.0f;
        gunEmitter.directionJitter = 0.15f;
        world.AddComponent<Engine::ParticleEmitterComponent>(player, std::move(gunEmitter));
        world.AddScript(player, std::make_unique<ShipControllerScript>());

        BuildProceduralLevel(world);
        SpawnEnemy(world, {8.0f, 1.0f, 8.0f});
        SpawnEnemy(world, {-10.0f, 1.0f, 6.0f});

        world.StartScripts();
        UpdateHud();
    }

    void Update(float deltaSeconds) override {
        if (m_controlLock > 0.0f) {
            m_controlLock -= deltaSeconds;
        }

        const float scaledDt = m_app.GetTime().ScaledDelta();
        if (m_controlLock <= 0.0f) {
            m_app.GetWorld().UpdateScripts(scaledDt);
        }

        m_spawnTimer -= scaledDt;
        if (m_spawnTimer <= 0.0f && m_enemyCount < m_maxEnemies) {
            m_spawnTimer = 2.8f;
            const float angle = NextFloat() * 6.2831853f;
            const float radius = 12.0f + NextFloat() * 14.0f;
            SpawnEnemy(
                m_app.GetWorld(),
                {std::cos(angle) * radius, 1.0f, std::sin(angle) * radius});
        }

        m_hudTimer += deltaSeconds;
        if (m_hudTimer >= 0.1f) {
            m_hudTimer = 0.0f;
            UpdateHud();
        }

        m_timer += deltaSeconds;
        if (GameSession::CheckWin() || GameSession::CheckLose()) {
            GetManager()->ChangeState("GameOver");
            return;
        }

        if (m_timer >= m_timeLimitSeconds) {
            GameSession::Won() = false;
            GameSession::Finished() = true;
            GetManager()->ChangeState("GameOver");
            return;
        }

        if (m_app.GetInput().WasActionPressed(Engine::Action::Pause)) {
            GameSession::Won() = false;
            GameSession::Finished() = true;
            GetManager()->ChangeState("GameOver");
        }

        if (m_app.GetInput().WasPressed(Engine::Key::Num1)) {
            m_app.SetPostProcess(1.0f, 0.0f, 1.0f);
        } else if (m_app.GetInput().WasPressed(Engine::Key::Num2)) {
            m_app.SetPostProcess(0.15f, 0.05f, 1.05f);
        } else if (m_app.GetInput().WasPressed(Engine::Key::Num3)) {
            m_app.SetPostProcess(1.6f, 0.04f, 1.45f);
        }

        if (m_app.GetInput().WasPressed(Engine::Key::T)) {
            Engine::Time& time = m_app.GetTime();
            if (time.GetTimeScale() > 0.7f) {
                time.SetTimeScale(0.35f);
            } else {
                time.SetTimeScale(1.0f);
                time.ClearSlowMotion();
            }
        }
    }

    void Exit() override {
        m_app.GetUI().Clear();
    }

private:
    unsigned int NextU32() {
        m_seed = m_seed * 1664525u + 1013904223u;
        return m_seed;
    }

    float NextFloat() {
        return static_cast<float>(NextU32() & 0xFFFFFFu) / static_cast<float>(0xFFFFFFu);
    }

    void BuildProceduralLevel(Engine::World& world) {
        // Arena perimeter walls.
        const Engine::Vec3 wallTint{0.12f, 0.42f, 0.72f};
        SpawnObstacle(world, {0.0f, 2.0f, 28.0f}, {30.0f, 4.0f, 1.0f}, false, wallTint, false);
        SpawnObstacle(world, {0.0f, 2.0f, -28.0f}, {30.0f, 4.0f, 1.0f}, false, wallTint, false);
        SpawnObstacle(world, {28.0f, 2.0f, 0.0f}, {1.0f, 4.0f, 28.0f}, false, wallTint, false);
        SpawnObstacle(world, {-28.0f, 2.0f, 0.0f}, {1.0f, 4.0f, 28.0f}, false, wallTint, false);

        // Full checkerboard floor (two tones).
        for (int z = -4; z <= 4; ++z) {
            for (int x = -4; x <= 4; ++x) {
                const bool bright = ((x + z) & 1) != 0;
                SpawnObstacle(
                    world,
                    {static_cast<float>(x) * 5.0f, -0.45f, static_cast<float>(z) * 5.0f},
                    {2.35f, 0.16f, 2.35f},
                    false,
                    bright ? Engine::Vec3{0.10f, 0.38f, 0.58f} : Engine::Vec3{0.04f, 0.14f, 0.26f},
                    false);
            }
        }

        // Scatter shootable obstacles (keep clear area around player spawn).
        constexpr float kPlayerSpawnX = 0.0f;
        constexpr float kPlayerSpawnZ = -4.0f;
        constexpr float kClearRadius = 5.5f;
        for (int i = 0; i < 18; ++i) {
            const float x = (NextFloat() * 2.0f - 1.0f) * 22.0f;
            const float z = (NextFloat() * 2.0f - 1.0f) * 22.0f;
            const float dx = x - kPlayerSpawnX;
            const float dz = z - kPlayerSpawnZ;
            if ((dx * dx + dz * dz) < (kClearRadius * kClearRadius)) {
                continue;
            }
            const bool rotating = (NextU32() % 4u) == 0u;
            const float size = 0.7f + NextFloat() * 1.4f;
            SpawnObstacle(
                world,
                {x, size * 0.5f, z},
                {size, size, size},
                rotating,
                rotating ? Engine::Vec3{0.45f, 1.0f, 1.0f} : Engine::Vec3{0.25f, 0.78f, 0.92f});
        }
    }

    void SpawnObstacle(
        Engine::World& world,
        const Engine::Vec3& position,
        const Engine::Vec3& size,
        bool rotating,
        const Engine::Vec3& tint,
        bool withFx = true) {
        const Engine::Entity entity = world.CreateEntity();

        Engine::TransformComponent transform;
        transform.local.position = position;
        transform.local.scale = size;
        world.AddComponent<Engine::TransformComponent>(entity, transform);

        Engine::MeshComponent meshComponent;
        meshComponent.mesh = Engine::Mesh::CreateCube(m_app.GetDevice(), 1.0f);
        meshComponent.tint = tint;
        world.AddComponent<Engine::MeshComponent>(entity, std::move(meshComponent));

        Engine::ColliderComponent collider;
        collider.shape = Engine::ColliderShape::AABB;
        collider.halfExtents = size * 0.5f;
        collider.isStatic = true;
        world.AddComponent<Engine::ColliderComponent>(entity, collider);
        world.AddComponent<Engine::TagComponent>(entity, Engine::TagComponent{Engine::EntityTag::Obstacle});

        if (!withFx) {
            return;
        }

        Engine::ParticleEmitterComponent sparks;
        sparks.maxParticles = 96;
        sparks.emissionRate = 0.0f;
        sparks.startColor = {1.0f, 0.75f, 0.25f};
        sparks.endColor = {1.0f, 0.15f, 0.05f};
        sparks.minSpeed = 1.5f;
        sparks.maxSpeed = 5.0f;
        sparks.directionJitter = 0.9f;
        world.AddComponent<Engine::ParticleEmitterComponent>(entity, std::move(sparks));

        if (rotating) {
            world.AddScript(entity, std::make_unique<RotateCubeScript>());
        } else {
            world.AddScript(entity, std::make_unique<CollisionFlashScript>());
        }
    }

    void SpawnEnemy(Engine::World& world, const Engine::Vec3& position) {
        if (m_enemyCount >= m_maxEnemies) {
            return;
        }

        const Engine::Entity entity = world.CreateEntity();

        Engine::TransformComponent transform;
        transform.local.position = position;
        transform.local.scale = {0.95f, 1.45f, 0.95f};
        world.AddComponent<Engine::TransformComponent>(entity, transform);

        Engine::MeshComponent mesh;
        mesh.mesh = Engine::Mesh::CreateCube(m_app.GetDevice(), 1.0f);
        mesh.tint = {1.0f, 0.18f, 0.42f};
        world.AddComponent<Engine::MeshComponent>(entity, std::move(mesh));

        Engine::ColliderComponent collider;
        collider.shape = Engine::ColliderShape::AABB;
        collider.halfExtents = {0.48f, 0.72f, 0.48f};
        collider.isStatic = false;
        collider.isTrigger = false;
        world.AddComponent<Engine::ColliderComponent>(entity, collider);
        world.AddComponent<Engine::TagComponent>(entity, Engine::TagComponent{Engine::EntityTag::Enemy});

        Engine::ParticleEmitterComponent sparks;
        sparks.maxParticles = 128;
        sparks.emissionRate = 0.0f;
        sparks.startColor = {1.0f, 0.35f, 0.55f};
        sparks.endColor = {0.9f, 0.05f, 0.2f};
        sparks.minSpeed = 2.0f;
        sparks.maxSpeed = 6.0f;
        sparks.directionJitter = 1.0f;
        world.AddComponent<Engine::ParticleEmitterComponent>(entity, std::move(sparks));

        world.AddScript(entity, std::make_unique<EnemyScript>());
        world.StartScripts();
        ++m_enemyCount;
    }

    void UpdateHud() {
        // Refresh live enemy count from tags (destroyed enemies decrease count).
        m_enemyCount = 0;
        for (const auto& [entity, tag] : m_app.GetWorld().Components<Engine::TagComponent>()) {
            if (tag.tag == Engine::EntityTag::Enemy && m_app.GetWorld().IsAlive(entity)) {
                ++m_enemyCount;
            }
        }

        char scoreLine[64] = {};
        std::snprintf(scoreLine, sizeof(scoreLine), "SCORE:%d/%d", GameSession::Score(), GameSession::ScoreTarget());
        m_app.GetUI().SetText("score", scoreLine, 24.0f, 20.0f, 3.5f, {1.0f, 0.92f, 0.35f});

        char hpLine[64] = {};
        std::snprintf(hpLine, sizeof(hpLine), "HP:%d  KILLS:%d/%d", GameSession::Health(), GameSession::Kills(), GameSession::KillTarget());
        m_app.GetUI().SetText("hp", hpLine, 24.0f, 56.0f, 3.0f, {0.35f, 1.0f, 0.65f});

        char goalLine[96] = {};
        std::snprintf(
            goalLine,
            sizeof(goalLine),
            "TIME:%.0fs  ENEMIES:%d",
            m_timeLimitSeconds - m_timer,
            m_enemyCount);
        m_app.GetUI().SetText("goal", goalLine, 24.0f, 92.0f, 2.0f, {0.55f, 0.95f, 1.0f});

        float sat = 1.0f;
        float bri = 0.0f;
        float con = 1.0f;
        m_app.GetPostProcess(sat, bri, con);
        char ppLine[120] = {};
        std::snprintf(
            ppLine,
            sizeof(ppLine),
            "POST S:%.2f C:%.2f  TIME:%.2fx%s",
            sat,
            con,
            m_app.GetTime().GetTimeScale(),
            m_app.GetTime().IsSlowMotionActive() ? " SLOW" : "");
        m_app.GetUI().SetText("pp", ppLine, 24.0f, 152.0f, 2.0f, {0.7f, 0.55f, 1.0f});

        char statsLine[96] = {};
        std::snprintf(
            statsLine,
            sizeof(statsLine),
            "DRAW:%u  CULL:%u",
            m_app.GetLastDrawnCount(),
            m_app.GetLastCulledCount());
        m_app.GetUI().SetText("stats", statsLine, 24.0f, 180.0f, 2.0f, {0.5f, 0.7f, 0.85f});
    }

    Engine::Application& m_app;
    float m_timer = 0.0f;
    float m_hudTimer = 0.0f;
    float m_spawnTimer = 0.0f;
    float m_controlLock = 0.0f;
    float m_timeLimitSeconds = 90.0f;
    int m_enemyCount = 0;
    int m_maxEnemies = 6;
    unsigned int m_seed = 1u;
};
