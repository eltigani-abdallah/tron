#include "TimeSystem.hpp"

TimeSystem::TimeSystem()
{
    // Récupère la fréquence du timer haute précision
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);

    lastTime = static_cast<double>(t.QuadPart);
    currentTime = lastTime;

    deltaTime = 0.0f;
    totalTime = 0.0f;
    fps = 0.0f;

    fixedDelta = 1.0f / 60.0f;  // physique à 60 Hz
    accumulator = 0.0f;
}

void TimeSystem::Update()
{
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);

    currentTime = static_cast<double>(t.QuadPart);

    // Calcul du dt
    double diff = (currentTime - lastTime) / static_cast<double>(frequency.QuadPart);
    deltaTime = static_cast<float>(diff);

    lastTime = currentTime;

    // Temps total
    totalTime += deltaTime;

    // FPS
    if (deltaTime > 0.0f)
        fps = 1.0f / deltaTime;

    // Accumulateur pour le fixed-step
    accumulator += deltaTime;
}

bool TimeSystem::ShouldRunFixedUpdate()
{
    if (accumulator >= fixedDelta)
    {
        accumulator -= fixedDelta;
        return true;
    }
    return false;
}
    