#include "lab4.h"
#define LEFT false
#define RIGHT true
#define PLUS 0xBB
#define MINUS 0xBD
#define PLUS2 0x6B
#define MINUS2 0x6D
#define ESC    0x1B
#define ID_LABEL 1


wchar_t szClassName[] = L"lab 4"; // Class  name
POINT points[4];
COLORREF curColor = RGB(47, 79, 79);
HBRUSH bckBrush = CreateSolidBrush(curColor);
HPEN pen = CreatePen(PS_SOLID, 1, curColor);
int speed = 10;
bool reversed = false;
HWND hWnd;
HHOOK hook;
RECT wRect, dRect;

int WINAPI WinMain(HINSTANCE hInstance, // A handle to the current instance of the application.
			HINSTANCE hPrevInstance, 
			LPSTR lpszCmdLine, 
			int nCmdShow)				// Controls how the window is to be shown.
{
	WNDCLASS w;
	// Set window attributes
	w.lpszClassName = szClassName;
	w.hInstance = hInstance; // A handle to the instance that contains the window procedure for the class
	w.lpfnWndProc = WndProc;
	w.hCursor = LoadCursor(NULL, IDC_ARROW);
	w.hIcon = 0;
	w.lpszMenuName = 0;
	w.hbrBackground = bckBrush;
	w.style = CS_HREDRAW | CS_VREDRAW; // Redraw the entire window when changing the height or width
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;

	// Register window class
	if (!RegisterClass(&w)) {
		MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
		return NULL;
	}

	// Create window
	hWnd = CreateWindow(szClassName, // Class name
		L"Lab 4",    // window name
		WS_POPUP, 0, // The style of the window being created
		0, 300,      // position
		100, NULL,   // width and height
		NULL,
		hInstance,   // A handle to the instance of the module to be associated with the window
		NULL);

	if (!hWnd) {
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK);
		return NULL;
	}
	// Show window, start handling his messages
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hInstance, NULL); // Installs a hook procedure 
																					  // that monitors low-level keyboard input events.
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DestroyWindow(hWnd);
	UnregisterClass(szClassName, NULL);
	return (int)msg.wParam;
}

void updateLabel(HWND owner) {
	TCHAR lpszSTR[30];
	ZeroMemory(lpszSTR, sizeof(lpszSTR));
	wsprintf(lpszSTR, TEXT("Speed = %d\npixel per click"), speed);
	SetDlgItemText(owner, ID_LABEL, lpszSTR);
}

void moveWindow(HWND hWnd, bool Right, RECT wRect, RECT dRect) {
	int oldspeed = speed;
	if (reversed) Right = !Right;
	if (Right) {
		if (wRect.right + speed >= dRect.right) {
			speed = dRect.right - wRect.right;
			reversed = !reversed;
		}
		SetWindowPos(hWnd,
			HWND_TOPMOST,
			wRect.left + speed,
			wRect.top, 
			0,0,
			SWP_NOSIZE);
	}
	else
	{
		if (wRect.left - speed <= dRect.left) {
			speed = -(dRect.left - wRect.left);
			reversed = !reversed;
		}
		SetWindowPos(hWnd,
			HWND_TOPMOST,
			wRect.left - speed,
			wRect.top,
			0,0,
			SWP_NOSIZE);
	}
	if (speed != oldspeed) speed = oldspeed;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HWND dWnd, speedLabel;
	HRGN hRegn;
	GetClientRect(GetDesktopWindow(), &dRect);
	switch (messg)
	{
	case WM_CREATE:
		points[0].x = 250;
		points[0].y = 100;


		points[1].x = 0;
		points[1].y = 100;


		points[3].x = 300;
		points[3].y = 0;


		points[2].x = 50;
		points[2].y = 0;
		GetClientRect(hWnd, &wRect);

		hRegn = CreatePolygonRgn(points, 4, 2);

		SetWindowRgn(hWnd, hRegn, TRUE);
		SetWindowPos(hWnd,
			HWND_TOPMOST,
			(dRect.right - dRect.left - wRect.right + wRect.left) / 2,
			(dRect.bottom - dRect.top - wRect.bottom + wRect.top) / 2,
			0,0, SWP_NOSIZE);
		SetForegroundWindow(hWnd);

		speedLabel = CreateWindow(L"STATIC",
			L"Speed info",
			WS_VISIBLE | WS_CHILD | SS_CENTER,
			(wRect.right - wRect.left) / 3,
			(wRect.bottom - wRect.top) / 3,
			(wRect.right - wRect.left) / 3,
			(wRect.bottom - wRect.top) / 3,
			hWnd,
			(HMENU)ID_LABEL,
			NULL, NULL);
		updateLabel(hWnd);
		if (!speedLabel) {
			MessageBox(NULL, L"Cannot create window Speed info", L"Error", MB_OK);
			return NULL;
		}
		SetTimer(hWnd, 1, 10, NULL); // update rate
		break;
	case WM_TIMER:
		GetWindowRect(hWnd, &wRect);
		moveWindow(hWnd, true, wRect, dRect);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hWnd, messg, wParam, lParam));
}

LRESULT  CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
	GetWindowRect(hWnd, &wRect);
	switch (hooked_key.vkCode)
	{
	case PLUS:
	case PLUS2:
		if (speed < 256)
			speed += 1;
		updateLabel(hWnd);
		break;
	case MINUS:
	case MINUS2:
		if (speed > 0)
			speed -= 1;
		updateLabel(hWnd);
		break;
	case ESC:
		DestroyWindow(hWnd);
		UnregisterClass(szClassName, NULL);
		break;
	default:
		break;
	}
	return CallNextHookEx(hook, nCode, wParam, lParam);
}