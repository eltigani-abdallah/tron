#include "Engine/Core/Application.hpp"
#include "Engine/States/StateManager.hpp"
#include "States/GameOverState.hpp"
#include "States/GameState.hpp"
#include "States/MenuState.hpp"
#include "States/SplashState.hpp"

#include <exception>
#include <iostream>
#include <memory>

int main() {
    try {
        Engine::Application app;

        Engine::ApplicationConfig config;
        config.windowTitle = "TRON";
        config.width = 1280;
        config.height = 720;

        if (!app.Initialize(config)) {
            std::cerr << "Echec de l'initialisation du moteur.\n";
            return 1;
        }

        Engine::StateManager& states = app.GetStateManager();
        states.Register("Splash", std::make_unique<SplashState>(app));
        states.Register("Menu", std::make_unique<MenuState>(app));
        states.Register("Game", std::make_unique<GameState>(app));
        states.Register("GameOver", std::make_unique<GameOverState>(app));
        states.ChangeState("Splash");

        app.Run();
        app.Shutdown();
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << '\n';
        return 1;
    }
}
