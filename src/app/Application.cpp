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
    if (!m_window.Create(L"MiniEngine (Part 7.2) - 디퍼드 + 회전 3색 점광원", 1280, 720))
        return false;
    m_window.SetInput(&m_input);
    m_window.SetOnResize([this](uint32_t w, uint32_t h) {
        m_gfx.Resize(w, h);
        m_deferred.Resize(m_gfx.Device(), w, h);
        m_camera.SetAspect(m_window.Aspect());
    });

    if (!m_gfx.Initialize(m_window.Handle(), m_window.Width(), m_window.Height()))
        return false;

    const std::wstring exeDir = ExecutableDir();
    m_deferred.Initialize(m_gfx.Device(), exeDir + L"\\shaders", exeDir + L"\\assets",
                          m_window.Width(), m_window.Height());

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

    // 디퍼드 렌더링: 지오메트리 패스(G-buffer) → 라이팅 패스(백버퍼).
    //   라이팅 패스가 화면 전체를 덮어 그리므로 별도 ClearBackbuffer는 불필요.
    Matrix model = Matrix::RotationY(time * 0.3f);

    render::LightingData lights;
    lights.eyePos        = m_camera.Eye();
    lights.dirLightDir   = Vector3(0.4f, 0.8f, 0.3f).Normalized();
    lights.dirLightColor = Vector3(0.45f, 0.43f, 0.40f);   // 방향광은 은은하게 (점광원이 주인공)

    // 회전하는 3색 점광원 — 큐브 둘레를 120도 간격으로 공전.
    const Vector3 cols[3] = { {1.0f,0.25f,0.20f}, {0.25f,1.0f,0.35f}, {0.30f,0.45f,1.0f} };
    const float twoPi = 6.2831853f;
    lights.numPoints = 3;
    for (int i = 0; i < 3; ++i) {
        float a = time * 0.9f + i * (twoPi / 3.0f);
        lights.points[i].position = Vector3(std::cos(a) * 2.2f, 0.6f, std::sin(a) * 2.2f);
        lights.points[i].radius   = 4.5f;
        lights.points[i].color    = cols[i];
    }

    m_deferred.Render(m_gfx.Context(), m_gfx.BackbufferRTV(),
                      m_camera.ViewProj(), model, lights);

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
