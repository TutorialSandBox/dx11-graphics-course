#pragma once

#include "core/DxCommon.h"
#include <cstdint>

namespace core {

// =============================================================================
// GBuffer — 디퍼드 렌더링의 핵심 저장소.
//   지오메트리 패스에서 화면의 각 픽셀에 대해 다음을 텍스처로 "기록"합니다:
//     0: albedo   (RGBA8)    — 표면 색 (a=1: 물체 있음 마스크)
//     1: normal   (RGBA8)    — 월드 법선 ([-1,1]→[0,1]로 저장)
//     2: position (RGBA16F)  — 월드 좌표 (정밀도 위해 16비트 float)
//   + 깊이 버퍼(DSV)
//   라이팅 패스는 이 텍스처들을 SRV로 읽어 조명을 한 번에 계산합니다.
// =============================================================================
class GBuffer {
public:
    static constexpr int kTargetCount = 3;

    void Initialize(ID3D11Device* device, uint32_t width, uint32_t height);
    void Resize(ID3D11Device* device, uint32_t width, uint32_t height);

    // 지오메트리 패스 대상으로 바인딩 + 클리어 (3 RTV + 깊이).
    void BindAndClear(ID3D11DeviceContext* ctx);

    // 라이팅 패스에서 읽을 SRV 3개 (albedo/normal/position 순).
    ID3D11ShaderResourceView* const* SRVs() const { return m_srvPtrs; }

private:
    void Create(ID3D11Device* device);

    uint32_t m_width = 0, m_height = 0;
    ComPtr<ID3D11Texture2D>          m_tex[kTargetCount];
    ComPtr<ID3D11RenderTargetView>   m_rtv[kTargetCount];
    ComPtr<ID3D11ShaderResourceView> m_srv[kTargetCount];
    ID3D11ShaderResourceView*        m_srvPtrs[kTargetCount] = {};
    ComPtr<ID3D11Texture2D>          m_depthTex;
    ComPtr<ID3D11DepthStencilView>   m_dsv;
};

} // namespace core
