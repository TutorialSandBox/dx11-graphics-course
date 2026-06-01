#pragma once

#include "app/Win32Window.h"
#include "app/Input.h"
#include "core/GraphicsDevice.h"
#include "render/CubeRenderer.h"
#include "render/Camera.h"
#include "render/EditorCameraController.h"
#include <cstdint>
#include <string>

namespace app {

// =============================================================================
// Application — 엔진 전체를 조립하고 메인 루프를 도는 "지휘자".
//   (Part 3.2: 창 + 입력 + DX11 디바이스 + 삼각형)
// =============================================================================
class Application {
public:
    bool Initialize();

    // maxFrames>=0 이면 그만큼만 돌고 종료(검증용).
    // capturePath 가 있으면 마지막 프레임을 BMP로 저장(렌더 결과 눈으로 확인).
    void Run(int maxFrames = -1, const wchar_t* capturePath = nullptr);

private:
    void Frame(float dt, float time, bool capture);
    std::wstring ExecutableDir() const;

    Win32Window                    m_window;
    Input                          m_input;
    core::GraphicsDevice           m_gfx;
    render::CubeRenderer           m_cube;
    render::Camera                 m_camera;
    render::EditorCameraController m_camController;

    std::wstring m_capturePath;

    int64_t m_tickFreq  = 0;
    int64_t m_startTick = 0;
    int64_t m_lastTick  = 0;
};

} // namespace app
