//-------------------------------------------------------------------------------//

#ifndef UNICODE
#define UNICODE
#endif 
#pragma comment(linker, "/ENTRY:wWinMainCRTStartup")
#include <windows.h>

// Глобальные переменные
HBITMAP g_hBackgroundBitmap = NULL;
HBITMAP g_hBufferBitmap = NULL;

//------------------------------------------------------------------------------//Структура окна
struct Window {
    HWND hWnd;
    HDC device_context;
    HDC context;
    int width, height;
};
Window window;
// Класс для отслеживания состояния мыши
class Mouse_ {
public:
    float x, y;
    bool L_butt, R_butt;

    // Обновление состояния мыши
    void Update() {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(window.hWnd, &p);
        x = static_cast<float>(p.x);
        y = static_cast<float>(p.y);
        L_butt = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        R_butt = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    }
};
Mouse_ Mouse;  

    RECT butt_close = { 0, 30, 0 , 80 };
    bool g_btnHover = false;

// -------------------------------------------------------------------------------//Прототипы функций
void DrawBackground(HDC hdc, int width, int height, HBITMAP hBitmap);
void DrawCloseButton(HDC hdc, bool hover = false);
bool CheckCollisionMouse();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR pCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);


    window.width = GetSystemMetrics(SM_CXSCREEN);
    window.height = GetSystemMetrics(SM_CYSCREEN);

    butt_close.left = window.width- 300;
    butt_close.right = window.width - 200;


    g_hBackgroundBitmap = (HBITMAP)LoadImageW(NULL,L"phon1.bmp",IMAGE_BITMAP,0, 0,LR_LOADFROMFILE);

    if (!g_hBackgroundBitmap) {
        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        g_hBackgroundBitmap = CreateCompatibleBitmap(hdcScreen, window.width, window.height);
        SelectObject(hdcMem, g_hBackgroundBitmap);

        HBRUSH blueBrush = CreateSolidBrush(RGB(23, 120, 250));
        RECT rect = { 0, 0, window.width, window.height };
        FillRect(hdcMem, &rect, blueBrush);

        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        DeleteObject(blueBrush);
    }

    // Регистрация класса окна
    const wchar_t CLASS_NAME[] = L"SampleWindowClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_OK);
        return 0;
    }

    // Создание окна
    window.hWnd = CreateWindowW(
        CLASS_NAME,
        L"ХУита на паре",
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window.width, window.height,
        NULL, NULL, hInstance, NULL
    );

    if (!window.hWnd)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_OK);
        return 0;
    }

    // Инициализация контекстов
    HDC hdcScreen = GetDC(window.hWnd);
    window.context = CreateCompatibleDC(hdcScreen);
    g_hBufferBitmap = CreateCompatibleBitmap(hdcScreen, window.width, window.height);
    SelectObject(window.context, g_hBufferBitmap);
    ReleaseDC(window.hWnd, hdcScreen);

    ShowWindow(window.hWnd, nCmdShow);
    UpdateWindow(window.hWnd);

    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        // Обновляем состояние мыши перед обработкой сообщения
        Mouse.Update();

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Освобождение ресурсов
    if (g_hBackgroundBitmap) DeleteObject(g_hBackgroundBitmap);
    if (g_hBufferBitmap) DeleteObject(g_hBufferBitmap);
    if (window.context) DeleteDC(window.context);

    return (int)msg.wParam;
}

void DrawBackground(HDC hdc, int width, int height, HBITMAP hBitmap)
{
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);

    StretchBlt(
        hdc, 0, 0, width, height,
        hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY
    );

    SelectObject(hdcMem, hOldBitmap);
    DeleteDC(hdcMem);
}

void DrawCloseButton(HDC hdc, bool hover)
{
    int prevBkMode = SetBkMode(hdc, TRANSPARENT);
    COLORREF prevTextColor = SetTextColor(hdc, RGB(255, 255, 255));
    HBRUSH prevBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    // Рисуем прямоугольник кнопки
    if (hover) {
        HBRUSH hoverBrush = CreateSolidBrush(RGB(255, 180, 230));
        FillRect(hdc, &butt_close, hoverBrush);
        DeleteObject(hoverBrush);
    }
    else {
        HBRUSH normalBrush = CreateSolidBrush(RGB(200, 50, 50));
        FillRect(hdc, &butt_close, normalBrush);
        DeleteObject(normalBrush);
    }

    //// Рисуем крестик
    //HPEN pen = CreatePen(PS_SOLID, 5, RGB(255, 255, 255));
    //HPEN prevPen = (HPEN)SelectObject(hdc, pen);

    //MoveToEx(hdc, butt_close.left + 30, butt_close.top + 30, NULL);
    //LineTo(hdc, butt_close.right - 30, butt_close.bottom - 30);

    //MoveToEx(hdc, butt_close.right - 30, butt_close.top + 30, NULL);
    //LineTo(hdc, butt_close.left + 30, butt_close.bottom - 30);

    const wchar_t* text = L"Закрыть";
    DrawTextW(hdc, text, -1, &butt_close, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    //SelectObject(hdc, prevPen);
    SelectObject(hdc, prevBrush);
    SetTextColor(hdc, prevTextColor);
    SetBkMode(hdc, prevBkMode);
    //DeleteObject(pen);
}

// Проверка коллизии TODO 
bool CheckCollisionMouse()
{
    return (Mouse.x >= butt_close.left && Mouse.x <= butt_close.right &&
        Mouse.y >= butt_close.top && Mouse.y <= butt_close.bottom);
}

// Оконная процедура с исправлениями
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        window.device_context = BeginPaint(hwnd, &ps);
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // Рисуем фон в буфер
        if (g_hBackgroundBitmap) {
            DrawBackground(
                window.context,
                clientRect.right,
                clientRect.bottom,
                g_hBackgroundBitmap
            );
        }

        //// Текст в буфер
        //SetBkMode(window.context, TRANSPARENT);
        //SetTextColor(window.context, RGB(255, 255, 0));
        //TextOutW(window.context, 50, 50, L"Figering, Huila!", 16);

        // Рисуем кнопку закрытия
        DrawCloseButton(window.context, g_btnHover);

        // Копируем буфер на экран
        BitBlt(
            window.device_context,
            0, 0,
            clientRect.right, clientRect.bottom,
            window.context,
            0, 0,
            SRCCOPY
        );

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:
        // Используем состояние мыши из объекта Mouse
        if (CheckCollisionMouse()) {
            DestroyWindow(hwnd);
        }
        return 0;

    case WM_MOUSEMOVE:
    {
        // Исправление: объявляем переменные внутри блока {}
        bool wasHover = g_btnHover;
        g_btnHover = CheckCollisionMouse();

        // Перерисовываем только если состояние изменилось
        if (g_btnHover != wasHover) {
            InvalidateRect(hwnd, &butt_close, FALSE);
        }
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}