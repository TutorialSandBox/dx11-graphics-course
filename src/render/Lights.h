#pragma once

#include "math/Vector3.h"

namespace render {

// 라이팅 패스에 넘길 빛 정보(엔진 측 표현). (7.2에서 점광원이 추가됩니다)
struct LightingData {
    Math::Vector3 eyePos;          // 카메라 위치
    Math::Vector3 dirLightDir;     // "빛으로 향하는" 단위 방향
    Math::Vector3 dirLightColor;   // 방향광 색
};

} // namespace render
