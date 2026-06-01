#pragma once

#include "core/DxCommon.h"
#include <string>

namespace render {

// =============================================================================
// Texture — DDS 이미지 파일을 GPU 텍스처로 로드한다.
//   (비압축 32비트 DDS만 지원 — 학습용으로 충분. test.dds 가 그 형식)
//   DX11은 DX12와 달리 업로드 힙/배리어가 필요 없습니다:
//   CreateTexture2D 에 초기 데이터를 넘기면 끝.
// =============================================================================
class Texture {
public:
    void LoadDDS(ID3D11Device* device, const std::wstring& path);
    ID3D11ShaderResourceView* SRV() const { return m_srv.Get(); }

private:
    ComPtr<ID3D11Texture2D>          m_texture;
    ComPtr<ID3D11ShaderResourceView> m_srv;
    uint32_t m_width = 0, m_height = 0;
};

} // namespace render
