#pragma once

// =============================================================================
// Vector2 — 2차원 벡터. 주로 텍스처 좌표(UV)에 씁니다.
// =============================================================================

namespace Math {

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;

    Vector2() = default;
    Vector2(float x_, float y_) : x(x_), y(y_) {}

    // 더하기/빼기/스칼라배 — 성분별 연산
    Vector2 operator+(const Vector2& v) const;
    Vector2 operator-(const Vector2& v) const;
    Vector2 operator*(float s) const;

    float Dot(const Vector2& v) const;     // 내적
    float Length() const;                  // 길이(크기)
    Vector2 Normalized() const;            // 같은 방향, 길이 1
};

} // namespace Math
