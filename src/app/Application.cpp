#include "app/Application.h"
#include <Windows.h>
#include <cstdio>
#include <cwchar>

namespace app {

bool Application::Initialize() {
    if (!m_window.Create(L"MiniEngine (Part 2) - Win32 창과 입력", 1280, 720))
        return false;

    m_window.SetInput(&m_input);   // 키/마우스 메시지를 Input 으로 보내라

    // 고해상도 타이머 준비 (프레임 간 경과시간 dt 측정용)
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_tickFreq));
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTick));
    m_lastTick = m_startTick;
    return true;
}

void Application::Frame(float dt, float time) {
    // Part 2에는 아직 그릴 게 없습니다. 대신 입력 상태를 제목표시줄에 출력해
    // "입력이 정말 들어오는지" 눈으로 확인합니다.
    const float fps = dt > 0.0f ? 1.0f / dt : 0.0f;

    wchar_t title[256];
    std::swprintf(title, 256,
        L"MiniEngine (Part 2)  |  %.0f FPS  |  W:%d A:%d S:%d D:%d  RMB:%d  마우스Δ(%.0f, %.0f)",
        fps,
        m_input.IsKeyDown('W'), m_input.IsKeyDown('A'),
        m_input.IsKeyDown('S'), m_input.IsKeyDown('D'),
        m_input.RightButtonDown(),
        m_input.MouseDX(), m_input.MouseDY());
    m_window.SetTitle(title);

    m_input.EndFrame();   // 이번 프레임의 마우스 델타 소비 → 0으로
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
            break;   // 스모크 테스트: 정해진 프레임 수 후 자동 종료
        Sleep(1);    // CPU 100% 점유 방지 (학습용 — 실엔진은 다른 방식)
    }
}

} // namespace app
