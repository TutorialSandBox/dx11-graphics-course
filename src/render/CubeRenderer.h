#pragma once

#include "core/DxCommon.h"
#include "math/Matrix.h"
#include <string>

namespace render {

// =============================================================================
// CubeRenderer — 텍스처/조명이 들어갈 3D 큐브 하나를 그리는 렌더러.
//   (4.1: 메시 + 인덱스 버퍼 + 깊이 테스트 + 회전 MVP. 텍스처는 4.2, 조명은 4.3)
// =============================================================================
class CubeRenderer {
public:
    void Initialize(ID3D11Device* device, const std::wstring& shaderDir);
    void Render(ID3D11DeviceContext* ctx, const Math::Matrix& mvp);

private:
    ComPtr<ID3D11Buffer>          m_vertexBuffer;
    ComPtr<ID3D11Buffer>          m_indexBuffer;
    ComPtr<ID3D11Buffer>          m_constantBuffer;
    ComPtr<ID3D11InputLayout>     m_inputLayout;
    ComPtr<ID3D11VertexShader>    m_vs;
    ComPtr<ID3D11PixelShader>     m_ps;
    ComPtr<ID3D11RasterizerState> m_rasterState;
    UINT m_indexCount = 0;
    UINT m_stride = 0;
};

} // namespace render
