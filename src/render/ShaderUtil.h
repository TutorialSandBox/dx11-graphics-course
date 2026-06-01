#pragma once

#include "core/DxCommon.h"
#include <string>

namespace render {

// HLSL 파일을 런타임에 컴파일해 바이트코드(blob)로 돌려준다.
//   DX11은 셰이더 모델 5(SM5) DXBC 바이트코드를 씁니다 → target 예: "vs_5_0", "ps_5_0".
//   (DX12에서 쓰던 DXC/SM6 DXIL 은 DX11에서 못 씁니다.)
//   컴파일 에러가 나면 메시지를 디버그 출력에 찍고 예외를 던집니다.
ComPtr<ID3DBlob> CompileShaderFromFile(const std::wstring& path,
                                       const char* entryPoint,
                                       const char* target);

} // namespace render
