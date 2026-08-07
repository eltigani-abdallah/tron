#include "TestAssert.hpp"

#include "GameScore.hpp"

void TestGameSession() {
    GameSession::Reset();
    EXPECT(GameSession::Score() == 0);
    EXPECT(GameSession::Health() == 100);
    EXPECT(GameSession::Kills() == 0);
    EXPECT(!GameSession::Finished());

    GameSession::AddScore(10);
    EXPECT(GameSession::Score() == 10);
    EXPECT(!GameSession::CheckWin());

    GameSession::DamagePlayer(30);
    EXPECT(GameSession::Health() == 70);
    EXPECT(!GameSession::CheckLose());

    // Win by kills.
    GameSession::Reset();
    for (int i = 0; i < GameSession::KillTarget(); ++i) {
        GameSession::AddKill();
    }
    EXPECT(GameSession::CheckWin());
    EXPECT(GameSession::Won());
    EXPECT(GameSession::Finished());

    // Lose by HP.
    GameSession::Reset();
    GameSession::DamagePlayer(100);
    EXPECT(GameSession::CheckLose());
    EXPECT(!GameSession::Won());
    EXPECT(GameSession::Finished());

    // Win by score target.
    GameSession::Reset();
    GameSession::AddScore(GameSession::ScoreTarget());
    EXPECT(GameSession::CheckWin());
}
