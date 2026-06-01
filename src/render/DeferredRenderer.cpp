#include "render/DeferredRenderer.h"
#include "render/Vertex.h"
#include "render/ShaderUtil.h"
#include <vector>
#include <cstdint>

namespace render {

using Math::Vector2;
using Math::Vector3;
using Math::Matrix;

// --- 상수 버퍼 레이아웃 (HLSL과 일치) ---
struct GeomCB { Matrix mvp; Matrix world; };                       // 128B
struct LightCB { float eyePos[4]; float dirL[4]; float dirColor[4]; };  // 48B (7.1: 방향광)

static void MakeCube(std::vector<Vertex>& v, std::vector<uint16_t>& idx) {
    const float h = 0.5f;
    struct Face { Vector3 n; Vector3 c[4]; };
    const Face faces[6] = {
        {{ 0, 0, 1}, {{-h,-h, h},{ h,-h, h},{ h, h, h},{-h, h, h}}},
        {{ 0, 0,-1}, {{ h,-h,-h},{-h,-h,-h},{-h, h,-h},{ h, h,-h}}},
        {{ 1, 0, 0}, {{ h,-h, h},{ h,-h,-h},{ h, h,-h},{ h, h, h}}},
        {{-1, 0, 0}, {{-h,-h,-h},{-h,-h, h},{-h, h, h},{-h, h,-h}}},
        {{ 0, 1, 0}, {{-h, h, h},{ h, h, h},{ h, h,-h},{-h, h,-h}}},
        {{ 0,-1, 0}, {{-h,-h,-h},{ h,-h,-h},{ h,-h, h},{-h,-h, h}}},
    };
    const Vector2 uv[4] = { {0,1},{1,1},{1,0},{0,0} };
    v.clear(); idx.clear();
    for (const auto& f : faces) {
        uint16_t base = (uint16_t)v.size();
        for (int i = 0; i < 4; ++i) v.push_back({ f.c[i], f.n, uv[i] });
        idx.insert(idx.end(), { base, uint16_t(base+1), uint16_t(base+2),
                                base, uint16_t(base+2), uint16_t(base+3) });
    }
}

void DeferredRenderer::Initialize(ID3D11Device* device, const std::wstring& shaderDir,
                                  const std::wstring& assetsDir, uint32_t width, uint32_t height) {
    m_width = width; m_height = height;
    m_gbuffer.Initialize(device, width, height);
    InitGeometry(device, shaderDir, assetsDir);
    InitLighting(device, shaderDir);
}

void DeferredRenderer::Resize(ID3D11Device* device, uint32_t width, uint32_t height) {
    m_width = width; m_height = height;
    m_gbuffer.Resize(device, width, height);
}

void DeferredRenderer::InitGeometry(ID3D11Device* device, const std::wstring& shaderDir, const std::wstring& assetsDir) {
    const std::wstring path = shaderDir + L"\\gbuffer.hlsl";
    ComPtr<ID3DBlob> vs = CompileShaderFromFile(path, "VSMain", "vs_5_0");
    ComPtr<ID3DBlob> ps = CompileShaderFromFile(path, "PSMain", "ps_5_0");
    Hr(device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), nullptr, &m_geomVS), "geom VS");
    Hr(device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), nullptr, &m_geomPS), "geom PS");

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    Hr(device->CreateInputLayout(layout, 3, vs->GetBufferPointer(), vs->GetBufferSize(), &m_layout), "layout");

    std::vector<Vertex> verts; std::vector<uint16_t> indices;
    MakeCube(verts, indices);
    m_indexCount = (UINT)indices.size();
    m_stride = sizeof(Vertex);

    D3D11_BUFFER_DESC vbd{}; vbd.ByteWidth = UINT(verts.size()*sizeof(Vertex));
    vbd.Usage = D3D11_USAGE_IMMUTABLE; vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vi{ verts.data() };
    Hr(device->CreateBuffer(&vbd, &vi, &m_vb), "vb");

    D3D11_BUFFER_DESC ibd{}; ibd.ByteWidth = UINT(indices.size()*sizeof(uint16_t));
    ibd.Usage = D3D11_USAGE_IMMUTABLE; ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ii{ indices.data() };
    Hr(device->CreateBuffer(&ibd, &ii, &m_ib), "ib");

    D3D11_BUFFER_DESC cbd{}; cbd.ByteWidth = sizeof(GeomCB);
    cbd.Usage = D3D11_USAGE_DYNAMIC; cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Hr(device->CreateBuffer(&cbd, nullptr, &m_geomCB), "geomCB");

    D3D11_RASTERIZER_DESC rs{}; rs.FillMode = D3D11_FILL_SOLID; rs.CullMode = D3D11_CULL_BACK;
    Hr(device->CreateRasterizerState(&rs, &m_raster), "raster");

    m_texture.LoadDDS(device, assetsDir + L"\\test.dds");

    D3D11_SAMPLER_DESC sd{};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    Hr(device->CreateSamplerState(&sd, &m_sampler), "sampler");
}

