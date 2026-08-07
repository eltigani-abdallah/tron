#pragma once

#include "Engine/Export.hpp"

namespace Engine {

class ENGINE_API Time {
public:
    void Tick(float realDeltaSeconds);

    void SetTimeScale(float scale);
    float GetTimeScale() const { return m_timeScale; }

    float RealDelta() const { return m_realDelta; }
    float ScaledDelta() const { return m_scaledDelta; }
    float TotalReal() const { return m_totalReal; }
    float TotalScaled() const { return m_totalScaled; }

    // Temporary slow-mo: overrides scale for a real-time duration.
    void TriggerSlowMotion(float scale, float realDurationSeconds);
    void ClearSlowMotion();
    bool IsSlowMotionActive() const { return m_slowMoRemaining > 0.0f; }

private:
    float m_timeScale = 1.0f;
    float m_realDelta = 0.0f;
    float m_scaledDelta = 0.0f;
    float m_totalReal = 0.0f;
    float m_totalScaled = 0.0f;

    float m_baseScale = 1.0f;
    float m_slowMoScale = 1.0f;
    float m_slowMoRemaining = 0.0f;
};

} // namespace Engine
