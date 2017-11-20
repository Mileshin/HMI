#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK kbProc(INT, WPARAM, LPARAM);

LRESULT __declspec(dllexport)__stdcall  CALLBACK KeyboardProc(int, WPARAM, LPARAM);

void moveWindow(HWND, bool, RECT, RECT);