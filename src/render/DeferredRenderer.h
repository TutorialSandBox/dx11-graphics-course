#pragma once

#include "core/DxCommon.h"
#include "core/GBuffer.h"
#include "render/Texture.h"
#include "render/Lights.h"
#include "math/Matrix.h"
#include <string>

namespace render {

// =============================================================================
// DeferredRenderer — 디퍼드 렌더링 파이프라인 전체.
//   ① 지오메트리 패스: 큐브 → G-buffer(albedo/normal/position) 기록
//   ② 라이팅 패스: 풀스크린 삼각형으로 G-buffer 샘플 + 조명 합성 → 백버퍼
// =============================================================================
class DeferredRenderer {
public:
    void Initialize(ID3D11Device* device, const std::wstring& shaderDir,
                    const std::wstring& assetsDir, uint32_t width, uint32_t height);
    void Resize(ID3D11Device* device, uint32_t width, uint32_t height);

    void Render(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* backbufferRTV,
                const Math::Matrix& viewProj, const Math::Matrix& model,
                const LightingData& lights);

private:
    void InitGeometry(ID3D11Device* device, const std::wstring& shaderDir, const std::wstring& assetsDir);
    void InitLighting(ID3D11Device* device, const std::wstring& shaderDir);

    core::GBuffer m_gbuffer;
    uint32_t m_width = 0, m_height = 0;

    // 지오메트리 패스 자원 (큐브)
    ComPtr<ID3D11Buffer>          m_vb, m_ib, m_geomCB;
    ComPtr<ID3D11InputLayout>     m_layout;
    ComPtr<ID3D11VertexShader>    m_geomVS;
    ComPtr<ID3D11PixelShader>     m_geomPS;
    ComPtr<ID3D11RasterizerState> m_raster;
    ComPtr<ID3D11SamplerState>    m_sampler;
    Texture                       m_texture;
    UINT m_indexCount = 0, m_stride = 0;

    // 라이팅 패스 자원 (풀스크린)
    ComPtr<ID3D11VertexShader> m_lightVS;
    ComPtr<ID3D11PixelShader>  m_lightPS;
    ComPtr<ID3D11Buffer>       m_lightCB;
};

} // namespace render
