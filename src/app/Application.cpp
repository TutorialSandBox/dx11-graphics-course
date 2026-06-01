#include "app/Application.h"
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
    if (!m_window.Create(L"MiniEngine (Part 5) - 자유비행 카메라 (우클릭+WASD)", 1280, 720))
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

    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_tickFreq));
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTick));
    m_lastTick = m_startTick;
    return true;
}

void Application::Frame(float dt, float time, bool capture) {
    using Math::Matrix;
    using Math::Vector3;

    // 입력 → 카메라 갱신 (우클릭 시점 회전 + WASD 이동)
    m_camController.Update(m_input, m_camera, dt);

    m_gfx.ClearBackbuffer(0.10f, 0.12f, 0.18f);   // 배경 + 깊이 버퍼 클리어

    // 모델: 천천히 회전하는 큐브 (카메라로 둘러볼 대상)
    Matrix model = Matrix::RotationY(time * 0.3f);
    // 뷰·투영은 이제 카메라에서 가져온다 (Part 1.4 + Part 5)
    Matrix mvp = model * m_camera.ViewProj();

    Vector3 lightDir = Vector3(0.4f, 0.8f, 0.3f).Normalized();
    m_cube.Render(m_gfx.Context(), mvp, model, lightDir);

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
}

} // namespace app
