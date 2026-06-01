#pragma once

#include "core/DxCommon.h"
#include <cstdint>

namespace core {

// =============================================================================
// GraphicsDevice — DX11의 "그래픽 장치"를 한 묶음으로 관리.
//   DX12와 달리 DX11은 이게 아주 단순합니다:
//     · Device           : 자원(버퍼/텍스처/셰이더)을 "만드는" 객체
//     · ImmediateContext : 실제로 "그리라고 명령하는" 객체 (커맨드리스트/펜스/배리어 없음!)
//     · SwapChain        : 화면에 보여줄 백버퍼들을 관리 (Present로 교체)
//     · BackbufferRTV    : 백버퍼를 "렌더 타깃"으로 보는 뷰 (여기에 그림)
// =============================================================================
class GraphicsDevice {
public:
    bool Initialize(HWND hwnd, uint32_t width, uint32_t height);
    void Resize(uint32_t width, uint32_t height);

    // 백버퍼를 단색으로 지우고, 렌더 타깃 + 뷰포트를 바인딩한다.
    void ClearBackbuffer(float r, float g, float b, float a = 1.0f);
    // 다 그린 백버퍼를 화면에 표시 (vsync=true면 수직동기화 대기 → 동기화 역할).
    void Present(bool vsync = true);

    // 현재 백버퍼를 BMP 파일로 저장 (렌더 결과를 눈으로 검증할 때 사용 — Present 전에 호출).
    bool CaptureBackbufferToBMP(const wchar_t* path);

    ID3D11Device*           Device()        const { return m_device.Get(); }
    ID3D11DeviceContext*    Context()       const { return m_context.Get(); }
    ID3D11RenderTargetView* BackbufferRTV() const { return m_backbufferRTV.Get(); }
    uint32_t Width()  const { return m_width; }
    uint32_t Height() const { return m_height; }

private:
    void CreateBackbufferRTV();

    ComPtr<ID3D11Device>           m_device;
    ComPtr<ID3D11DeviceContext>    m_context;       // ImmediateContext
    ComPtr<IDXGISwapChain>         m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_backbufferRTV;
    uint32_t m_width = 0, m_height = 0;
};

} // namespace core
