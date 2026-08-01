#include <iostream>
#include <Windows.h>


int CALLBACK WinMain(
    HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, 
    int nCmdShow 
    )

    {
    /**
     * @brief entry point for windows API applications
     * @param HINSTANCE hInstance  handle to current instance of application
     * @param HINSTANCE hPrevInstance handle to previous instance of application, always null because it's been deprecated. we don't care about this prick
     * @param LPSTR lpCmdLine command line where the application receives messages
     * @param int nCmdShow how the window should be shown on program startup. we don't care about this guy either
     */

    

    const auto pClassName = "direct3d";

    WNDCLASSEX wc = {0}; // config structure for the windows class
                        // wc = windows class , not the toilet version

    

    wc.cbSize = sizeof(wc); // size of the windows class
    wc.style = CS_OWNDC; // gives the window its own device context for rendering
    wc.lpfnWndProc = DefWindowProc; // funcion to handle messages to the window and how it looks. DefWindow = default window
    wc.cbClsExtra = 0; // allows for allocation of extra bytes to the window class. we need none
    wc.cbWndExtra = 0; // allows for allocation of extra bytes to the child windows. we need 0
    wc.hInstance = hInstance; //  handle to the current instance of the application
    wc.hIcon = nullptr; // window icon. none
    wc.hCursor = nullptr; // special cursor? none
    wc.hbrBackground = nullptr; // window background. no need since we'll be drawing over it with direct3d
    wc.lpszMenuName = nullptr; // no menus so no menu names
    wc.lpszClassName = pClassName; // class name for windows of this type
    wc.hIconSm = nullptr; // custom icon for application. none


    RegisterClassEx(&wc); // registers class for use in window instances

    HWND hWnd  = CreateWindowEx(
         0, // Window Ex styles. none are needed for this project
         pClassName, // window type class name
         "window is open, weeeee", // window caption
         WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // give the window a caption, a minimize box, and allows the system window to be opened on it
         200,200, // window position (x, y)
         640,480, // window size (width, height)
         nullptr, // handle to parent of current window. ain't got none of that
         nullptr, // handle to a menu window. again, none here
         hInstance, // handle to instance of current window
         nullptr // handle to custom parameter. none again
         );


    ShowWindow(hWnd, SW_SHOW); // shows the window after it is created

    while (true); // DELETE LATER an infinite loop to actually see the window

    return 0; 
}