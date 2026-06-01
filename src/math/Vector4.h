#pragma once

#include "math/Vector3.h"

// =============================================================================
// Vector4 — 4차원 벡터.
//   왜 4차원? 3D 점에 w=1을 붙이면 "이동"까지 행렬 곱 하나로 처리할 수 있습니다.
//   (동차 좌표, homogeneous coordinates — docs/00-foundations/0.2 참고)
//     · 위치(점)  → w = 1  (이동의 영향을 받음)
//     · 방향      → w = 0  (이동의 영향을 안 받음)
// =============================================================================

namespace Math {

struct Vector4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    Vector4() = default;
    Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

    // Vector3 + w 로 만들기 (점이면 w=1, 방향이면 w=0)
    Vector4(const Vector3& v, float w_) : x(v.x), y(v.y), z(v.z), w(w_) {}

    Vector3 XYZ() const { return { x, y, z }; }

    // 원근 나눗셈(perspective divide): 클립 공간 → 정규화 좌표.
    //   멀리 있을수록 w가 커져, 나누면 화면에서 작게 보입니다(Part 1.4).
    Vector3 ToVector3DivW() const {
        if (w == 0.0f) return { x, y, z };
        return { x / w, y / w, z / w };
    }
};

} // namespace Math
