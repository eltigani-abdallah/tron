#include "TestAssert.hpp"

void TestMath();
void TestTime();
void TestSpatialPartition();
void TestWorld();
void TestGameSession();

int main() {
    TestMath();
    TestTime();
    TestSpatialPartition();
    TestWorld();
    TestGameSession();
    return Test::Summary("Tron Tests");
}
