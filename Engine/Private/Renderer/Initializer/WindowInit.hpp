#pragma once
#include <iostream>
#include <Windows.h>


/**
 * @brief Initialize window to be drawn on. maybe should be related to input manager
 * 
 */
class WindowInit{

    

    private:

        WNDCLASSEX wc = {0}; // config structure for the windows class
                        // wc = windows class , not the toilet version
        
        MSG msg;
       const static inline auto pClassName = "direct3d";



    public:




    /**
     * @brief Create a Process object
     * 
     * @param hWnd 
     * @param wParam 
     * @param lParam 
     * @return LRESULT 
     */
    static LRESULT CALLBACK createProcess(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief register Window Class for use when creating the window
     * 
     * @param hInstance 
     * @param WndProc 
     */
    void registerWindowClass(HINSTANCE hInstance, WNDPROC WndProc);

    /**
     * @brief Create a Window object to be opened in openWindow()
     * 
     * @param pClassName 
     * @param caption 
     * @return HWND 
     */
    HWND createWindow(auto pClassName,  LPCSTR caption, HINSTANCE hInstance);

    /**
     * @brief open the window created by createWindow and keep it persistent until the user clicks the close button
     * 
     * @param hWnd 
     */
    void showWindow(HWND hWnd);

    
    int openWindow(HINSTANCE hInstance, LPSTR lpCmdLine);


};