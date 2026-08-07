#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace Test {

inline int& Failures() {
    static int failures = 0;
    return failures;
}

inline int& Passed() {
    static int passed = 0;
    return passed;
}

inline void Expect(bool condition, const char* expr, const char* file, int line) {
    if (condition) {
        ++Passed();
        return;
    }
    ++Failures();
    std::fprintf(stderr, "[FAIL] %s:%d  %s\n", file, line, expr);
}

inline void ExpectNear(float a, float b, float eps, const char* expr, const char* file, int line) {
    Expect(std::fabs(a - b) <= eps, expr, file, line);
}

inline int Summary(const char* suiteName) {
    std::printf(
        "%s: %d passed, %d failed\n",
        suiteName,
        Passed(),
        Failures());
    return Failures() == 0 ? 0 : 1;
}

} // namespace Test

#define EXPECT(cond) ::Test::Expect(!!(cond), #cond, __FILE__, __LINE__)
#define EXPECT_NEAR(a, b, eps) ::Test::ExpectNear((a), (b), (eps), #a " ~= " #b, __FILE__, __LINE__)
