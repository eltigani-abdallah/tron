#pragma once

#include "Engine/Core/Application.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/States/State.hpp"
#include "Engine/States/StateManager.hpp"
#include "Engine/UI/UISystem.hpp"

class MenuState : public Engine::State {
public:
    explicit MenuState(Engine::Application& app)
        : m_app(app) {
    }

    void Enter() override {
        m_app.SetWindowTitle("TRON - Menu");
        m_app.SetClearColor(0.03f, 0.08f, 0.14f);
        m_app.GetWorld().Clear();

        Engine::UISystem& ui = m_app.GetUI();
        ui.Clear();
        ui.SetVisible(true);

        const float cx = static_cast<float>(m_app.GetWidth()) * 0.5f;
        const float cy = static_cast<float>(m_app.GetHeight()) * 0.5f;
        ui.SetText("title", "TRON", cx - 70.0f, cy - 120.0f, 6.0f, {0.3f, 0.95f, 1.0f});
        ui.SetText("hint", "SPACE OU BOUTON", cx - 120.0f, cy - 50.0f, 2.5f, {0.7f, 0.85f, 1.0f});
        ui.SetText("goal", "6 KILLS OU SCORE 80", cx - 140.0f, cy - 20.0f, 2.0f, {0.5f, 0.9f, 1.0f});
        ui.SetButton(
            "play",
            "JOUER",
            Engine::UIRect{cx - 100.0f, cy, 200.0f, 56.0f},
            {0.05f, 0.45f, 0.65f});
    }

    void Update(float) override {
        Engine::InputManager& input = m_app.GetInput();
        Engine::UISystem& ui = m_app.GetUI();

        if (ui.WasClicked("play") || input.WasActionPressed(Engine::Action::Confirm)) {
            GetManager()->ChangeState("Game");
        }
    }

    void Exit() override {
        m_app.GetUI().Clear();
    }

private:
    Engine::Application& m_app;
};
