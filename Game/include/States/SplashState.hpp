#pragma once

#include "Engine/Core/Application.hpp"
#include "Engine/States/State.hpp"
#include "Engine/States/StateManager.hpp"
#include "Engine/UI/UISystem.hpp"

class SplashState : public Engine::State {
public:
    explicit SplashState(Engine::Application& app)
        : m_app(app) {
    }

    void Enter() override {
        m_timer = 0.0f;
        m_app.SetWindowTitle("TRON - Splash");
        m_app.SetClearColor(0.01f, 0.02f, 0.06f);
        m_app.GetWorld().Clear();

        Engine::UISystem& ui = m_app.GetUI();
        ui.Clear();
        ui.SetVisible(true);
        const float cx = static_cast<float>(m_app.GetWidth()) * 0.5f - 80.0f;
        const float cy = static_cast<float>(m_app.GetHeight()) * 0.5f - 20.0f;
        ui.SetText("title", "TRON", cx, cy, 6.0f, {0.3f, 0.95f, 1.0f});
        ui.SetText("sub", "ENGINE DEMO", cx - 40.0f, cy + 50.0f, 3.0f, {0.6f, 0.8f, 1.0f});
    }

    void Update(float deltaSeconds) override {
        m_timer += deltaSeconds;
        if (m_timer >= 2.0f) {
            GetManager()->ChangeState("Menu");
        }
    }

    void Exit() override {
        m_app.GetUI().Clear();
    }

private:
    Engine::Application& m_app;
    float m_timer = 0.0f;
};
