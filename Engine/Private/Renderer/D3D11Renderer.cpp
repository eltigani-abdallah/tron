#include <iostream>
#include <Windows.h>


int CALLBACK WinMain(
    HINSTANCE hInstance, 
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow
                    )
    {

    const auto pClassName = "classname";

    WNDCLASSEX wc = {0};
    
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = pClassName;
    wc.hIconSm = nullptr;


    RegisterClassEx(&wc);

    HWND hWnd  = CreateWindowEx(0,
         pClassName,
         "window is open, weeeee",
         WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
         200,200, // window position (x, y)
         640,480, // window size (width, height)
         nullptr,
         nullptr,
         hInstance,
         nullptr
         );


    ShowWindow(hWnd, SW_SHOW);

    while (true);

    return 0;
}