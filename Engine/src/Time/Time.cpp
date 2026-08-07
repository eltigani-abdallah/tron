#include "Engine/Time/Time.hpp"

#include <algorithm>

namespace Engine {

void Time::Tick(float realDeltaSeconds) {
    m_realDelta = std::max(0.0f, std::min(realDeltaSeconds, 0.1f));

    if (m_slowMoRemaining > 0.0f) {
        m_timeScale = m_slowMoScale;
        m_slowMoRemaining -= m_realDelta;
        if (m_slowMoRemaining <= 0.0f) {
            m_slowMoRemaining = 0.0f;
            m_timeScale = m_baseScale;
        }
    } else {
        m_timeScale = m_baseScale;
    }

    if (m_timeScale < 0.0f) {
        m_timeScale = 0.0f;
    }

    m_scaledDelta = m_realDelta * m_timeScale;
    m_totalReal += m_realDelta;
    m_totalScaled += m_scaledDelta;
}

void Time::SetTimeScale(float scale) {
    m_baseScale = scale < 0.0f ? 0.0f : scale;
    if (m_slowMoRemaining <= 0.0f) {
        m_timeScale = m_baseScale;
    }
}

void Time::TriggerSlowMotion(float scale, float realDurationSeconds) {
    m_slowMoScale = scale < 0.0f ? 0.0f : scale;
    m_slowMoRemaining = realDurationSeconds > 0.0f ? realDurationSeconds : 0.0f;
    if (m_slowMoRemaining > 0.0f) {
        m_timeScale = m_slowMoScale;
    }
}

void Time::ClearSlowMotion() {
    m_slowMoRemaining = 0.0f;
    m_timeScale = m_baseScale;
}

} // namespace Engine
