#include <windows.h>
#include <math.h>
#include <cmath>
#include <tchar.h>
#include <strsafe.h>
#include <errno.h>
#include "Button.h"

#define TITLE_TEXTBOX_ID		2987
#define WIDTH_TEXTBOX_ID		2988
#define HEIGHT_TEXTBOX_ID		2989
#define ADD_NEW_BUTTON_ID		2990
#define INFO_TEXTBOX_ID	519
#define MAX_CHARS	16

HWND mainHandle;
HINSTANCE hInstance;

TCHAR buf[MAX_CHARS], title[MAX_CHARS];
int width, height, tmp;
int maxWidth = 170, maxHeight = 170;

LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
void handleTextboxes(WPARAM param);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	WNDCLASS WndClass;
	int offsetV, offsetH;
	int height, width;
	wchar_t szClassName[] = L"Lab3"; // Class  name
	memset(&WndClass, 0, sizeof(WNDCLASS));
	
	// Set window attributes
	WndClass.style = CS_HREDRAW | CS_VREDRAW; // Redraw the entire window when changing the height or width
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = NULL;
	WndClass.cbWndExtra = NULL;
	WndClass.hInstance = hInstance; // A handle to the instance that contains the window procedure for the class
	//hInstance = hInstance;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszClassName = szClassName;
	WndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	// Register window class
	if (!RegisterClass(&WndClass)) {
		MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
		return NULL;
	}

	// Create window
	mainHandle = CreateWindow(szClassName, L"Button test", 
					WS_CAPTION | WS_POPUPWINDOW, 
					CW_USEDEFAULT, CW_USEDEFAULT, 
					400, 800,
					NULL, NULL, hInstance, NULL);

	offsetV = 8, offsetH = 120;
	height = 20, width = 60;

	// create 3 textboxes (btn title, width, height)
	HWND hTitle = CreateWindow(L"Edit", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT, offsetH, offsetV, width, height, mainHandle, (HMENU)TITLE_TEXTBOX_ID, hInstance, 0);
	offsetV += (10 + height);
	HWND hWidth = CreateWindow(L"Edit", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER, offsetH, offsetV, width, height, mainHandle, (HMENU)WIDTH_TEXTBOX_ID, hInstance, 0);
	offsetV += (10 + height);
	HWND hHeight = CreateWindow(L"Edit", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER, offsetH, offsetV, width, height, mainHandle, (HMENU)HEIGHT_TEXTBOX_ID, hInstance, 0);
	offsetV += (10 + height);
	HWND hInfo = CreateWindow(L"Edit", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY, 10, 128, 170, 20, mainHandle, (HMENU)INFO_TEXTBOX_ID, hInstance, 0);

	SetWindowText(hTitle, L"Button");		// def title
	SetWindowText(hWidth, L"100");			// def width
	SetWindowText(hHeight, L"20");			// def height

	offsetV = 98, offsetH = 10;
	height = 20, width = 170;
	// create button to create new btns
	HWND hAddBtn = CreateWindow(L"Button", NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, offsetH, offsetV, width, height, mainHandle, (HMENU)ADD_NEW_BUTTON_ID, hInstance, 0);
	SetWindowText(hAddBtn, L"Add Button");

	ShowWindow(mainHandle, SW_SHOWDEFAULT);
	UpdateWindow(mainHandle);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam) {
	HDC hdc;
	PAINTSTRUCT ps;

	switch (Message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		if (HIWORD(wparam) == EN_UPDATE) {
			handleTextboxes(wparam);
		}

		if (HIWORD(wparam) == BN_CLICKED)
			if (LOWORD(wparam) == ADD_NEW_BUTTON_ID) {
				// Add-new-button is clicked
				if (width == 0 || height == 0 || wcslen(title) == 0) {
					Button *btn = new Button(hInstance, mainHandle);
				}
				else {
					Button *btn = new Button(hInstance, mainHandle, title, width, height);
				}
			}
			else {
				// Added button is clicked
				GetDlgItemText(mainHandle, LOWORD(wparam), buf, MAX_CHARS);
				StringCbPrintf(buf, sizeof(buf), L"%s clicked", buf);
				SetDlgItemText(mainHandle, INFO_TEXTBOX_ID, buf);
			}

			break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		TextOut(hdc, 10, 10, L"button title:", 13);
		TextOut(hdc, 10, 40, L"button width:", 13);
		TextOut(hdc, 10, 70, L"button height:", 14);

		EndPaint(hwnd, &ps);
		break;
	default:
		return DefWindowProc(hwnd, Message, wparam, lparam);
	}

	return 0;
}

void SetClickedButtonTitleToTextbox(int CID, int data) {
	StringCbPrintf(buf, sizeof(buf), L"%d", data);
	SetDlgItemText(mainHandle, CID, buf);
}
void handleTextboxes(WPARAM param) {
	// handle title-textbox
	if (LOWORD(param) == TITLE_TEXTBOX_ID) {
		GetDlgItemText(mainHandle, TITLE_TEXTBOX_ID, title, MAX_CHARS);
	}

	// handle width-textbox 
	if (LOWORD(param) == WIDTH_TEXTBOX_ID) {
		// get text, convert to int
		GetDlgItemText(mainHandle, WIDTH_TEXTBOX_ID, buf, MAX_CHARS);
		tmp = _tstoi(buf);

		if (tmp <= maxWidth) {
			width = tmp;
		}
		else {
			SetClickedButtonTitleToTextbox(WIDTH_TEXTBOX_ID, maxWidth);
		}
	}

	// handle height-textbox
	if (LOWORD(param) == HEIGHT_TEXTBOX_ID) {
		// get text, convert to int
		GetDlgItemText(mainHandle, HEIGHT_TEXTBOX_ID, buf, MAX_CHARS);
		tmp = _tstoi(buf);

		if (tmp <= maxHeight) {
			height = tmp;
		}
		else {
			SetClickedButtonTitleToTextbox(HEIGHT_TEXTBOX_ID, maxHeight);
		}
	}
}