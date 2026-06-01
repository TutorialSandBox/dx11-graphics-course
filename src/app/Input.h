#pragma once

namespace app {

// =============================================================================
// Input — 입력 facade(겉창구).
//   Win32Window가 OS 메시지를 이 클래스로 전달(On*)하고,
//   게임 코드는 IsKeyDown / MouseDX 같은 질의(query)만 합니다.
//   이렇게 "메시지 처리"와 "게임 로직"을 분리하면 코드가 깔끔해집니다.
// =============================================================================
class Input {
public:
    // --- Win32Window가 호출 (메시지 → 상태 저장) ---
    void OnKeyDown(int vk)         { if (vk >= 0 && vk < 256) m_keys[vk] = true; }
    void OnKeyUp(int vk)           { if (vk >= 0 && vk < 256) m_keys[vk] = false; }
    void OnMouseMove(int x, int y);
    void OnRightButton(bool down)  { m_rmbDown = down; if (down) m_haveLast = false; }

    // 매 프레임 끝에 호출 — 누적된 마우스 이동량(델타)을 0으로 리셋.
    void EndFrame()                { m_mouseDX = 0.0f; m_mouseDY = 0.0f; }

    // --- 게임 코드가 질의 ---
    bool  IsKeyDown(int vk) const  { return (vk >= 0 && vk < 256) && m_keys[vk]; }
    bool  RightButtonDown() const  { return m_rmbDown; }
    float MouseDX() const          { return m_mouseDX; }   // 이번 프레임 가로 이동량
    float MouseDY() const          { return m_mouseDY; }   // 이번 프레임 세로 이동량

private:
    bool  m_keys[256] = {};
    bool  m_rmbDown   = false;
    bool  m_haveLast  = false;
    int   m_lastX = 0, m_lastY = 0;
    float m_mouseDX = 0.0f, m_mouseDY = 0.0f;
};

} // namespace app
