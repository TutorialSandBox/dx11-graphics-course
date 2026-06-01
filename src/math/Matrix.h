#pragma once

#include "math/Vector4.h"

// =============================================================================
// Matrix — 4x4 행렬. 벡터를 변환(이동·회전·스케일·투영)하는 도구.
//   규약 (강의 전체 공통):
//     · 저장: 행우선(row-major) — 메모리에 _11,_12,_13,_14, _21,... 순서.
//     · 곱셈: 행벡터(row-vector) — 점을 왼쪽에 두고  v * M.
//     · 좌표계: 왼손(LH).
//   이 규약 덕분에 HLSL에서 row_major로 선언하면 전치 없이 그대로 업로드됩니다(Part 4).
// =============================================================================

namespace Math {

struct Matrix {
    // 행우선 4x4. m[row][col] 으로도, _<row><col> 으로도 접근 가능.
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };

    // 기본값은 항등 행렬(아무것도 바꾸지 않는 행렬).
    //   주의: Identity()를 호출하지 않고 직접 채웁니다 —
    //   Identity()가 내부에서 Matrix를 만들면 생성자→Identity→생성자... 무한 재귀가 되기 때문.
    Matrix();

    // 행렬 곱 (this * rhs). 행벡터 규약에서  v * (A * B) = (v * A) * B,
    // 즉 "A를 적용한 뒤 B를 적용"하는 합성이 됩니다(Part 1.3에서 활용).
    Matrix operator*(const Matrix& rhs) const;

    // ---- 만들기 ----
    static Matrix Identity();        // 아무것도 바꾸지 않는 행렬 (곱셈의 1에 해당)
    static Matrix Transpose(const Matrix& src);   // 행↔열 뒤집기

    // ---- 변환 행렬 (1.3) ----
    static Matrix Translation(const Vector3& t);          // 이동
    static Matrix Scaling(const Vector3& s);              // 축별 확대/축소
    static Matrix Scaling(float s);                       // 균등 확대/축소
    static Matrix RotationX(float radians);               // X축 둘레 회전
    static Matrix RotationY(float radians);               // Y축 둘레 회전
    static Matrix RotationZ(float radians);               // Z축 둘레 회전

    // ---- 카메라/투영 (1.4) ----
    // 뷰 행렬: 카메라를 원점에 두도록 온 세상을 옮김. (eye 위치에서 forward 방향을 봄)
    static Matrix LookToLH(const Vector3& eye, const Vector3& forward, const Vector3& up);
    // 원근 투영: 멀수록 작게. 시야각(fovY)·종횡비(aspect)·근/원 평면(near/far).
    static Matrix PerspectiveFovLH(float fovYRadians, float aspect, float nearZ, float farZ);
};

// 행벡터 × 행렬:  result_j = Σ_i  v_i * m[i][j]
//   (점/방향을 다른 좌표 공간으로 옮기는 핵심 연산)
Vector4 operator*(const Vector4& v, const Matrix& m);

// 편의 함수 (1.3): 3D 점/방향을 행렬로 변환.
//   점(point)    → w=1 로 올려서 변환하고, 원근 나눗셈(÷w) 후 3D로. (이동의 영향 받음)
//   방향(dir)    → w=0 로 변환. (이동의 영향 안 받음 — 회전/스케일만)
Vector3 TransformPoint(const Vector3& p, const Matrix& m);
Vector3 TransformDirection(const Vector3& d, const Matrix& m);

} // namespace Math
