#pragma once

#include "math/Matrix.h"
#include "math/Vector3.h"

namespace render {

// =============================================================================
// Camera — 순수 데이터/수학. 입력에 의존하지 않습니다.
//   eye(위치) + forward(보는 방향) + up 으로 정의하고, ViewProj 행렬을 제공합니다.
//   (입력→카메라 조작은 EditorCameraController 가 담당 — 관심사 분리)
// =============================================================================
class Camera {
public:
    void SetPerspective(float fovYRadians, float aspect, float nearZ, float farZ) {
        m_fovY = fovYRadians; m_aspect = aspect; m_near = nearZ; m_far = farZ;
    }
    void SetAspect(float aspect) { m_aspect = aspect; }

    void SetView(const Math::Vector3& eye, const Math::Vector3& forward, const Math::Vector3& up) {
        m_eye = eye; m_forward = forward; m_up = up;
    }

    const Math::Vector3& Eye() const { return m_eye; }

    Math::Matrix View() const { return Math::Matrix::LookToLH(m_eye, m_forward, m_up); }
    Math::Matrix Proj() const { return Math::Matrix::PerspectiveFovLH(m_fovY, m_aspect, m_near, m_far); }
    Math::Matrix ViewProj() const { return View() * Proj(); }   // 행벡터: v * View * Proj

private:
    float m_fovY = 1.0472f /*60도*/, m_aspect = 1.7777f, m_near = 0.1f, m_far = 100.0f;
    Math::Vector3 m_eye{ 0.0f, 0.4f, -3.0f };
    Math::Vector3 m_forward{ 0.0f, 0.0f, 1.0f };
    Math::Vector3 m_up{ 0.0f, 1.0f, 0.0f };
};

} // namespace render
