#include "math/Vector3.h"
#include <cmath>

namespace Math {

Vector3 Vector3::operator+(const Vector3& v) const { return { x + v.x, y + v.y, z + v.z }; }
Vector3 Vector3::operator-(const Vector3& v) const { return { x - v.x, y - v.y, z - v.z }; }
Vector3 Vector3::operator*(float s) const          { return { x * s, y * s, z * s }; }
Vector3 Vector3::operator/(float s) const          { return { x / s, y / s, z / s }; }
Vector3 Vector3::operator-() const                 { return { -x, -y, -z }; }

Vector3& Vector3::operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }

// 내적 (dot product)
//   a·b = ax*bx + ay*by + az*bz
//   기하학적 의미: a·b = |a||b|cosθ  →  두 방향이 얼마나 "나란한가".
//   둘 다 단위벡터면 결과는 cosθ 그 자체. 조명 계산의 핵심(Part 4).
float Vector3::Dot(const Vector3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

// 외적 (cross product)
//   a×b = 두 벡터에 동시에 수직인 새 벡터. 면의 법선·좌표축을 만들 때 씀(Part 1.4 카메라).
//   왼손 좌표계: Right × Up = Forward 가 되도록 정의됨.
Vector3 Vector3::Cross(const Vector3& v) const {
    return {
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    };
}

float Vector3::Length() const {
    return std::sqrt(Dot(*this));   // √(x²+y²+z²)
}

Vector3 Vector3::Normalized() const {
    float len = Length();
    if (len <= 1e-8f) return { 0.0f, 0.0f, 0.0f };  // 0벡터는 방향이 없음
    return { x / len, y / len, z / len };
}

} // namespace Math
