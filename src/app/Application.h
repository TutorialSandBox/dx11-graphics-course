#pragma once

#include "app/Win32Window.h"
#include "app/Input.h"
#include "core/GraphicsDevice.h"
#include <cstdint>
#include <string>

namespace app {

// =============================================================================
// Application — 엔진 전체를 조립하고 메인 루프를 도는 "지휘자".
//   Part가 진행될수록 여기에 디바이스/렌더러/카메라/UI가 하나씩 추가됩니다.
//   (Part 3.1: 창 + 입력 + DX11 디바이스 + 화면 클리어)
// =============================================================================
class Application {
public:
    bool Initialize();
    void Run(int maxFrames = -1);

private:
    void Frame(float dt, float time);

    // 실행 파일이 있는 폴더 경로 (셰이더/에셋을 여기 기준으로 로드)
    std::wstring ExecutableDir() const;

    Win32Window          m_window;
    Input                m_input;
    core::GraphicsDevice m_gfx;

    int64_t m_tickFreq  = 0;
    int64_t m_startTick = 0;
    int64_t m_lastTick  = 0;
};

} // namespace app
