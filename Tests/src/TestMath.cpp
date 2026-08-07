#include "TestAssert.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"

void TestMath() {
    using namespace Engine;

    EXPECT_NEAR(ToRadians(180.0f), kPi, 0.0001f);
    EXPECT_NEAR(ToDegrees(kPi * 0.5f), 90.0f, 0.0001f);
    EXPECT(Clamp(5, 0, 3) == 3);
    EXPECT(Clamp(-1, 0, 3) == 0);

    const Vec3 a{3.0f, 0.0f, 4.0f};
    EXPECT_NEAR(a.Length(), 5.0f, 0.0001f);

    const Vec3 n = a.Normalized();
    EXPECT_NEAR(n.Length(), 1.0f, 0.0001f);
    EXPECT_NEAR(n.x, 0.6f, 0.0001f);
    EXPECT_NEAR(n.z, 0.8f, 0.0001f);

    EXPECT(Vec3::Zero().Normalized().LengthSquared() == 0.0f);

    const Vec3 sum = Vec3::Right() + Vec3::Up();
    EXPECT_NEAR(sum.x, 1.0f, 0.0001f);
    EXPECT_NEAR(sum.y, 1.0f, 0.0001f);
}
