#include "render/Texture.h"
#include <fstream>
#include <vector>
#include <cstring>

namespace render {

namespace {
constexpr uint32_t DDPF_RGB = 0x40;

#pragma pack(push, 1)
struct DDSPixelFormat {
    uint32_t size, flags, fourCC, rgbBitCount, rMask, gMask, bMask, aMask;
};
struct DDSHeader {
    uint32_t size, flags, height, width, pitchOrLinearSize, depth, mipMapCount;
    uint32_t reserved1[11];
    DDSPixelFormat pf;
    uint32_t caps, caps2, caps3, caps4, reserved2;
};
#pragma pack(pop)
} // namespace

void Texture::LoadDDS(ID3D11Device* device, const std::wstring& path) {
    // 1) 파일 통째로 읽기
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("DDS 파일을 열 수 없습니다");
    const std::streamsize fileSize = file.tellg();
    file.seekg(0);
    std::vector<uint8_t> bytes(static_cast<size_t>(fileSize));
    file.read(reinterpret_cast<char*>(bytes.data()), fileSize);

    // 2) 매직("DDS ") + 헤더 검사
    if (bytes.size() < 4 + sizeof(DDSHeader) || std::memcmp(bytes.data(), "DDS ", 4) != 0)
        throw std::runtime_error("유효한 DDS가 아닙니다");
    const DDSHeader* hdr = reinterpret_cast<const DDSHeader*>(bytes.data() + 4);
    m_width  = hdr->width;
    m_height = hdr->height;

    // 3) 포맷: 비압축 32비트만 지원. R 마스크 위치로 BGRA/RGBA 구분.
    if (!(hdr->pf.flags & DDPF_RGB) || hdr->pf.rgbBitCount != 32)
        throw std::runtime_error("지원하지 않는 DDS (비압축 32비트만)");
    DXGI_FORMAT format = (hdr->pf.rMask == 0x00FF0000) ? DXGI_FORMAT_B8G8R8A8_UNORM
                                                       : DXGI_FORMAT_R8G8B8A8_UNORM;

    const uint8_t* pixels = bytes.data() + 4 + sizeof(DDSHeader);

    // 4) 텍스처 생성 — 초기 데이터를 바로 넘김 (DX11의 단순함!)
    D3D11_TEXTURE2D_DESC td{};
    td.Width      = m_width;
    td.Height     = m_height;
    td.MipLevels  = 1;
    td.ArraySize  = 1;
    td.Format     = format;
    td.SampleDesc.Count = 1;
    td.Usage      = D3D11_USAGE_IMMUTABLE;
    td.BindFlags  = D3D11_BIND_SHADER_RESOURCE;     // 셰이더에서 읽을 자원

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem     = pixels;
    init.SysMemPitch = m_width * 4;                 // 한 줄의 바이트 수
    Hr(device->CreateTexture2D(&td, &init, &m_texture), "CreateTexture2D");

    // 5) 셰이더에서 샘플할 뷰(SRV) 생성
    Hr(device->CreateShaderResourceView(m_texture.Get(), nullptr, &m_srv),
       "CreateShaderResourceView");
}

} // namespace render
