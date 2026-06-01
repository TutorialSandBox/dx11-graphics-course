#pragma once

#include "math/Vector3.h"

namespace render {

// 점광원 하나: 위치 + 반경(이 거리 밖이면 영향 0) + 색.
struct PointLight {
    Math::Vector3 position;
    float         radius = 1.0f;
    Math::Vector3 color;
};

// 라이팅 패스에 넘길 빛 정보(엔진 측 표현).
struct LightingData {
    Math::Vector3 eyePos;          // 카메라 위치
    Math::Vector3 dirLightDir;     // "빛으로 향하는" 단위 방향
    Math::Vector3 dirLightColor;   // 방향광 색
    PointLight    points[4];       // 점광원 (최대 4개)
    int           numPoints = 0;   // 켜진 개수
};

} // namespace render
