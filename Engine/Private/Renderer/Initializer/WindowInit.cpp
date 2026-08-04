#include "WindowInit.hpp"
// TODO: refactor code into a proper C++ class



LRESULT CALLBACK WindowInit::createProcess(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam){

    

   switch (uMsg){
        case WM_CLOSE:
            PostQuitMessage(69);
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



void WindowInit::registerWindowClass(HINSTANCE hInstance, WNDPROC WndProc){


    wc.cbSize = sizeof(wc); // size of the windows class
    wc.style = CS_OWNDC; // gives the window its own device context for rendering
    wc.lpfnWndProc = WndProc; // funcion to handle messages to the window and how it looks. DefWindow = default window
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

        
    }

HWND WindowInit::createWindow(auto pClassName,  LPCSTR caption, HINSTANCE hInstance) {

    HWND hWnd  = CreateWindowEx(
         0, // Window Ex styles. none are needed for this project
         pClassName, // window type class name
         caption, // window caption
         WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // give the window a caption, a minimize box, and allows the system window to be opened on it
         200,200, // window position (x, y)
         640,480, // window size (width, height)
         nullptr, // handle to parent of current window. ain't got none of that
         nullptr, // handle to a menu window. again, none here
         hInstance, // handle to instance of current window
         nullptr // handle to custom parameter. none again
         );

    return hWnd;
}

void WindowInit::showWindow(HWND hWnd){

    ShowWindow(hWnd, SW_SHOW); // shows the window after it is created


   

}


int WindowInit::openWindow(HINSTANCE hInstance, LPSTR lpCmdLine){



    WindowInit::registerWindowClass(hInstance, WindowInit::createProcess); // register the window class with the appropriate window

    HWND windowHandle = WindowInit::createWindow(pClassName, "refactored window", hInstance); // create window and handle

    WindowInit::showWindow(windowHandle);


    BOOL gResult; // the result of GetMessages, used to debug the exit code

    while ( (gResult = GetMessage(&msg, nullptr, 0,0)) > 0 ) // as long as the message passed from the user is not a message to quit the application...
    {
        TranslateMessage(&msg); // translate virtual key messages to character messages then post them to the thread's message queue
        DispatchMessage(&msg); // dispatch message from GetMessage to the window procedure

    }

    if (gResult == -1){ // if GetMessage gets a -1 message...
        
        std::cout<< msg.wParam<<std::endl;
        return msg.wParam;
    }

    else{ // if it got any other message than -1...

        std::cout<< msg.wParam<<std::endl;
        return msg.wParam;
    }


}


/**
* @brief entry point for windows API applications
* @param HINSTANCE hInstance  handle to current instance of application
* @param HINSTANCE hPrevInstance handle to previous instance of application, always null because it's been deprecated. we don't care about this prick
* @param LPSTR lpCmdLine command line where the application receives messages
* @param int nCmdShow how the window should be shown on program startup. we don't care about this guy either
*/
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )

    {


    WindowInit winInit;

    return winInit.openWindow(hInstance, lpCmdLine);
  
    
   
}