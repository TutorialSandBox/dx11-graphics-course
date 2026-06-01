#pragma once

#include "app/Win32Window.h"
#include "app/Input.h"
#include <cstdint>

namespace app {

// =============================================================================
// Application — 엔진 전체를 조립하고 메인 루프를 도는 "지휘자".
//   Part가 진행될수록 여기에 디바이스/렌더러/카메라/UI가 하나씩 추가됩니다.
//   (Part 2: 창 + 입력 + 루프만)
// =============================================================================
class Application {
public:
    bool Initialize();

    // 메인 루프. maxFrames >= 0 이면 그 프레임 수만큼만 돌고 종료(검증/스모크용).
    void Run(int maxFrames = -1);

private:
    void Frame(float dt, float time);   // 한 프레임 처리

    Win32Window m_window;
    Input       m_input;

    // 시간 측정 (고해상도 타이머)
    int64_t m_tickFreq  = 0;
    int64_t m_startTick = 0;
    int64_t m_lastTick  = 0;
};

} // namespace app
