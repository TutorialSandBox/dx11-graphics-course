#include "app/Win32Window.h"
#include "app/Input.h"
#include <windowsx.h>   // GET_X_LPARAM / GET_Y_LPARAM

namespace app {

static const wchar_t* kWindowClassName = L"MiniEngineWindowClass";

Win32Window::~Win32Window() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    UnregisterClassW(kWindowClassName, GetModuleHandleW(nullptr));
}

bool Win32Window::Create(const wchar_t* title, uint32_t width, uint32_t height) {
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    // 1) 윈도우 클래스 등록 — 이 창의 "타입"을 OS에 알림(어떤 함수가 메시지를 처리할지 등)
    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = &Win32Window::WndProcThunk;   // 메시지 처리 함수(정적)
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWindowClassName;
    RegisterClassExW(&wc);

    // 2) 원하는 "클라이언트 영역(그림 그릴 안쪽)"이 width x height 가 되도록 전체 창 크기 보정
    //    (제목표시줄/테두리 두께만큼 더 커야 안쪽이 정확히 원하는 크기가 됨)
    RECT rc{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    m_width  = width;
    m_height = height;

    // 3) 창 생성 — 마지막 인자로 this 를 넘겨, WndProc에서 다시 받아 멤버 함수로 위임
    m_hwnd = CreateWindowExW(
        0, kWindowClassName, title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInst, this);

    if (!m_hwnd)
        return false;

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    return true;
}

bool Win32Window::PumpMessages() {
    MSG msg{};
    // 큐에 있는 메시지를 모두 비운다.
    //   게임 루프는 멈추면 안 되므로, 블로킹하는 GetMessage 대신 PeekMessage(PM_REMOVE)를 씀.
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            return false;                  // 종료 신호
        TranslateMessage(&msg);
        DispatchMessageW(&msg);            // → WndProcThunk 로 전달
    }
    return true;
}

LRESULT CALLBACK Win32Window::WndProcThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // 창 생성(WM_NCCREATE) 시점에 this 포인터를 HWND의 USERDATA 슬롯에 저장해 둔다.
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }

    auto* self = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (self) {
        self->m_hwnd = hwnd;
        return self->HandleMessage(msg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT Win32Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    // 후크(예: ImGui, Part 6)가 먼저 메시지를 가로챌 기회. UI 위에서의 입력은 여기서 소비됨.
    if (m_msgHook && m_msgHook(m_hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED) {
            m_width  = LOWORD(lParam);
            m_height = HIWORD(lParam);
            if (m_onResize)
                m_onResize(m_width, m_height);
        }
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {         // ESC 로 종료 (개발 편의)
            PostQuitMessage(0);
            return 0;
        }
        if (m_input) m_input->OnKeyDown(static_cast<int>(wParam));
        return 0;

    case WM_KEYUP:
        if (m_input) m_input->OnKeyUp(static_cast<int>(wParam));
        return 0;

    case WM_MOUSEMOVE:
        if (m_input) m_input->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    case WM_RBUTTONDOWN:
        if (m_input) m_input->OnRightButton(true);
        SetCapture(m_hwnd);                // 창 밖으로 드래그해도 마우스 추적 유지
        return 0;

    case WM_RBUTTONUP:
        if (m_input) m_input->OnRightButton(false);
        ReleaseCapture();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(m_hwnd, msg, wParam, lParam);
}

} // namespace app
