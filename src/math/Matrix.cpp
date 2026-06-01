#include "math/Matrix.h"
#include <cmath>

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

// ---- 변환 행렬 (1.3) -------------------------------------------------------
//   모두 "행벡터 v*M, 왼손 좌표계" 규약에 맞춰 작성. (DirectXMath와 동일)

// 이동: 평행이동 성분은 맨 아래 행(_41,_42,_43)에 들어갑니다.
//   (x,y,z,1) * T = (x+tx, y+ty, z+tz, 1)  ← w=1 일 때만 이동이 더해짐
Matrix Matrix::Translation(const Vector3& t) {
    Matrix r;                       // 항등에서 시작
    r._41 = t.x; r._42 = t.y; r._43 = t.z;
    return r;
}

// 스케일: 대각선에 배율.
Matrix Matrix::Scaling(const Vector3& s) {
    Matrix r;
    r._11 = s.x; r._22 = s.y; r._33 = s.z;
    return r;
}
Matrix Matrix::Scaling(float s) { return Scaling({ s, s, s }); }

// 회전 (라디안). 부호 규약은 왼손 좌표계 기준.
Matrix Matrix::RotationX(float a) {
    float c = std::cos(a), s = std::sin(a);
    Matrix r;
    r._22 =  c; r._23 = s;
    r._32 = -s; r._33 = c;
    return r;
}
Matrix Matrix::RotationY(float a) {
    float c = std::cos(a), s = std::sin(a);
    Matrix r;
    r._11 = c; r._13 = -s;
    r._31 = s; r._33 =  c;
    return r;
}
Matrix Matrix::RotationZ(float a) {
    float c = std::cos(a), s = std::sin(a);
    Matrix r;
    r._11 =  c; r._12 = s;
    r._21 = -s; r._22 = c;
    return r;
}

// ---- 점/방향 변환 편의 함수 -------------------------------------------------
Vector3 TransformPoint(const Vector3& p, const Matrix& m) {
    Vector4 r = Vector4{ p, 1.0f } * m;   // 점이므로 w=1
    return r.ToVector3DivW();              // 원근 나눗셈(÷w). 아핀 변환이면 w=1이라 그대로.
}
Vector3 TransformDirection(const Vector3& d, const Matrix& m) {
    Vector4 r = Vector4{ d, 0.0f } * m;   // 방향이므로 w=0 → 이동 무시
    return r.XYZ();
}

} // namespace Math
