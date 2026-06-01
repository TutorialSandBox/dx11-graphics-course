#pragma once

#include "core/DxCommon.h"
#include "math/Matrix.h"
#include "render/Texture.h"
#include <string>

namespace render {

// =============================================================================
// CubeRenderer — 텍스처/조명이 들어가는 3D 큐브 하나를 그리는 렌더러.
//   (4.1: 메시+인덱스+깊이+MVP, 4.2: 텍스처, 4.3: 조명)
// =============================================================================
class CubeRenderer {
public:
    void Initialize(ID3D11Device* device, const std::wstring& shaderDir, const std::wstring& assetsDir);
    void Render(ID3D11DeviceContext* ctx, const Math::Matrix& mvp);

private:
    ComPtr<ID3D11Buffer>          m_vertexBuffer;
    ComPtr<ID3D11Buffer>          m_indexBuffer;
    ComPtr<ID3D11Buffer>          m_constantBuffer;
    ComPtr<ID3D11InputLayout>     m_inputLayout;
    ComPtr<ID3D11VertexShader>    m_vs;
    ComPtr<ID3D11PixelShader>     m_ps;
    ComPtr<ID3D11RasterizerState> m_rasterState;
    ComPtr<ID3D11SamplerState>    m_sampler;
    Texture                       m_texture;
    UINT m_indexCount = 0;
    UINT m_stride = 0;
};

} // namespace render
