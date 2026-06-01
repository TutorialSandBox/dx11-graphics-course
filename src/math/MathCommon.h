#pragma once

// =============================================================================
// MathCommon.h — 강의 전체에서 쓰는 수학 상수/도우미
//   규약: 왼손 좌표계(LH), 행우선 저장(row-major), 행벡터(v * M)
// =============================================================================

#include <cmath>

namespace Math {

constexpr float PI = 3.14159265358979323846f;

// 각도 변환. 삼각함수(sin/cos)는 라디안을 받으므로 자주 쓰게 됩니다.
inline float ToRadians(float degrees) { return degrees * (PI / 180.0f); }
inline float ToDegrees(float radians) { return radians * (180.0f / PI); }

// 부동소수점은 == 비교가 위험합니다(0.1+0.2 != 0.3). "거의 같은가"로 비교하세요.
inline bool NearlyEqual(float a, float b, float eps = 1e-5f) {
    return std::fabs(a - b) <= eps;
}

} // namespace Math
