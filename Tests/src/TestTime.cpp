#include "TestAssert.hpp"

#include "Engine/Time/Time.hpp"

void TestTime() {
    Engine::Time time;

    time.Tick(0.016f);
    EXPECT_NEAR(time.RealDelta(), 0.016f, 0.0001f);
    EXPECT_NEAR(time.ScaledDelta(), 0.016f, 0.0001f);

    time.SetTimeScale(0.5f);
    time.Tick(0.020f);
    EXPECT_NEAR(time.ScaledDelta(), 0.010f, 0.0001f);

    // Clamp huge spikes.
    time.SetTimeScale(1.0f);
    time.Tick(5.0f);
    EXPECT_NEAR(time.RealDelta(), 0.1f, 0.0001f);

    time.TriggerSlowMotion(0.25f, 0.2f);
    EXPECT(time.IsSlowMotionActive());
    time.Tick(0.05f);
    EXPECT_NEAR(time.GetTimeScale(), 0.25f, 0.0001f);
    EXPECT_NEAR(time.ScaledDelta(), 0.05f * 0.25f, 0.0001f);

    // Remaining was 0.15 after first tick (duration 0.2 - 0.05); need >= 0.15 more real time.
    time.Tick(0.1f);
    time.Tick(0.1f);
    EXPECT(!time.IsSlowMotionActive());
    EXPECT_NEAR(time.GetTimeScale(), 1.0f, 0.0001f);
}
