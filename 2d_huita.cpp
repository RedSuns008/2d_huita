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



//void ShowBitmap(int x, int y, int width, int height, HBITMAP hBitmap, bool alpha) {
//    HDC hMemDC = CreateCompatibleDC(window.context);
//    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
//    BITMAP bm;
//
//    if (hOldBitmap) {
//        GetObject(hBitmap, sizeof(HBITMAP), &bm);
//        if (alpha) {
//            TransparentBlt(window.context, x, y, width, height, hMemDC, NULL, NULL, width, height, RGB(0, 0, 0));
//        }
//        else {
//            StretchBlt(window.context, x, y, width, height, hMemDC, NULL, NULL, bm.bmWidth, bm.bmHeight, SRCCOPY);
//        }
//        SelectObject(hMemDC, hOldBitmap);
//    }
//    DeleteDC(hMemDC);
//}

 void ShowBitmap(int x, int y, int width, int height, HBITMAP hBitmap, bool alpha);

HBITMAP LoadBMP(const char* name)
{
    return (HBITMAP)LoadImageA(NULL, name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

class Button {
public:

    float x, y, width, height;
    HBITMAP hBitmap;
    HBITMAP hBitmapGlow;

    bool CheckCollisionMouse() {
        return Mouse.x < x + width && Mouse.x > x && Mouse.y < y + height && Mouse.y > y;
    }
    void Load(const char* imagename, const char* imagenameglow, float x_, float y_, float w, float h) {
        x = x_; y = y_;
        hBitmap = LoadBMP(imagename);
        hBitmapGlow = LoadBMP(imagenameglow);
        height = h * window.height;
        width = w * window.width;
        x = window.width / 2 - width * x;
        y = window.height / 2 + height * y;
    }
    
    bool Show() {
        bool pw_collision = CheckCollisionMouse();
       
        /*if ((currentTime < healStartTime + healTime) || (AttackcurrentTime < AttackStartTime + AttackTime))
        {
            offset = 0;
        }*/

        ShowBitmap(x, y, width, height, pw_collision ? hBitmapGlow : hBitmap, true);
        return pw_collision;
    }
   
};

Button Exit;
HBITMAP Exit_bmp;

// -------------------------------------------------------------------------------//Прототипы функций
// void DrawBackground(HDC hdc, int width, int height, HBITMAP hBitmap);
// void DrawCloseButton(HDC hdc, bool hover = false);
// bool CheckCollisionMouse();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR pCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);


    window.width = GetSystemMetrics(SM_CXSCREEN);
    window.height = GetSystemMetrics(SM_CYSCREEN);


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

// Оконная процедура с исправлениями
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Exit.Load("Exit_butt.bmp", "Exit_butt.bmp", 12, -16, .04, .03);
    Exit.Show();


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
        SetBkMode(window.context, TRANSPARENT);
        SetTextColor(window.context, RGB(255, 255, 0));
        TextOutW(window.context, 50, 50, L"Figering, Huila!", 16);

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

     case WM_ERASEBKGND:
        return 1;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}