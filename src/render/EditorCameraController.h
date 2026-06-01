#pragma once

#include "math/Vector3.h"

namespace app { class Input; }

namespace render {

class Camera;

// =============================================================================
// EditorCameraController — 입력을 받아 카메라를 움직이는 "자유비행" 컨트롤러.
//   우클릭 드래그 = 시점 회전(yaw/pitch), WASD = 이동, E/Q = 상하, Shift = 가속.
//   내부에 위치(m_pos)와 각도(yaw/pitch)를 들고, 매 프레임 Camera 에 반영합니다.
// =============================================================================
class EditorCameraController {
public:
    void Update(const app::Input& in, Camera& cam, float dt);

private:
    Math::Vector3 m_pos{ 0.0f, 0.4f, -3.0f };
    float m_yaw   = 0.0f;   // 좌우 회전(라디안)
    float m_pitch = 0.0f;   // 상하 회전(라디안)
    float m_moveSpeed = 4.0f;
    float m_lookSpeed = 0.005f;
};

} // namespace render
