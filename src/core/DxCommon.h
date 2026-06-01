#pragma once

// =============================================================================
// DxCommon.h — DX11 공통 도우미.
//   · ComPtr: COM 객체(ID3D11*)를 자동으로 Release 해주는 스마트 포인터.
//   · Hr():   HRESULT 실패 시 예외를 던져 바로 알 수 있게 함.
// =============================================================================

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <stdexcept>
#include <string>

// COM 스마트 포인터. obj.Get(), &obj, obj.Reset() 등으로 사용.
template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// DX11 함수는 대부분 HRESULT를 반환합니다. 실패(음수)면 예외로 즉시 중단.
inline void Hr(HRESULT hr, const char* what) {
    if (FAILED(hr)) {
        char buf[256];
        std::snprintf(buf, sizeof(buf), "%s 실패 (HRESULT=0x%08lX)", what, static_cast<unsigned long>(hr));
        throw std::runtime_error(buf);
    }
}
