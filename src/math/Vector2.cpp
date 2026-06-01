#include "math/Vector2.h"
#include <cmath>

namespace Math {

Vector2 Vector2::operator+(const Vector2& v) const { return { x + v.x, y + v.y }; }
Vector2 Vector2::operator-(const Vector2& v) const { return { x - v.x, y - v.y }; }
Vector2 Vector2::operator*(float s) const          { return { x * s, y * s }; }

float Vector2::Dot(const Vector2& v) const { return x * v.x + y * v.y; }

float Vector2::Length() const { return std::sqrt(Dot(*this)); }

Vector2 Vector2::Normalized() const {
    float len = Length();
    if (len <= 1e-8f) return { 0.0f, 0.0f };   // 0벡터는 방향이 없음 → 그대로 0
    return { x / len, y / len };
}

} // namespace Math
