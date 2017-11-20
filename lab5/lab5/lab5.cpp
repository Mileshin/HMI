#include <Windows.h>
#include <iostream>
#define B_OPEN				0
#define B_CHANGE_2_ENGINEER 1
#define B_CHANGE_2_STD		2
#define B_CLOSE				3

struct BUTTONSIZE {
	int width,
		height;
};

wchar_t prName[] = L"CalcModeChanger";
PROCESS_INFORMATION piCalc;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
RECT rMain;
BUTTONSIZE bs = { 85, 26 };

int WINAPI WinMain(HINSTANCE hInstance,  // A handle to the current instance of the application.
	HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	WNDCLASS w;
	// Set window attributes
	w.lpszClassName = prName;
	w.hInstance = hInstance;  // A handle to the instance that contains the window procedure for the class
	w.lpfnWndProc = WndProc;
	w.hCursor = LoadCursor(NULL, IDC_ARROW);
	w.hIcon = 0;
	w.lpszMenuName = 0;
	w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	w.style = CS_HREDRAW | CS_VREDRAW; // Redraw the entire window when changing the height or width
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;

	// Register window class
	if (!RegisterClass(&w)) {
		MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
		return NULL;
	}

	// Create window
	HWND hWnd = CreateWindow(prName, // Class name
		L"lab 5", // window name
		WS_SYSMENU, 400, // The style of the window being created
		200, bs.width * 2 + 30, // position
		bs.height * 2 + 70, NULL, // width and height
		NULL,
		hInstance, // A handle to the instance of the module to be associated with the window
		NULL);

	if (!hWnd) {
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK);
		return NULL;
	}

	// Show window, start handling his messages
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DestroyWindow(hWnd);
	UnregisterClass(prName, NULL);
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam){
	// visibles
	static HWND hbOPEN = NULL,
		hbClose = NULL,
		hbEng = NULL,
		hbStd = NULL;

	switch (messg){
	case WM_CREATE:
		// create buttons
		hbOPEN = CreateWindow(L"button", L"Run calc", WS_CHILD | WS_BORDER | WS_VISIBLE, 5, 10, bs.width, bs.height, hWnd, (HMENU)B_OPEN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hbEng = CreateWindow(L"button", L"Engineering", WS_CHILD | WS_BORDER | WS_VISIBLE, 5 * 2 + bs.width, 10 * 2 + bs.height, bs.width, bs.height, hWnd, (HMENU)B_CHANGE_2_ENGINEER, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hbStd = CreateWindow(L"button", L"Standart", WS_CHILD | WS_BORDER | WS_VISIBLE, 5, 10 * 2 + bs.height, bs.width, bs.height, hWnd, (HMENU)B_CHANGE_2_STD, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hbClose = CreateWindow(L"button", L"Close calc", WS_CHILD | WS_BORDER | WS_VISIBLE, 5 * 2 + bs.width, 10, bs.width, bs.height, hWnd, (HMENU)B_CLOSE, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		EnableWindow(hbEng, false);
		EnableWindow(hbStd, false);
		EnableWindow(hbClose, false);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case B_OPEN: 
			// path to calculator
			ShellExecute(hWnd, L"open", L"C:\\Windows\\System32\\calc.exe", NULL, NULL, SW_SHOWNORMAL);
			GetWindowRect(hWnd, &rMain);
			SetWindowPos(FindWindow(L"CalcFrame", NULL), NULL, rMain.right, rMain.top, NULL, NULL, SWP_NOOWNERZORDER || SWP_NOSIZE);
			EnableWindow(hbOPEN, false);
			EnableWindow(hbEng, true);
			EnableWindow(hbStd, true);
			EnableWindow(hbClose, true);
			break;
		case B_CLOSE:
			PostMessage(FindWindow(L"CalcFrame", NULL), WM_CLOSE, 0, 0);
			EnableWindow(hbOPEN, true);
			EnableWindow(hbEng, false);
			EnableWindow(hbStd, false);
			EnableWindow(hbClose, false);
			break;
		case B_CHANGE_2_ENGINEER:
			PostMessage(FindWindow(L"CalcFrame", NULL), WM_USER, 0x130, 0x0);
			break;
		case B_CHANGE_2_STD:
			PostMessage(FindWindow(L"CalcFrame", NULL), WM_USER, 0x131, 0x0);
			break;
		}
		break;

	case WM_DESTROY:
		PostMessage(FindWindow(L"CalcFrame", NULL), WM_CLOSE, 0, 0);
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hWnd, messg, wParam, lParam));
}
