#pragma once

namespace GameSession {

inline int& Score() {
    static int value = 0;
    return value;
}

inline int& Health() {
    static int value = 100;
    return value;
}

inline int& Kills() {
    static int value = 0;
    return value;
}

inline int& KillTarget() {
    static int value = 6;
    return value;
}

inline int& ScoreTarget() {
    static int value = 80;
    return value;
}

inline bool& Won() {
    static bool value = false;
    return value;
}

inline bool& Finished() {
    static bool value = false;
    return value;
}

inline void Reset() {
    Score() = 0;
    Health() = 100;
    Kills() = 0;
    KillTarget() = 6;
    ScoreTarget() = 80;
    Won() = false;
    Finished() = false;
}

inline void AddScore(int amount) {
    Score() += amount;
}

inline void AddKill() {
    ++Kills();
    AddScore(15);
}

inline void DamagePlayer(int amount) {
    if (Finished()) {
        return;
    }
    Health() -= amount;
    if (Health() < 0) {
        Health() = 0;
    }
}

inline bool CheckWin() {
    if (Finished()) {
        return Won();
    }
    if (Kills() >= KillTarget() || Score() >= ScoreTarget()) {
        Won() = true;
        Finished() = true;
        return true;
    }
    return false;
}

inline bool CheckLose() {
    if (Finished()) {
        return !Won();
    }
    if (Health() <= 0) {
        Won() = false;
        Finished() = true;
        return true;
    }
    return false;
}

// Compat Phase J HUD / anciens includes
inline int& Value() { return Score(); }
inline void Add(int amount) { AddScore(amount); }

} // namespace GameSession

namespace GameScore = GameSession;
