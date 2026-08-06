#pragma once
#include <cstdint>
#include <windows.h>

class TimeSystem
{
public:
    TimeSystem();

    void Update();                  // calcule dt, fps, totalTime
    bool ShouldRunFixedUpdate();    // physique à pas fixe

    float GetDeltaTime() const      { return deltaTime; }
    float GetFixedDelta() const     { return fixedDelta; }
    float GetFPS() const            { return fps; }
    float GetTotalTime() const      { return totalTime; }

private:
    double lastTime;
    double currentTime;

    float deltaTime;
    float totalTime;
    float fps;

    // Fixed-step
    float fixedDelta;   // ex: 1/60 = 0.01666s
    float accumulator;

    LARGE_INTEGER frequency;
};
