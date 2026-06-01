#include "render/TriangleRenderer.h"
#include "render/ShaderUtil.h"

namespace render {

// 이 렌더러가 쓰는 정점 한 개의 메모리 배치: 위치(3) + 색(3) = 24바이트.
struct TriVertex {
    float x, y, z;     // POSITION (offset 0)
    float r, g, b;     // COLOR    (offset 12)
};

void TriangleRenderer::Initialize(ID3D11Device* device, const std::wstring& shaderDir) {
    const std::wstring path = shaderDir + L"\\triangle.hlsl";

    // 1) 셰이더 컴파일 → 셰이더 객체 생성
    ComPtr<ID3DBlob> vsBlob = CompileShaderFromFile(path, "VSMain", "vs_5_0");
    ComPtr<ID3DBlob> psBlob = CompileShaderFromFile(path, "PSMain", "ps_5_0");
    Hr(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vs), "CreateVertexShader");
    Hr(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_ps), "CreatePixelShader");

    // 2) 입력 레이아웃 — "정점 버퍼의 바이트를 셰이더 입력으로 어떻게 해석할지" 알려줌.
    //    셰이더의 POSITION/COLOR 시맨틱과, 위 TriVertex의 오프셋(0, 12)을 짝지음.
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    Hr(device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout),
       "CreateInputLayout");

    // 3) 정점 데이터 — 삼각형 3개 꼭짓점 (클립공간 좌표 + 색)
    TriVertex verts[3] = {
        {  0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f },   // 위    - 빨강
        {  0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f },   // 우하   - 초록
        { -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f },   // 좌하   - 파랑
    };
    m_stride = sizeof(TriVertex);
    m_vertexCount = 3;

    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(verts);
    bd.Usage     = D3D11_USAGE_IMMUTABLE;            // 한 번 만들고 안 바꿈 (가장 빠름)
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = verts;
    Hr(device->CreateBuffer(&bd, &init, &m_vertexBuffer), "CreateBuffer(vertex)");

    // 4) 래스터라이저 상태 — 컬링(뒷면 제거)을 꺼서 삼각형이 어느 쪽이든 보이게 함.
    //    (컬링과 앞/뒷면 개념은 Part 4에서 자세히)
    D3D11_RASTERIZER_DESC rs{};
    rs.FillMode = D3D11_FILL_SOLID;
    rs.CullMode = D3D11_CULL_NONE;
    Hr(device->CreateRasterizerState(&rs, &m_rasterState), "CreateRasterizerState");

    // 5) 상수 버퍼 — 매 프레임 바뀌는 행렬을 담을 GPU 버퍼.
    //    DYNAMIC + CPU_WRITE: CPU가 매 프레임 Map 으로 새 값을 써넣을 수 있음.
    //    ByteWidth 는 16의 배수여야 함 (float4x4 = 64바이트, OK).
    D3D11_BUFFER_DESC cbd{};
    cbd.ByteWidth      = sizeof(Math::Matrix);   // 64
    cbd.Usage          = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Hr(device->CreateBuffer(&cbd, nullptr, &m_constantBuffer), "CreateBuffer(constant)");
}

void TriangleRenderer::Render(ID3D11DeviceContext* ctx, const Math::Matrix& transform) {
    // 상수 버퍼 갱신: Map 으로 잠그고 → 행렬 복사 → Unmap.
    D3D11_MAPPED_SUBRESOURCE mapped{};
    ctx->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &transform, sizeof(Math::Matrix));
    ctx->Unmap(m_constantBuffer.Get(), 0);

    UINT offset = 0;
    ID3D11Buffer* vb = m_vertexBuffer.Get();
    ID3D11Buffer* cb = m_constantBuffer.Get();

    // IA = Input Assembler: 정점을 모아 셰이더에 공급하는 단계
    ctx->IASetInputLayout(m_inputLayout.Get());
    ctx->IASetVertexBuffers(0, 1, &vb, &m_stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   // 3개 정점 = 삼각형 1개

    ctx->RSSetState(m_rasterState.Get());
    ctx->VSSetShader(m_vs.Get(), nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, &cb);   // register(b0) 에 연결
    ctx->PSSetShader(m_ps.Get(), nullptr, 0);

    ctx->Draw(m_vertexCount, 0);   // 그려라!
}

} // namespace render
