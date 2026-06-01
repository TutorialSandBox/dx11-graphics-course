#include "core/GraphicsDevice.h"

namespace core {

bool GraphicsDevice::Initialize(HWND hwnd, uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;

    // 스왑체인 설정: 백버퍼 2개(더블 버퍼링), 창 모드, 화면에 보일 형식.
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount       = 2;
    sd.BufferDesc.Width  = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   // RGBA 8비트
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // 이 버퍼에 그릴 것
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;                             // MSAA 끔 (지금은 단순하게)
    sd.Windowed     = TRUE;
    sd.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;     // 현대 윈도우 권장 방식

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;   // 디버그 레이어(잘못된 사용 시 경고) — 학습에 유용
#endif

    D3D_FEATURE_LEVEL want = D3D_FEATURE_LEVEL_11_0;

    // Device + ImmediateContext + SwapChain 을 한 번에 생성.
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
        &want, 1, D3D11_SDK_VERSION, &sd,
        &m_swapChain, &m_device, nullptr, &m_context);

#ifdef _DEBUG
    // 디버그 레이어(그래픽 도구)가 설치 안 된 PC도 있으므로, 실패하면 디버그 없이 재시도.
    if (FAILED(hr)) {
        flags &= ~D3D11_CREATE_DEVICE_DEBUG;
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
            &want, 1, D3D11_SDK_VERSION, &sd,
            &m_swapChain, &m_device, nullptr, &m_context);
    }
#endif
    Hr(hr, "D3D11CreateDeviceAndSwapChain");

    CreateBackbufferRTV();
    return true;
}

// 백버퍼 텍스처를 얻어 "렌더 타깃 뷰(RTV)"를 만든다. (여기에 그림을 그림)
void GraphicsDevice::CreateBackbufferRTV() {
    ComPtr<ID3D11Texture2D> backbuffer;
    Hr(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backbuffer)), "SwapChain GetBuffer");
    Hr(m_device->CreateRenderTargetView(backbuffer.Get(), nullptr, &m_backbufferRTV),
       "CreateRenderTargetView(backbuffer)");
}

void GraphicsDevice::Resize(uint32_t width, uint32_t height) {
    if (!m_swapChain || width == 0 || height == 0)
        return;
    m_width = width;
    m_height = height;

    // 백버퍼 크기를 바꾸려면 먼저 그것을 가리키던 뷰를 모두 풀어야 한다.
    m_context->OMSetRenderTargets(0, nullptr, nullptr);
    m_backbufferRTV.Reset();

    Hr(m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0), "ResizeBuffers");
    CreateBackbufferRTV();
}

void GraphicsDevice::ClearBackbuffer(float r, float g, float b, float a) {
    ID3D11RenderTargetView* rtv = m_backbufferRTV.Get();
    m_context->OMSetRenderTargets(1, &rtv, nullptr);   // "여기에 그려라"

    // 뷰포트: 클립공간(-1~1)을 실제 픽셀 영역으로 매핑하는 사각형.
    D3D11_VIEWPORT vp{};
    vp.Width    = static_cast<float>(m_width);
    vp.Height   = static_cast<float>(m_height);
    vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);

    const float color[4] = { r, g, b, a };
    m_context->ClearRenderTargetView(rtv, color);
}

void GraphicsDevice::Present(bool vsync) {
    m_swapChain->Present(vsync ? 1 : 0, 0);
}

} // namespace core
