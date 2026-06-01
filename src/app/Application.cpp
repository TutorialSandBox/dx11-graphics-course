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
    if (!m_window.Create(L"MiniEngine (Part 4.1) - 3D 큐브와 깊이 버퍼", 1280, 720))
        return false;
    m_window.SetInput(&m_input);
    m_window.SetOnResize([this](uint32_t w, uint32_t h) { m_gfx.Resize(w, h); });

    if (!m_gfx.Initialize(m_window.Handle(), m_window.Width(), m_window.Height()))
        return false;

    m_cube.Initialize(m_gfx.Device(), ExecutableDir() + L"\\shaders");

    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_tickFreq));
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTick));
    m_lastTick = m_startTick;
    return true;
}

void Application::Frame(float dt, float time, bool capture) {
    m_gfx.ClearBackbuffer(0.10f, 0.12f, 0.18f);   // 배경 + 깊이 버퍼 클리어

    // --- MVP 조립 (Part 1.4) ---
    using Math::Matrix;
    using Math::Vector3;
    // 모델: 두 축으로 천천히 회전 (큐브의 여러 면을 보여주기 위해)
    Matrix model = Matrix::RotationY(time * 0.6f) * Matrix::RotationX(time * 0.3f);
    // 뷰: 카메라를 (0, 0.4, -3)에 두고 +Z(앞)를 바라봄 (자유 카메라는 Part 5)
    Matrix view = Matrix::LookToLH({ 0.0f, 0.4f, -3.0f }, Vector3::Forward(), Vector3::Up());
    // 투영: 원근
    Matrix proj = Matrix::PerspectiveFovLH(Math::ToRadians(60.0f), m_window.Aspect(), 0.1f, 100.0f);

    Matrix mvp = model * view * proj;
    m_cube.Render(m_gfx.Context(), mvp);

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
