#pragma once

// =============================================================================
// Vector3 — 3차원 벡터. 그래픽스에서 가장 많이 쓰는 타입.
//   위치(점), 방향, 색(rgb), 법선 등 거의 모든 것을 표현합니다.
//   좌표계: 왼손(LH) — +X 오른쪽, +Y 위, +Z 화면 안쪽.
// =============================================================================

namespace Math {

struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3() = default;
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    // ---- 기본 연산 (성분별) ----
    Vector3 operator+(const Vector3& v) const;
    Vector3 operator-(const Vector3& v) const;
    Vector3 operator*(float s) const;        // 스칼라배 (확대/축소)
    Vector3 operator/(float s) const;
    Vector3 operator-() const;               // 반대 방향

    Vector3& operator+=(const Vector3& v);

    // ---- 그래픽스 핵심 연산 ----
    float   Dot(const Vector3& v) const;        // 내적: 두 벡터가 "얼마나 같은 방향인가"
    Vector3 Cross(const Vector3& v) const;      // 외적: 두 벡터에 동시에 수직인 벡터
    float   Length() const;                     // 길이(크기)
    Vector3 Normalized() const;                 // 같은 방향, 길이 1 (방향만 필요할 때)

    // ---- 자주 쓰는 상수 방향 (왼손 좌표계 기준) ----
    static Vector3 Zero()    { return { 0, 0, 0 }; }
    static Vector3 One()     { return { 1, 1, 1 }; }
    static Vector3 Right()   { return { 1, 0, 0 }; }   // +X
    static Vector3 Up()      { return { 0, 1, 0 }; }   // +Y
    static Vector3 Forward() { return { 0, 0, 1 }; }   // +Z (화면 안쪽)
};

// 스칼라가 왼쪽일 때도 되도록 (예: 2.0f * v)
inline Vector3 operator*(float s, const Vector3& v) { return v * s; }

} // namespace Math
