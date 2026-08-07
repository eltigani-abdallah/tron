#pragma once

#include "Engine/Core/Application.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/States/State.hpp"
#include "Engine/States/StateManager.hpp"
#include "Engine/UI/UISystem.hpp"
#include "GameScore.hpp"

#include <cstdio>

class GameOverState : public Engine::State {
public:
    explicit GameOverState(Engine::Application& app)
        : m_app(app) {
    }

    void Enter() override {
        const bool won = GameSession::Won();
        m_app.SetWindowTitle(won ? "TRON - Victory" : "TRON - Game Over");
        if (won) {
            m_app.SetClearColor(0.02f, 0.1f, 0.08f);
        } else {
            m_app.SetClearColor(0.12f, 0.02f, 0.04f);
        }

        Engine::UISystem& ui = m_app.GetUI();
        ui.Clear();
        ui.SetVisible(true);

        const float cx = static_cast<float>(m_app.GetWidth()) * 0.5f;
        const float cy = static_cast<float>(m_app.GetHeight()) * 0.5f;

        if (won) {
            ui.SetText("over", "VICTORY", cx - 100.0f, cy - 80.0f, 5.0f, {0.35f, 1.0f, 0.55f});
        } else {
            ui.SetText("over", "GAME OVER", cx - 120.0f, cy - 80.0f, 5.0f, {1.0f, 0.35f, 0.35f});
        }

        char scoreLine[64] = {};
        std::snprintf(scoreLine, sizeof(scoreLine), "SCORE:%d", GameSession::Score());
        ui.SetText("score", scoreLine, cx - 90.0f, cy - 10.0f, 4.0f, {1.0f, 0.9f, 0.4f});

        char killsLine[64] = {};
        std::snprintf(killsLine, sizeof(killsLine), "KILLS:%d", GameSession::Kills());
        ui.SetText("kills", killsLine, cx - 90.0f, cy + 40.0f, 3.5f, {0.7f, 0.9f, 1.0f});

        ui.SetText("hint", "SPACE MENU   ESC QUITTER", cx - 160.0f, cy + 100.0f, 2.0f, {0.75f, 0.85f, 1.0f});
        ui.SetButton(
            "menu",
            "MENU",
            Engine::UIRect{cx - 100.0f, cy + 140.0f, 200.0f, 52.0f},
            {0.05f, 0.4f, 0.55f});
    }

    void Update(float) override {
        Engine::InputManager& input = m_app.GetInput();
        Engine::UISystem& ui = m_app.GetUI();

        if (ui.WasClicked("menu") || input.WasActionPressed(Engine::Action::Confirm)) {
            GetManager()->ChangeState("Menu");
            return;
        }

        if (input.WasActionPressed(Engine::Action::Pause)) {
            m_app.RequestQuit();
        }
    }

    void Exit() override {
        m_app.GetUI().Clear();
    }

private:
    Engine::Application& m_app;
};