void DeferredRenderer::InitLighting(ID3D11Device* device, const std::wstring& shaderDir) {
    const std::wstring path = shaderDir + L"\\lighting.hlsl";
    ComPtr<ID3DBlob> vs = CompileShaderFromFile(path, "VSMain", "vs_5_0");
    ComPtr<ID3DBlob> ps = CompileShaderFromFile(path, "PSMain", "ps_5_0");
    Hr(device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), nullptr, &m_lightVS), "light VS");
    Hr(device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), nullptr, &m_lightPS), "light PS");

    D3D11_BUFFER_DESC cbd{}; cbd.ByteWidth = sizeof(LightCB);
    cbd.Usage = D3D11_USAGE_DYNAMIC; cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Hr(device->CreateBuffer(&cbd, nullptr, &m_lightCB), "lightCB");
}

void DeferredRenderer::Render(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* backbufferRTV,
                              const Matrix& viewProj, const Matrix& model, const LightingData& lights) {
    // ===== ① 지오메트리 패스: 큐브 → G-buffer =====
    m_gbuffer.BindAndClear(ctx);

    GeomCB gcb; gcb.mvp = model * viewProj; gcb.world = model;
    D3D11_MAPPED_SUBRESOURCE m{};
    ctx->Map(m_geomCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m); memcpy(m.pData, &gcb, sizeof(gcb)); ctx->Unmap(m_geomCB.Get(), 0);

    UINT off = 0; ID3D11Buffer* vb = m_vb.Get(); ID3D11Buffer* gcbB = m_geomCB.Get();
    ID3D11ShaderResourceView* tex = m_texture.SRV(); ID3D11SamplerState* samp = m_sampler.Get();
    ctx->IASetInputLayout(m_layout.Get());
    ctx->IASetVertexBuffers(0, 1, &vb, &m_stride, &off);
    ctx->IASetIndexBuffer(m_ib.Get(), DXGI_FORMAT_R16_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->RSSetState(m_raster.Get());
    ctx->VSSetShader(m_geomVS.Get(), nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, &gcbB);
    ctx->PSSetShader(m_geomPS.Get(), nullptr, 0);
    ctx->PSSetShaderResources(0, 1, &tex);
    ctx->PSSetSamplers(0, 1, &samp);
    ctx->DrawIndexed(m_indexCount, 0, 0);

    // ===== ② 라이팅 패스: 풀스크린 삼각형으로 G-buffer 샘플 → 백버퍼 =====
    ctx->OMSetRenderTargets(1, &backbufferRTV, nullptr);   // G-buffer RTV 해제 + 백버퍼로
    D3D11_VIEWPORT vp{}; vp.Width = float(m_width); vp.Height = float(m_height); vp.MaxDepth = 1.0f;
    ctx->RSSetViewports(1, &vp);

    LightCB lcb{};
    lcb.eyePos[0]=lights.eyePos.x; lcb.eyePos[1]=lights.eyePos.y; lcb.eyePos[2]=lights.eyePos.z;
    lcb.dirL[0]=lights.dirLightDir.x; lcb.dirL[1]=lights.dirLightDir.y; lcb.dirL[2]=lights.dirLightDir.z;
    lcb.dirColor[0]=lights.dirLightColor.x; lcb.dirColor[1]=lights.dirLightColor.y; lcb.dirColor[2]=lights.dirLightColor.z;
    ctx->Map(m_lightCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m); memcpy(m.pData, &lcb, sizeof(lcb)); ctx->Unmap(m_lightCB.Get(), 0);

    ID3D11Buffer* lcbB = m_lightCB.Get();
    ctx->IASetInputLayout(nullptr);                                  // 정점버퍼 불필요(SV_VertexID)
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(m_lightVS.Get(), nullptr, 0);
    ctx->PSSetShader(m_lightPS.Get(), nullptr, 0);
    ctx->PSSetShaderResources(0, core::GBuffer::kTargetCount, m_gbuffer.SRVs());  // t0,t1,t2
    ctx->PSSetSamplers(0, 1, &samp);
    ctx->PSSetConstantBuffers(0, 1, &lcbB);
    ctx->Draw(3, 0);

    // 다음 프레임 지오메트리 패스에서 같은 텍스처를 RTV로 쓰므로, SRV 바인딩 해제(경고 방지).
    ID3D11ShaderResourceView* nulls[core::GBuffer::kTargetCount] = {};
    ctx->PSSetShaderResources(0, core::GBuffer::kTargetCount, nulls);
}

} // namespace render
