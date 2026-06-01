#include "app/Application.h"
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
    if (!m_window.Create(L"MiniEngine (Part 3.3) - 상수 버퍼로 삼각형 회전", 1280, 720))
        return false;
    m_window.SetInput(&m_input);
    m_window.SetOnResize([this](uint32_t w, uint32_t h) { m_gfx.Resize(w, h); });

    if (!m_gfx.Initialize(m_window.Handle(), m_window.Width(), m_window.Height()))
        return false;

    m_triangle.Initialize(m_gfx.Device(), ExecutableDir() + L"\\shaders");

    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_tickFreq));
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTick));
    m_lastTick = m_startTick;
    return true;
}

void Application::Frame(float dt, float time, bool capture) {
    m_gfx.ClearBackbuffer(0.10f, 0.12f, 0.18f);   // 어두운 남색 배경

    // 시간에 따라 Z축 회전 + 종횡비 보정(x를 1/aspect 로 줄여 삼각형이 안 찌그러지게).
    //   행벡터 규약: v * (Rotation * AspectScale) = 먼저 회전, 그 다음 화면비 보정.
    const float aspect = m_window.Aspect();
    Math::Matrix transform =
        Math::Matrix::RotationZ(time) *
        Math::Matrix::Scaling({ 1.0f / aspect, 1.0f, 1.0f });
    m_triangle.Render(m_gfx.Context(), transform);

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
