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
};

// 행벡터 × 행렬:  result_j = Σ_i  v_i * m[i][j]
//   (점/방향을 다른 좌표 공간으로 옮기는 핵심 연산)
Vector4 operator*(const Vector4& v, const Matrix& m);

} // namespace Math
