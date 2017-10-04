#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <iostream> 
#include <fstream>

// Human readable signals for window action handlers
#define OPEN			1
#define SAVE_AS			2	
#define APPLY_BITBLT	3
#define APPLY_SETPIXEL	4
#define START_TESTS		5

const int GraphWidth = 1050;
const int GraphHeight = 650;

// Prototypes for functions below
LPCTSTR getFileName();
int saveBitmap(HBITMAP bm);
LRESULT CALLBACK handleWindowEvents(HWND, UINT, WPARAM, LPARAM);
void setPixelDisplay(HWND hwnd, HBITMAP hBitmap);
void setBlt(HWND hWnd, HBITMAP hBitmap);
void runTests(HWND hwnd);
int newWindow(HWND hwnd);
LRESULT CALLBACK childProc(HWND, UINT, WPARAM, LPARAM);

bool isLoaded = false;

HBITMAP hBitmap;
HINSTANCE hinst;
HWND child;
int stats[20];
int max = 0;

#endif 