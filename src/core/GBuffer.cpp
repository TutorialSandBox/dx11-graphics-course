#include "core/GBuffer.h"

namespace core {

// 각 타깃의 픽셀 포맷. position 은 월드 좌표라 정밀도가 필요해 16비트 float.
static const DXGI_FORMAT kFormats[GBuffer::kTargetCount] = {
    DXGI_FORMAT_R8G8B8A8_UNORM,        // albedo
    DXGI_FORMAT_R8G8B8A8_UNORM,        // normal
    DXGI_FORMAT_R16G16B16A16_FLOAT,    // position
};

void GBuffer::Initialize(ID3D11Device* device, uint32_t width, uint32_t height) {
    m_width = width; m_height = height;
    Create(device);
}

void GBuffer::Resize(ID3D11Device* device, uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return;
    m_width = width; m_height = height;
    for (int i = 0; i < kTargetCount; ++i) { m_tex[i].Reset(); m_rtv[i].Reset(); m_srv[i].Reset(); }
    m_depthTex.Reset(); m_dsv.Reset();
    Create(device);
}

void GBuffer::Create(ID3D11Device* device) {
    for (int i = 0; i < kTargetCount; ++i) {
        // 텍스처: 렌더 타깃(쓰기) + 셰이더 자원(읽기) 둘 다 가능해야 함.
        D3D11_TEXTURE2D_DESC td{};
        td.Width = m_width; td.Height = m_height;
        td.MipLevels = 1; td.ArraySize = 1;
        td.Format = kFormats[i];
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        Hr(device->CreateTexture2D(&td, nullptr, &m_tex[i]), "GBuffer CreateTexture2D");
        Hr(device->CreateRenderTargetView(m_tex[i].Get(), nullptr, &m_rtv[i]), "GBuffer RTV");
        Hr(device->CreateShaderResourceView(m_tex[i].Get(), nullptr, &m_srv[i]), "GBuffer SRV");
        m_srvPtrs[i] = m_srv[i].Get();
    }

    // 깊이 버퍼 (지오메트리 패스의 깊이 테스트용)
    D3D11_TEXTURE2D_DESC dd{};
    dd.Width = m_width; dd.Height = m_height;
    dd.MipLevels = 1; dd.ArraySize = 1;
    dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dd.SampleDesc.Count = 1;
    dd.Usage = D3D11_USAGE_DEFAULT;
    dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    Hr(device->CreateTexture2D(&dd, nullptr, &m_depthTex), "GBuffer depth tex");
    Hr(device->CreateDepthStencilView(m_depthTex.Get(), nullptr, &m_dsv), "GBuffer DSV");
}

void GBuffer::BindAndClear(ID3D11DeviceContext* ctx) {
    ID3D11RenderTargetView* rtvs[kTargetCount] = { m_rtv[0].Get(), m_rtv[1].Get(), m_rtv[2].Get() };
    ctx->OMSetRenderTargets(kTargetCount, rtvs, m_dsv.Get());   // ★ 한 번에 3개 타깃 (MRT)

    // albedo.a 를 0으로 클리어 → 물체가 안 그려진 픽셀은 a=0(배경 마스크).
    const float zero[4] = { 0, 0, 0, 0 };
    for (int i = 0; i < kTargetCount; ++i)
        ctx->ClearRenderTargetView(m_rtv[i].Get(), zero);
    ctx->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    D3D11_VIEWPORT vp{};
    vp.Width = float(m_width); vp.Height = float(m_height); vp.MaxDepth = 1.0f;
    ctx->RSSetViewports(1, &vp);
}

} // namespace core
