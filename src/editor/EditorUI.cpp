#include "editor/EditorUI.h"
#include "imgui.h"

namespace editor {

// 패널 = 함수 하나. 정보를 받아 ImGui 위젯으로 표시한다.
void DrawEditor(const FrameInfo& info) {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(340, 180), ImGuiCond_FirstUseEver);

    ImGui::Begin("MiniEngine");
    ImGui::Text("DX11 미니엔진 (Forward 렌더링)");
    ImGui::Separator();
    ImGui::Text("FPS : %.1f  (%.2f ms)", info.fps, info.frameMs);
    ImGui::Text("Cam : %.2f, %.2f, %.2f",
                info.cameraPos.x, info.cameraPos.y, info.cameraPos.z);
    ImGui::Separator();
    ImGui::TextWrapped("조작: 우클릭 드래그 = 시점 회전, WASD = 이동, "
                       "E/Q = 상하, Shift = 가속, ESC = 종료");
    ImGui::End();
}

} // namespace editor
