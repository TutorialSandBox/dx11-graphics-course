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
    if (!m_window.Create(L"MiniEngine (Part 3.2) - 첫 삼각형", 1280, 720))
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
    m_triangle.Render(m_gfx.Context());            // 그 위에 삼각형

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
