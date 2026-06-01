// =============================================================================
// math_demo.cpp — Part 1에서 만든 수학 라이브러리가 정말 동작하는지 눈으로 확인.
//   레슨이 진행되며 이 파일도 함께 자랍니다. (lesson-1.1: 벡터)
// =============================================================================

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix.h"
#include "math/MathCommon.h"
#include <cstdio>

using Math::Vector3;
using Math::Vector4;
using Math::Matrix;

static void Print(const char* label, const Vector3& v) {
    std::printf("  %-12s = (% .3f, % .3f, % .3f)\n", label, v.x, v.y, v.z);
}

static void Print4(const char* label, const Vector4& v) {
    std::printf("  %-12s = (% .3f, % .3f, % .3f, % .3f)\n", label, v.x, v.y, v.z, v.w);
}

static void PrintMatrix(const char* label, const Matrix& m) {
    std::printf("  %s =\n", label);
    for (int i = 0; i < 4; ++i)
        std::printf("    [% .2f % .2f % .2f % .2f]\n", m.m[i][0], m.m[i][1], m.m[i][2], m.m[i][3]);
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

    // =====================================================================
    std::printf("\n\n=== Part 1.2  행렬 ===\n\n");

    std::printf("[항등 행렬 — 아무것도 바꾸지 않음]\n");
    Matrix I = Matrix::Identity();
    PrintMatrix("I", I);
    Vector4 p{ 1, 2, 3, 1 };          // w=1 → 위치(점)
    Print4("p", p);
    Print4("p * I", p * I);           // 그대로 나와야 함
    std::printf("  (p * I == p : 항등 행렬은 곱셈의 1)\n");

    std::printf("\n[전치 — 행과 열을 맞바꿈]\n");
    Matrix A{};                        // 항등 행렬에서 시작
    A._11 = 1; A._12 = 2; A._13 = 3; A._14 = 4;   // 첫 행을 1,2,3,4로 바꿔 전치를 눈으로 확인
    PrintMatrix("A", A);
    PrintMatrix("Transpose(A)", Matrix::Transpose(A));
    std::printf("  (A의 첫 '행' 1,2,3,4 가 Transpose(A)의 첫 '열'이 됨)\n");

    // =====================================================================
    std::printf("\n\n=== Part 1.3  변환 행렬 ===\n\n");

    Vector3 origin{ 0, 0, 0 };
    Vector3 px{ 1, 0, 0 };            // +X 위의 점

    std::printf("[이동 (Translation)]\n");
    Matrix T = Matrix::Translation({ 5, 0, 0 });
    Print("점 (0,0,0)", origin);
    Print("→ T(+5,0,0)", Math::TransformPoint(origin, T));   // (5,0,0)

    std::printf("\n[스케일 (Scaling)]\n");
    Matrix S = Matrix::Scaling(2.0f);
    Print("점 (1,0,0)", px);
    Print("→ S(x2)", Math::TransformPoint(px, S));           // (2,0,0)

    std::printf("\n[회전 (RotationZ 90도)]\n");
    Matrix Rz = Matrix::RotationZ(Math::ToRadians(90.0f));
    Print("점 (1,0,0)", px);
    Print("→ Rz(90)", Math::TransformPoint(px, Rz));         // ≈ (0,1,0)
    std::printf("  (+X 위의 점이 +Y로 회전)\n");

    std::printf("\n[합성: 먼저 회전, 그 다음 이동  (Rz * T)]\n");
    // 행벡터 규약: v * (Rz * T) = (v * Rz) * T  →  왼쪽부터 차례로 적용
    Matrix RzT = Rz * T;
    Print("점 (1,0,0)", px);
    Print("→ Rz then T", Math::TransformPoint(px, RzT));     // 회전(0,1,0) 후 +5 → (5,1,0)
    std::printf("  (순서 중요! T * Rz 와 결과가 다름)\n");
    Print("→ T then Rz", Math::TransformPoint(px, T * Rz));  // 이동(6,0,0) 후 회전 → (0,6,0)

    std::printf("\n[방향은 이동에 영향 안 받음]\n");
    Print("방향 (1,0,0)", Math::TransformDirection(px, T));  // 이동해도 (1,0,0) 그대로
    std::printf("  (TransformDirection 은 w=0 이라 이동 무시)\n");

    std::printf("\n완료.\n");
    return 0;
}
