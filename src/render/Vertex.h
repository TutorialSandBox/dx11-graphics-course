#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"

namespace render {

// 3D 메시 정점 포맷: 위치 + 법선 + 텍스처 좌표(UV).
//   메모리 배치 (총 32바이트):
//     position : offset 0   (12B)
//     normal   : offset 12  (12B)   — 조명 계산용 (Part 4.3)
//     uv       : offset 24  (8B)    — 텍스처 입히기용 (Part 4.2)
struct Vertex {
    Math::Vector3 position;
    Math::Vector3 normal;
    Math::Vector2 uv;
};

} // namespace render
