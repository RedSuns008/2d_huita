

#ifndef UNICODE
#define UNICODE
#endif 
#pragma comment(linker, "/ENTRY:wWinMainCRTStartup")
#include <windows.h>

// Глобальные переменные для изображения
HBITMAP g_hBackgroundBitmap = NULL;

// Прототип функции для отрисовки изображения
void DrawBackground(HDC hdc, int width, int height, HBITMAP hBitmap);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR pCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);

    // Загрузка фонового изображения (используйте свой путь)
    g_hBackgroundBitmap = (HBITMAP)LoadImageW(
        NULL,
        L"phon1.bmp", // Замените на свой путь к файлу
        IMAGE_BITMAP,
        0, 0,
        LR_LOADFROMFILE
    );

    // Если не удалось загрузить - создаем синий фон для примера
    if (!g_hBackgroundBitmap) {
        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        g_hBackgroundBitmap = CreateCompatibleBitmap(hdcScreen, 800, 600);
        SelectObject(hdcMem, g_hBackgroundBitmap);

        // Заливаем синим цветом
        HBRUSH blueBrush = CreateSolidBrush(RGB(23, 120, 250));
        RECT rect = { 0, 0, 1920, 1080 };
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
    wc.hbrBackground = NULL; // Фон будем рисовать вручную

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_OK);
        return 0;
    }

    // Создание окна
    HWND hwnd = CreateWindowW(
        CLASS_NAME,
        L"ХУита на паре",
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Освобождение ресурсов
    if (g_hBackgroundBitmap) DeleteObject(g_hBackgroundBitmap);

    return (int)msg.wParam;
}

// Функция для отрисовки фона
void DrawBackground(HDC hdc, int width, int height, HBITMAP hBitmap)
{
    // Создаем контекст для битмапа
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // Получаем размеры битмапа
    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);

    // Растягиваем изображение на всю область
    StretchBlt(
        hdc,       // Целевой контекст
        0, 0,      // Начало рисования
        width,     // Ширина области
        height,    // Высота области
        hdcMem,    // Контекст с изображением
        0, 0,      // Начало в источнике
        bm.bmWidth, // Ширина источника
        bm.bmHeight,// Высота источника
        SRCCOPY    // Простое копирование
    );

    // Восстанавливаем контекст и удаляем его
    SelectObject(hdcMem, hOldBitmap);
    DeleteDC(hdcMem);
}

// Оконная процедура
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
        HDC hdc = BeginPaint(hwnd, &ps);

        // Получаем размеры клиентской области
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // Рисуем фоновое изображение
        if (g_hBackgroundBitmap) {
            DrawBackground(
                hdc,
                clientRect.right,
                clientRect.bottom,
                g_hBackgroundBitmap
            );
        }

        // Текст поверх изображения
        SetBkMode(hdc, TRANSPARENT); // Прозрачный фон текста
        SetTextColor(hdc, RGB(255, 255, 0)); // Желтый текст

        TextOutW(hdc, 50, 50, L"Figering, Huila!", 16);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        // Отменяем стандартную отрисовку фона
        return 1;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}





