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
    if (!m_window.Create(L"MiniEngine (Part 3.1) - 화면 클리어", 1280, 720))
        return false;
    m_window.SetInput(&m_input);

    // 창 크기가 바뀌면 스왑체인 백버퍼도 다시 만든다.
    m_window.SetOnResize([this](uint32_t w, uint32_t h) { m_gfx.Resize(w, h); });

    if (!m_gfx.Initialize(m_window.Handle(), m_window.Width(), m_window.Height()))
        return false;

    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_tickFreq));
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTick));
    m_lastTick = m_startTick;
    return true;
}

void Application::Frame(float dt, float time) {
    // 시간에 따라 부드럽게 변하는 배경색 — "매 프레임 화면이 갱신된다"는 증거.
    float r = 0.5f + 0.5f * std::sin(time * 0.7f);
    float g = 0.5f + 0.5f * std::sin(time * 0.7f + 2.0f);
    float b = 0.5f + 0.5f * std::sin(time * 0.7f + 4.0f);

    m_gfx.ClearBackbuffer(r, g, b);
    m_gfx.Present(true);

    m_input.EndFrame();
}

void Application::Run(int maxFrames) {
    int frame = 0;
    while (m_window.PumpMessages()) {
        int64_t now;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
        const float dt   = static_cast<float>(double(now - m_lastTick)  / double(m_tickFreq));
        const float time = static_cast<float>(double(now - m_startTick) / double(m_tickFreq));
        m_lastTick = now;

        Frame(dt, time);

        if (maxFrames >= 0 && ++frame >= maxFrames)
            break;
    }
}

} // namespace app
