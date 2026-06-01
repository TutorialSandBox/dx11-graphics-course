#include "render/ShaderUtil.h"
#include <d3dcompiler.h>
#include <Windows.h>

namespace render {

ComPtr<ID3DBlob> CompileShaderFromFile(const std::wstring& path,
                                       const char* entryPoint,
                                       const char* target) {
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // 디버그 빌드: 최적화 끄고 디버그 정보 포함 → 셰이더 디버깅 가능
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> code, errors;
    HRESULT hr = D3DCompileFromFile(
        path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint, target, flags, 0, &code, &errors);

    // 컴파일 에러/경고 메시지는 디버그 출력 창(VS '출력')에 그대로 보여줌.
    if (errors)
        OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));

    Hr(hr, "D3DCompileFromFile");
    return code;
}

} // namespace render
