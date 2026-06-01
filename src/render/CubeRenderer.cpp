#include "render/CubeRenderer.h"
#include "render/Vertex.h"
#include "render/ShaderUtil.h"
#include <vector>
#include <cstdint>

namespace render {

using Math::Vector2;
using Math::Vector3;

// 큐브 메시 생성: 6면 × 4꼭짓점 = 24정점, 6면 × 2삼각형 × 3 = 36인덱스.
//   왜 꼭짓점이 8개가 아니라 24개? 면마다 법선과 UV가 다르기 때문.
//   (한 꼭짓점이 3면에 공유되면 법선을 하나로 정할 수 없음 → 면마다 따로 둠)
static void MakeCube(std::vector<Vertex>& v, std::vector<uint16_t>& idx) {
    const float h = 0.5f;
    struct Face { Vector3 n; Vector3 c[4]; };
    const Face faces[6] = {
        {{ 0, 0, 1}, {{-h,-h, h},{ h,-h, h},{ h, h, h},{-h, h, h}}}, // +Z
        {{ 0, 0,-1}, {{ h,-h,-h},{-h,-h,-h},{-h, h,-h},{ h, h,-h}}}, // -Z
        {{ 1, 0, 0}, {{ h,-h, h},{ h,-h,-h},{ h, h,-h},{ h, h, h}}}, // +X
        {{-1, 0, 0}, {{-h,-h,-h},{-h,-h, h},{-h, h, h},{-h, h,-h}}}, // -X
        {{ 0, 1, 0}, {{-h, h, h},{ h, h, h},{ h, h,-h},{-h, h,-h}}}, // +Y
        {{ 0,-1, 0}, {{-h,-h,-h},{ h,-h,-h},{ h,-h, h},{-h,-h, h}}}, // -Y
    };
    const Vector2 uv[4] = { {0,1}, {1,1}, {1,0}, {0,0} };

    v.clear(); idx.clear();
    for (const auto& f : faces) {
        uint16_t base = static_cast<uint16_t>(v.size());
        for (int i = 0; i < 4; ++i)
            v.push_back({ f.c[i], f.n, uv[i] });
        // 두 삼각형: (base, base+1, base+2) 와 (base, base+2, base+3)
        idx.insert(idx.end(), { base, uint16_t(base+1), uint16_t(base+2),
                                base, uint16_t(base+2), uint16_t(base+3) });
    }
}

void CubeRenderer::Initialize(ID3D11Device* device, const std::wstring& shaderDir, const std::wstring& assetsDir) {
    const std::wstring path = shaderDir + L"\\cube.hlsl";

    ComPtr<ID3DBlob> vsBlob = CompileShaderFromFile(path, "VSMain", "vs_5_0");
    ComPtr<ID3DBlob> psBlob = CompileShaderFromFile(path, "PSMain", "ps_5_0");
    Hr(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vs), "CreateVertexShader");
    Hr(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_ps), "CreatePixelShader");

    // 입력 레이아웃: position(0) / normal(12) / uv(24) — Vertex 구조체의 오프셋과 일치.
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    Hr(device->CreateInputLayout(layout, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout),
       "CreateInputLayout");

    // 메시 → 정점/인덱스 버퍼
    std::vector<Vertex> verts;
    std::vector<uint16_t> indices;
    MakeCube(verts, indices);
    m_indexCount = static_cast<UINT>(indices.size());
    m_stride = sizeof(Vertex);

    D3D11_BUFFER_DESC vbd{};
    vbd.ByteWidth = UINT(verts.size() * sizeof(Vertex));
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vinit{ verts.data() };
    Hr(device->CreateBuffer(&vbd, &vinit, &m_vertexBuffer), "CreateBuffer(vertex)");

    D3D11_BUFFER_DESC ibd{};
    ibd.ByteWidth = UINT(indices.size() * sizeof(uint16_t));
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA iinit{ indices.data() };
    Hr(device->CreateBuffer(&ibd, &iinit, &m_indexBuffer), "CreateBuffer(index)");

    // 상수 버퍼 (MVP)
    D3D11_BUFFER_DESC cbd{};
    cbd.ByteWidth = sizeof(Math::Matrix);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Hr(device->CreateBuffer(&cbd, nullptr, &m_constantBuffer), "CreateBuffer(constant)");

    // 래스터라이저: 뒷면 컬링 켬(성능). 면 winding이 앞면=시계방향(기본) 규약에 맞음.
    D3D11_RASTERIZER_DESC rs{};
    rs.FillMode = D3D11_FILL_SOLID;
    rs.CullMode = D3D11_CULL_BACK;
    Hr(device->CreateRasterizerState(&rs, &m_rasterState), "CreateRasterizerState");

    // 텍스처 로드 + 샘플러(이미지에서 색을 어떻게 읽을지) 생성.
    m_texture.LoadDDS(device, assetsDir + L"\\test.dds");

    D3D11_SAMPLER_DESC sd{};
    sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;   // 부드러운 보간(선형 필터)
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;        // UV가 0~1 밖이면 반복
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    Hr(device->CreateSamplerState(&sd, &m_sampler), "CreateSamplerState");
}

void CubeRenderer::Render(ID3D11DeviceContext* ctx, const Math::Matrix& mvp) {
    D3D11_MAPPED_SUBRESOURCE mapped{};
    ctx->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &mvp, sizeof(Math::Matrix));
    ctx->Unmap(m_constantBuffer.Get(), 0);

    UINT offset = 0;
    ID3D11Buffer* vb = m_vertexBuffer.Get();
    ID3D11Buffer* cb = m_constantBuffer.Get();

    ctx->IASetInputLayout(m_inputLayout.Get());
    ctx->IASetVertexBuffers(0, 1, &vb, &m_stride, &offset);
    ctx->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11ShaderResourceView* srv = m_texture.SRV();
    ID3D11SamplerState* samp = m_sampler.Get();

    ctx->RSSetState(m_rasterState.Get());
    ctx->VSSetShader(m_vs.Get(), nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, &cb);
    ctx->PSSetShader(m_ps.Get(), nullptr, 0);
    ctx->PSSetShaderResources(0, 1, &srv);   // t0 = 텍스처
    ctx->PSSetSamplers(0, 1, &samp);         // s0 = 샘플러

    ctx->DrawIndexed(m_indexCount, 0, 0);   // 인덱스로 그리기 (정점 재사용)
}

} // namespace render
