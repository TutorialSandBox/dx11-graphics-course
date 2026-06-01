#include "core/GraphicsDevice.h"
#include <vector>
#include <cstdio>

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

// 백버퍼를 CPU로 복사해 32비트 BMP로 저장한다. (검증/디버깅 전용)
//   백버퍼는 GPU 전용이라 바로 못 읽음 → STAGING 텍스처로 복사 후 Map 으로 읽는다.
bool GraphicsDevice::CaptureBackbufferToBMP(const wchar_t* path) {
    ComPtr<ID3D11Texture2D> back;
    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&back)))) return false;

    D3D11_TEXTURE2D_DESC desc{};
    back->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC sdesc = desc;
    sdesc.Usage          = D3D11_USAGE_STAGING;      // CPU가 읽을 수 있는 텍스처
    sdesc.BindFlags      = 0;
    sdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    sdesc.MiscFlags      = 0;
    ComPtr<ID3D11Texture2D> staging;
    if (FAILED(m_device->CreateTexture2D(&sdesc, nullptr, &staging))) return false;

    m_context->CopyResource(staging.Get(), back.Get());   // GPU→CPU 복사

    D3D11_MAPPED_SUBRESOURCE map{};
    if (FAILED(m_context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &map))) return false;

    const uint32_t W = desc.Width, H = desc.Height;
    const uint32_t rowBytes = W * 4;
    const uint32_t imgBytes = rowBytes * H;

#pragma pack(push, 1)
    struct BmpFileHeader { uint16_t type; uint32_t size; uint16_t r1, r2; uint32_t off; };
    struct BmpInfoHeader { uint32_t size; int32_t w, h; uint16_t planes, bpp;
                           uint32_t comp, imgSize; int32_t xppm, yppm; uint32_t clrUsed, clrImp; };
#pragma pack(pop)
    BmpFileHeader fh{ 0x4D42, uint32_t(54 + imgBytes), 0, 0, 54 };
    BmpInfoHeader ih{ 40, int32_t(W), -int32_t(H), 1, 32, 0, imgBytes, 0, 0, 0, 0 };  // h<0 = 위에서 아래로

    FILE* f = nullptr;
    _wfopen_s(&f, path, L"wb");
    if (!f) { m_context->Unmap(staging.Get(), 0); return false; }
    fwrite(&fh, sizeof(fh), 1, f);
    fwrite(&ih, sizeof(ih), 1, f);

    // 백버퍼는 RGBA 순서, BMP는 BGRA 순서 → R/B 교환하며 한 줄씩 기록.
    auto* src = static_cast<const uint8_t*>(map.pData);
    std::vector<uint8_t> row(rowBytes);
    for (uint32_t y = 0; y < H; ++y) {
        const uint8_t* s = src + y * map.RowPitch;
        for (uint32_t x = 0; x < W; ++x) {
            row[x*4+0] = s[x*4+2];  // B
            row[x*4+1] = s[x*4+1];  // G
            row[x*4+2] = s[x*4+0];  // R
            row[x*4+3] = s[x*4+3];  // A
        }
        fwrite(row.data(), rowBytes, 1, f);
    }
    fclose(f);
    m_context->Unmap(staging.Get(), 0);
    return true;
}

} // namespace core
