#pragma once

#include <Windows.h>
#include <cstdint>
#include <functional>

namespace app {

class Input;   // 입력 메시지를 전달할 대상 (선택)

// =============================================================================
// Win32Window
//   OS 창(window) 하나를 담당하는 클래스. (아직 그래픽 API와 무관 — 순수 Win32)
//
//   하는 일:
//     1) 창 생성        (RegisterClass + CreateWindow)
//     2) 메시지 처리    (WndProc)
//     3) 게임 루프용 메시지 펌프 (PumpMessages)
//
//   핵심 패턴:
//     - Win32의 WndProc는 "C 스타일 정적 함수"여야 합니다(멤버 함수 불가).
//       그래서 HWND에 this 포인터를 저장해두고, 정적 thunk → 멤버 함수로 위임합니다.
// =============================================================================
class Win32Window {
public:
    // 리사이즈 콜백: (새 width, 새 height) — Part 3에서 스왑체인 재생성에 연결됩니다.
    using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
    // WndProc 메시지 후크 (예: ImGui, Part 6). true 반환 시 메시지가 소비됨.
    using MessageHook = std::function<bool(HWND, UINT, WPARAM, LPARAM)>;

    Win32Window() = default;
    ~Win32Window();

    Win32Window(const Win32Window&) = delete;
    Win32Window& operator=(const Win32Window&) = delete;

    // 창 생성. 실패하면 false.
    bool Create(const wchar_t* title, uint32_t width, uint32_t height);

    // 큐에 쌓인 OS 메시지를 전부 처리한다.
    // 창이 닫히면(WM_QUIT) false 반환 → 메인 루프 종료 신호.
    bool PumpMessages();

    void SetTitle(const wchar_t* title) { if (m_hwnd) SetWindowTextW(m_hwnd, title); }

    HWND     Handle() const { return m_hwnd; }
    uint32_t Width()  const { return m_width; }
    uint32_t Height() const { return m_height; }
    float    Aspect() const { return m_height ? float(m_width) / float(m_height) : 1.0f; }

    void SetOnResize(ResizeCallback cb)   { m_onResize = std::move(cb); }
    void SetInput(Input* input)           { m_input = input; }
    void SetMessageHook(MessageHook hook) { m_msgHook = std::move(hook); }

private:
    // 정적 thunk: HWND에 저장한 this 로 멤버 HandleMessage 에 위임
    static LRESULT CALLBACK WndProcThunk(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    HWND           m_hwnd   = nullptr;
    uint32_t       m_width  = 0;
    uint32_t       m_height = 0;
    ResizeCallback m_onResize;
    MessageHook    m_msgHook;
    Input*         m_input  = nullptr;
};

} // namespace app
