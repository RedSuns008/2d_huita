

#ifndef UNICODE
#define UNICODE
#endif 
#pragma comment(linker, "/ENTRY:wWinMainCRTStartup")
#include <windows.h>

// ���������� ���������� ��� �����������
HBITMAP g_hBackgroundBitmap = NULL;

// �������� ������� ��� ��������� �����������
void DrawBackground(HDC hdc, int width, int height, HBITMAP hBitmap);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR pCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);

    // �������� �������� ����������� (����������� ���� ����)
    g_hBackgroundBitmap = (HBITMAP)LoadImageW(
        NULL,
        L"phon1.bmp", // �������� �� ���� ���� � �����
        IMAGE_BITMAP,
        0, 0,
        LR_LOADFROMFILE
    );

    // ���� �� ������� ��������� - ������� ����� ��� ��� �������
    if (!g_hBackgroundBitmap) {
        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        g_hBackgroundBitmap = CreateCompatibleBitmap(hdcScreen, 800, 600);
        SelectObject(hdcMem, g_hBackgroundBitmap);

        // �������� ����� ������
        HBRUSH blueBrush = CreateSolidBrush(RGB(23, 120, 250));
        RECT rect = { 0, 0, 1920, 1080 };
        FillRect(hdcMem, &rect, blueBrush);

        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        DeleteObject(blueBrush);
    }

    // ����������� ������ ����
    const wchar_t CLASS_NAME[] = L"SampleWindowClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL; // ��� ����� �������� �������

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_OK);
        return 0;
    }

    // �������� ����
    HWND hwnd = CreateWindowW(
        CLASS_NAME,
        L"����� �� ����",
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

    // ���� ���������
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ������������ ��������
    if (g_hBackgroundBitmap) DeleteObject(g_hBackgroundBitmap);

    return (int)msg.wParam;
}

// ������� ��� ��������� ����
void DrawBackground(HDC hdc, int width, int height, HBITMAP hBitmap)
{
    // ������� �������� ��� �������
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // �������� ������� �������
    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);

    // ����������� ����������� �� ��� �������
    StretchBlt(
        hdc,       // ������� ��������
        0, 0,      // ������ ���������
        width,     // ������ �������
        height,    // ������ �������
        hdcMem,    // �������� � ������������
        0, 0,      // ������ � ���������
        bm.bmWidth, // ������ ���������
        bm.bmHeight,// ������ ���������
        SRCCOPY    // ������� �����������
    );

    // ��������������� �������� � ������� ���
    SelectObject(hdcMem, hOldBitmap);
    DeleteDC(hdcMem);
}

// ������� ���������
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

        // �������� ������� ���������� �������
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // ������ ������� �����������
        if (g_hBackgroundBitmap) {
            DrawBackground(
                hdc,
                clientRect.right,
                clientRect.bottom,
                g_hBackgroundBitmap
            );
        }

        // ����� ������ �����������
        SetBkMode(hdc, TRANSPARENT); // ���������� ��� ������
        SetTextColor(hdc, RGB(255, 255, 0)); // ������ �����

        TextOutW(hdc, 50, 50, L"Figering, Huila!", 16);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        // �������� ����������� ��������� ����
        return 1;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}





