#pragma once

#include "core/DxCommon.h"
#include "math/Matrix.h"
#include <string>

namespace render {

// =============================================================================
// TriangleRenderer — 색이 있는 삼각형 하나를 그리는 가장 단순한 렌더러.
//   배우는 것: 정점 버퍼 + 셰이더 + 입력 레이아웃 + (3.3) 상수 버퍼로 변환 전달.
// =============================================================================
class TriangleRenderer {
public:
    void Initialize(ID3D11Device* device, const std::wstring& shaderDir);
    // transform: 정점에 곱할 행렬 (회전 등). 상수 버퍼로 셰이더에 전달됨.
    void Render(ID3D11DeviceContext* ctx, const Math::Matrix& transform);

private:
    ComPtr<ID3D11Buffer>       m_vertexBuffer;
    ComPtr<ID3D11Buffer>       m_constantBuffer;
    ComPtr<ID3D11InputLayout>  m_inputLayout;
    ComPtr<ID3D11VertexShader> m_vs;
    ComPtr<ID3D11PixelShader>  m_ps;
    ComPtr<ID3D11RasterizerState> m_rasterState;   // 컬링 끔(양면 표시) — Part 4에서 설명
    UINT m_stride = 0;
    UINT m_vertexCount = 0;
};

} // namespace render
