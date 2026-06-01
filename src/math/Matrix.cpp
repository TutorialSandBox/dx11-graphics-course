#include "math/Matrix.h"

namespace Math {

// 기본 생성자: 대각선만 1, 나머지 0인 항등 행렬.  v * I = v (아무것도 안 바꿈).
//   필드를 직접 채워 무한 재귀를 피합니다(헤더 주석 참고).
Matrix::Matrix() {
    _11 = 1; _12 = 0; _13 = 0; _14 = 0;
    _21 = 0; _22 = 1; _23 = 0; _24 = 0;
    _31 = 0; _32 = 0; _33 = 1; _34 = 0;
    _41 = 0; _42 = 0; _43 = 0; _44 = 1;
}

// 명시적으로 "항등 행렬"이라고 읽히게 쓰고 싶을 때 사용. (기본 생성자와 결과 동일)
Matrix Matrix::Identity() { return Matrix(); }

// 행렬 곱:  C = A * B,   C[i][j] = Σ_k A[i][k] * B[k][j]
Matrix Matrix::operator*(const Matrix& rhs) const {
    Matrix r;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k)
                sum += m[i][k] * rhs.m[k][j];
            r.m[i][j] = sum;
        }
    }
    return r;
}

// 전치: 행과 열을 맞바꿈.  T[i][j] = src[j][i]
Matrix Matrix::Transpose(const Matrix& src) {
    Matrix r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r.m[i][j] = src.m[j][i];
    return r;
}

// 행벡터 × 행렬.  result_j = Σ_i v_i * m[i][j]
//   (v를 1x4로 보고 4x4 행렬을 오른쪽에서 곱한 것)
Vector4 operator*(const Vector4& v, const Matrix& mat) {
    return {
        v.x*mat._11 + v.y*mat._21 + v.z*mat._31 + v.w*mat._41,
        v.x*mat._12 + v.y*mat._22 + v.z*mat._32 + v.w*mat._42,
        v.x*mat._13 + v.y*mat._23 + v.z*mat._33 + v.w*mat._43,
        v.x*mat._14 + v.y*mat._24 + v.z*mat._34 + v.w*mat._44
    };
}

} // namespace Math
