#include "app/Input.h"

namespace app {

void Input::OnMouseMove(int x, int y) {
    // 우클릭 중일 때만 "이동량"을 누적한다. (시점 회전에 쓰임 — Part 5)
    if (m_rmbDown) {
        if (m_haveLast) {
            m_mouseDX += static_cast<float>(x - m_lastX);
            m_mouseDY += static_cast<float>(y - m_lastY);
        }
        m_haveLast = true;   // 첫 이동은 기준점만 잡고 델타는 0 (튐 방지)
    }
    m_lastX = x;
    m_lastY = y;
}

} // namespace app
