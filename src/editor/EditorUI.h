#pragma once

#include "math/Vector3.h"

namespace editor {

// 매 프레임 UI에 표시할 정보 묶음.
struct FrameInfo {
    float fps = 0.0f;
    float frameMs = 0.0f;
    Math::Vector3 cameraPos;
};

// ImGui 패널을 그린다. (BeginFrame 과 Render 사이에서 호출)
void DrawEditor(const FrameInfo& info);

} // namespace editor
