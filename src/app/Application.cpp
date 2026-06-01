#include "app/Application.h"
#include "editor/EditorUI.h"
#include "math/MathCommon.h"
#include "math/Vector3.h"
#include <Windows.h>
#include <cmath>

namespace app {

std::wstring Application::ExecutableDir() const {
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    std::wstring path(buf);
    return path.substr(0, path.find_last_of(L"\\/"));
}

bool Application::Initialize() {
    if (!m_window.Create(L"MiniEngine (Part 6) - ImGui 에디터", 1280, 720))
        return false;
    m_window.SetInput(&m_input);
    m_window.SetOnResize([this](uint32_t w, uint32_t h) {
        m_gfx.Resize(w, h);
        m_camera.SetAspect(m_window.Aspect());
    });

    if (!m_gfx.Initialize(m_window.Handle(), m_window.Width(), m_window.Height()))
        return false;

    const std::wstring exeDir = ExecutableDir();
    m_cube.Initialize(m_gfx.Device(), exeDir + L"\\shaders", exeDir + L"\\assets");

    m_camera.SetPerspective(Math::ToRadians(60.0f), m_window.Aspect(), 0.1f, 100.0f);

    // ImGui 초기화 + 윈도우 메시지를 ImGui로도 전달하도록 후크 연결
    m_ui.Initialize(m_window.Handle(), m_gfx.Device(), m_gfx.Context());
    m_window.SetMessageHook([this](HWND h, UINT m, WPARAM w, LPARAM l) {
        return m_ui.ProcessMessage(h, m, w, l);
    });

    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_tickFreq));
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTick));
    m_lastTick = m_startTick;
    return true;
}

void Application::Frame(float dt, float time, bool capture) {
    using Math::Matrix;
    using Math::Vector3;

    // UI 프레임 시작 → 패널 구성 (그리기는 씬 다음에)
    m_ui.BeginFrame();
    editor::FrameInfo fi;
    fi.fps       = dt > 0.0f ? 1.0f / dt : 0.0f;
    fi.frameMs   = dt * 1000.0f;
    fi.cameraPos = m_camera.Eye();
    editor::DrawEditor(fi);

    // 입력 → 카메라. 단, UI가 마우스/키보드를 점유 중이면 카메라 조작 금지.
    if (!m_ui.WantCaptureMouse() && !m_ui.WantCaptureKeyboard())
        m_camController.Update(m_input, m_camera, dt);

    m_gfx.ClearBackbuffer(0.10f, 0.12f, 0.18f);   // 배경 + 깊이 버퍼 클리어

    Matrix model = Matrix::RotationY(time * 0.3f);
    Matrix mvp   = model * m_camera.ViewProj();
    Vector3 lightDir = Vector3(0.4f, 0.8f, 0.3f).Normalized();
    m_cube.Render(m_gfx.Context(), mvp, model, lightDir);

    m_ui.Render();   // 씬 위에 UI 합성

    if (capture && !m_capturePath.empty())
        m_gfx.CaptureBackbufferToBMP(m_capturePath.c_str());   // Present 전에 캡처

    m_gfx.Present(true);
    m_input.EndFrame();
}

void Application::Run(int maxFrames, const wchar_t* capturePath) {
    if (capturePath) m_capturePath = capturePath;

    int frame = 0;
    while (m_window.PumpMessages()) {
        int64_t now;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
        const float dt   = static_cast<float>(double(now - m_lastTick)  / double(m_tickFreq));
        const float time = static_cast<float>(double(now - m_startTick) / double(m_tickFreq));
        m_lastTick = now;

        const bool last = (maxFrames >= 0 && frame + 1 >= maxFrames);
        Frame(dt, time, /*capture=*/last);

        if (last) break;
        ++frame;
    }
    m_ui.Shutdown();   // 종료 정리
}

} // namespace app
