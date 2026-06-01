#include "editor/ImGuiLayer.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// ImGui win32 백엔드가 제공하는 메시지 처리기 (선언만 가져옴)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace editor {

void ImGuiLayer::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // 키보드 내비게이션
    io.IniFilename = nullptr;                               // imgui.ini 저장 안 함(단순화)

    // 한글 표시: Windows 맑은 고딕을 한글 글리프 범위로 로드.
    //   (파일이 없으면 nullptr 반환 → 기본 폰트 사용, 한글은 네모로 보임)
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f,
                                 nullptr, io.Fonts->GetGlyphRangesKorean());

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);                // 입력/창 백엔드
    ImGui_ImplDX11_Init(device, context);      // 렌더 백엔드
    m_initialized = true;
}

void ImGuiLayer::Shutdown() {
    if (!m_initialized) return;
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    m_initialized = false;
}

void ImGuiLayer::BeginFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::Render() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());   // 현재 바인딩된 RTV에 그림
}

bool ImGuiLayer::ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (!m_initialized) return false;
    return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam) != 0;
}

bool ImGuiLayer::WantCaptureMouse() const {
    return m_initialized && ImGui::GetIO().WantCaptureMouse;
}
bool ImGuiLayer::WantCaptureKeyboard() const {
    return m_initialized && ImGui::GetIO().WantCaptureKeyboard;
}

} // namespace editor
