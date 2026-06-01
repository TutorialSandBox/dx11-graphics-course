// =============================================================================
// main.cpp — 프로그램 진입점.
//   엔진(Application)을 만들고, 초기화하고, 메인 루프를 돌립니다.
//   검증용 인자:
//     --smoke N        : N 프레임만 돌고 자동 종료
//     --capture PATH   : 마지막 프레임을 BMP로 저장 (렌더 결과 확인)
//   초기화 실패 시 메시지 박스로 원인을 보여줍니다.
// =============================================================================

#include "app/Application.h"
#include <Windows.h>
#include <cstring>
#include <cstdlib>
#include <string>

static std::wstring Widen(const char* s) {
    return std::wstring(s, s + std::strlen(s));
}

int main(int argc, char** argv) {
    int maxFrames = -1;
    std::wstring capturePath;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--smoke") == 0 && i + 1 < argc)
            maxFrames = std::atoi(argv[++i]);
        else if (std::strcmp(argv[i], "--capture") == 0 && i + 1 < argc)
            capturePath = Widen(argv[++i]);
    }

    try {
        app::Application app;
        if (!app.Initialize())
            return 1;
        app.Run(maxFrames, capturePath.empty() ? nullptr : capturePath.c_str());
    } catch (const std::exception& e) {
        // DX11 초기화/셰이더 컴파일 실패 등을 사용자에게 알림
        MessageBoxA(nullptr, e.what(), "MiniEngine 오류", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}
