// =============================================================================
// math_demo.cpp — Part 1에서 만든 수학 라이브러리가 정말 동작하는지 눈으로 확인.
//   레슨이 진행되며 이 파일도 함께 자랍니다. (lesson-1.1: 벡터)
// =============================================================================

#include "math/Vector3.h"
#include "math/MathCommon.h"
#include <cstdio>

using Math::Vector3;

static void Print(const char* label, const Vector3& v) {
    std::printf("  %-12s = (% .3f, % .3f, % .3f)\n", label, v.x, v.y, v.z);
}

int main() {
    std::printf("=== Part 1.1  벡터 ===\n\n");

    Vector3 a{ 1, 2, 3 };
    Vector3 b{ 4, 5, 6 };

    std::printf("[더하기/빼기/스칼라배]\n");
    Print("a", a);
    Print("b", b);
    Print("a + b", a + b);
    Print("b - a", b - a);
    Print("a * 2", a * 2.0f);

    std::printf("\n[내적 (Dot) — 두 벡터가 얼마나 나란한가]\n");
    std::printf("  a . b = %.3f\n", a.Dot(b));
    // 단위벡터끼리의 내적은 cosθ. 같은 방향이면 1, 수직이면 0, 반대면 -1.
    std::printf("  Right . Up      = %.3f  (수직이라 0)\n", Vector3::Right().Dot(Vector3::Up()));
    std::printf("  Right . Right   = %.3f  (같은 방향이라 1)\n", Vector3::Right().Dot(Vector3::Right()));
    std::printf("  Right . (-Right)= %.3f  (반대 방향이라 -1)\n", Vector3::Right().Dot(-Vector3::Right()));

    std::printf("\n[외적 (Cross) — 두 벡터에 동시에 수직]\n");
    // 왼손 좌표계: Right × Up = Forward
    Print("Right x Up", Vector3::Right().Cross(Vector3::Up()));
    std::printf("  (기대값: Forward = (0,0,1))\n");

    std::printf("\n[길이와 정규화]\n");
    Vector3 v{ 3, 4, 0 };
    Print("v", v);
    std::printf("  |v| = %.3f  (3,4,0의 길이는 5)\n", v.Length());
    Print("v.Normalized", v.Normalized());
    std::printf("  |normalized| = %.3f  (정규화하면 길이 1)\n", v.Normalized().Length());

    std::printf("\n완료.\n");
    return 0;
}
