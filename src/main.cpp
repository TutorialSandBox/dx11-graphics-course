// =============================================================================
// main.cpp — 프로그램 진입점.
//   엔진(Application)을 만들고, 초기화하고, 메인 루프를 돌립니다.
//   `--smoke N` 인자를 주면 N 프레임만 돌고 자동 종료(빌드 검증용).
// =============================================================================

#include "app/Application.h"
#include <cstring>
#include <cstdlib>

int main(int argc, char** argv) {
    int maxFrames = -1;   // 기본: 사용자가 닫을 때까지 무한 루프
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--smoke") == 0 && i + 1 < argc)
            maxFrames = std::atoi(argv[++i]);
    }

    app::Application app;
    if (!app.Initialize())
        return 1;
    app.Run(maxFrames);
    return 0;
}
