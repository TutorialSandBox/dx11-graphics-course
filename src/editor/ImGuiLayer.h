#pragma once

#include <Windows.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace editor {

// =============================================================================
// ImGuiLayer — Dear ImGui(디버그 UI)를 엔진에 붙이는 얇은 래퍼.
//   백엔드(win32 + dx11) 초기화/프레임/렌더/종료와 메시지 후크를 한곳에 모읍니다.
//   엔진 코드는 ImGui 백엔드 함수를 직접 부르지 않고 이 클래스만 씁니다(격리).
// =============================================================================
class ImGuiLayer {
public:
    void Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
    void Shutdown();

    void BeginFrame();   // 새 UI 프레임 시작 (이후 ImGui:: 호출 가능)
    void Render();       // 쌓인 UI를 현재 렌더 타깃에 그림 (씬 위에)

    // Win32 메시지를 ImGui에 전달. UI가 입력을 소비하면 true.
    bool ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // UI가 마우스/키보드를 점유 중인가? (카메라 조작과 충돌 방지에 사용)
    bool WantCaptureMouse() const;
    bool WantCaptureKeyboard() const;

private:
    bool m_initialized = false;
};

} // namespace editor
