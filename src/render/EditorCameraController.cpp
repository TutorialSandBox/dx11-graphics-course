#include "render/EditorCameraController.h"
#include "render/Camera.h"
#include "app/Input.h"

#include <Windows.h>     // VK_SHIFT
#include <algorithm>
#include <cmath>

namespace render {

using Math::Vector3;

void EditorCameraController::Update(const app::Input& in, Camera& cam, float dt) {
    // 1) 우클릭 중에만 시점 회전 (마우스 델타 → yaw/pitch)
    if (in.RightButtonDown()) {
        m_yaw   += in.MouseDX() * m_lookSpeed;
        m_pitch += in.MouseDY() * m_lookSpeed;
        const float limit = 1.55f;   // ≈ 89도 — 수직으로 뒤집히는 짐벌락 방지
        m_pitch = std::clamp(m_pitch, -limit, limit);
    }

    // 2) yaw/pitch → forward 방향 (왼손 좌표계, 게임수학)
    //    yaw=좌우(Y축 둘레), pitch=상하. 구면좌표로 방향 벡터를 만든다.
    const float cp = std::cos(m_pitch), sp = std::sin(m_pitch);
    const float cy = std::cos(m_yaw),   sy = std::sin(m_yaw);
    Vector3 forward = Vector3(sy * cp, sp, cy * cp).Normalized();
    Vector3 up      = Vector3::Up();
    Vector3 right   = up.Cross(forward).Normalized();   // 오른쪽 = 위 × 앞 (LH)

    // 3) 키보드 이동 (이동 방향은 바라보는 방향 기준)
    const float v = m_moveSpeed * dt * (in.IsKeyDown(VK_SHIFT) ? 3.0f : 1.0f);
    if (in.IsKeyDown('W')) m_pos += forward * v;
    if (in.IsKeyDown('S')) m_pos += forward * -v;
    if (in.IsKeyDown('D')) m_pos += right   * v;
    if (in.IsKeyDown('A')) m_pos += right   * -v;
    if (in.IsKeyDown('E')) m_pos += up      * v;
    if (in.IsKeyDown('Q')) m_pos += up      * -v;

    // 4) 카메라에 반영
    cam.SetView(m_pos, forward, up);
}

} // namespace render
