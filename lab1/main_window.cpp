#include <windows.h>
#include <time.h>
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

int WINAPI WinMain(HINSTANCE hInstance, // A handle to the current instance of the application.
	HINSTANCE hPrevInstance, 
	LPSTR lpszCmdParam, 
	int nCmdShow) { // Controls how the window is to be shown.
	HWND hWnd; 
	WNDCLASS WndClass; 
	MSG Msg; 
	wchar_t szClassName[] = L"Lab1"; // Class  name
	hinst = hInstance;

	// Set window attributes
	WndClass.style = CS_HREDRAW | CS_VREDRAW; // Redraw the entire window when changing the height or width
	WndClass.lpfnWndProc = handleWindowEvents; // A pointer to the window procedure
	WndClass.cbClsExtra = NULL; 
	WndClass.cbWndExtra = NULL; 
	WndClass.hInstance = hInstance; // A handle to the instance that contains the window procedure for the class
	WndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO); 
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	WndClass.lpszMenuName = L"Menu"; 
	WndClass.lpszClassName = szClassName; 

	// Register window class
	if (!RegisterClass(&WndClass)) {
		MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
		return NULL; 
	}

	// Create window
	hWnd = CreateWindow(szClassName, // Class name
		L"Image", // window name
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL, CW_USEDEFAULT,  // The style of the window being created
		CW_USEDEFAULT, CW_USEDEFAULT, // position
		CW_USEDEFAULT, NULL, // width and height
		NULL, // no parent
		hInstance, // A handle to the instance of the module to be associated with the window
		NULL);

	// Create menu
	HMENU MainMenu = CreateMenu();
	AppendMenu(MainMenu, MF_STRING, OPEN, TEXT("Open file"));
	AppendMenu(MainMenu, MF_STRING, SAVE_AS, TEXT("Save as"));
	AppendMenu(MainMenu, MF_STRING, APPLY_BITBLT, TEXT("Change color by BitBlt"));
	AppendMenu(MainMenu, MF_STRING, START_TESTS, TEXT("Run tests"));
	AppendMenu(MainMenu, MF_STRING, APPLY_SETPIXEL, TEXT("GetPixel"));
	SetMenu(hWnd, MainMenu);

	if (!hWnd) {
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK);
		return NULL;
	}

	// Show window, start handling his messages
	ShowWindow(hWnd, SW_SHOWDEFAULT); 
	UpdateWindow(hWnd); 
	while (GetMessage(&Msg, NULL, NULL, NULL)) { 
		TranslateMessage(&Msg);
		DispatchMessage(&Msg); 
	}

	return Msg.wParam; 
}

LRESULT CALLBACK handleWindowEvents(HWND hwnd, // Handle window
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam) { 
	switch (message) {
	case WM_PAINT: {
		BITMAP bm; 
		PAINTSTRUCT ps; //  structure contains information for an application
		HDC hDC = BeginPaint(hwnd, &ps); // initialize the device context

		HDC hCompatibleDC = CreateCompatibleDC(hDC); // create a device context in memory
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap); 
		GetObject(hBitmap, sizeof(bm), &bm); 
		BitBlt(hDC, NULL, NULL, bm.bmWidth, bm.bmHeight, hCompatibleDC, NULL, NULL, SRCCOPY); // we transfer bit blocks of data
		SelectObject(hCompatibleDC, hOldBitmap);
		DeleteDC(hCompatibleDC);
		EndPaint(hwnd, &ps);
		break;
	}

	case WM_COMMAND: { // command processing
		if (LOWORD(wParam) == OPEN) { // open and load picture
			LPCTSTR fileName = getFileName(); 
			if (fileName != NULL){
				hBitmap = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE | LR_CREATEDIBSECTION); 
				InvalidateRect(hwnd, NULL, TRUE); // add a rectangle to the updated region of the specified window
				isLoaded = true;
			}
		}
		if (LOWORD(wParam) == SAVE_AS) { // save picture
			saveBitmap(hBitmap);
			break;
		}
		if (LOWORD(wParam) == START_TESTS) { // start testing
			if (!isLoaded) {
				break;
			}
			runTests(hwnd); // testing
			newWindow(hwnd); // drawing
			break;
		}
		if (LOWORD(wParam) == APPLY_BITBLT) { // change color (bitBlt)
			if (!isLoaded) {
				break;
			}
			setBlt(hwnd, hBitmap);
		}
		if (LOWORD(wParam) == APPLY_SETPIXEL) { // change color (setpixel)
			if (!isLoaded) {
				break;
			}
			setPixelDisplay(hwnd, hBitmap);
		}
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0); //  WM_QUIT
		break;
	}
	default: {
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	}
	return 0;
}

void runTests(HWND hwnd) { //testing
	HBITMAP cpy;
	UINT32  start_time;
	for (int type = 0; type<2; type++) {
		for (int i = 1; i <= 10; i++) {
			cpy = (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, i * 200, i * 200, 0);
			start_time = GetTickCount();
			switch (type) {
			case 0:
				setBlt(hwnd, cpy);
				break;
			case 1:
				setPixelDisplay(hwnd, cpy);
				break;
			}
			stats[type * 10 + (i - 1)] = GetTickCount() - start_time;
			max = max < stats[type * 10 + (i - 1)] ? stats[type * 10 + (i - 1)] : max;
			DeleteObject(cpy);
		}
	}
}

// BITBLT
void setBlt(HWND hwnd, 
	HBITMAP hBitmap) { 

	BITMAP bm; 
	HDC hCompatibleDC = CreateCompatibleDC(NULL); // creates a memory device context (DC) compatible with the specified device
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);

	GetObject(hBitmap, sizeof(bm), &bm); 

	SelectObject(hCompatibleDC, GetStockObject(DC_BRUSH)); 

	SetDCBrushColor(hCompatibleDC, RGB(255, 0, 255));										// delete green
	BitBlt(hCompatibleDC, 0, 0, bm.bmWidth, bm.bmHeight, hCompatibleDC, 0, 0, MERGECOPY);	// brush

	SetDCBrushColor(hCompatibleDC, RGB(0, 64, 0));											// set green
	BitBlt(hCompatibleDC, 0, 0, bm.bmWidth, bm.bmHeight, hCompatibleDC, 0, 0, PATINVERT);	// 64
	
	InvalidateRect(hwnd, NULL, TRUE);
	SelectObject(hCompatibleDC, hOldBitmap);
	DeleteDC(hCompatibleDC);
}

// SETPIXEL
void setPixelDisplay(HWND hwnd, HBITMAP hBitmap) {
	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm); 

	HDC hCompatibleDC = CreateCompatibleDC(NULL); 
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);  

	for (int i = 0; i < bm.bmWidth; i++) {
		for (int j = 0; j < bm.bmHeight; j++) {
			SetPixel(hCompatibleDC, i, j,
				GetPixel(hCompatibleDC, i, j) & 0xFF00FF | 0x004000); // change color
		}
	}

	SelectObject(hCompatibleDC, hOldBitmap);
	InvalidateRect(hwnd, NULL, TRUE);
	DeleteDC(hCompatibleDC);
	DeleteObject(hOldBitmap);
}


LPCTSTR getFileName() {
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn)); 
	ofn.lStructSize = sizeof(ofn); 
	ofn.hwndOwner = NULL; 
	ofn.lpstrFile = new WCHAR[1000]; 
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 1000;  
	ofn.lpstrFilter = L"BMP\0*.bmp\0"; 
	ofn.nFilterIndex = 1;  
	ofn.lpstrFileTitle = NULL; 
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL; 
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn)){
		return ofn.lpstrFile;
	}
	else { return NULL; }
}

LPCTSTR SaveAs() {
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = new WCHAR[1000];
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 1000;
	ofn.lpstrFilter = L"BMP\0*.bmp\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST ;

	if (GetSaveFileName(&ofn)) {
		return ofn.lpstrFile;
	}
	else { return NULL; }
}




int saveBitmap(HBITMAP H) {
	BITMAP bm; 
	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;
	HDC hdc = CreateCompatibleDC(NULL); 
	GetObject(hBitmap, sizeof(bm), &bm); 

	// BitMapInfoHeader
	bi.biSize = sizeof(BITMAPINFOHEADER); 
	bi.biWidth = bm.bmWidth; 
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB; 
	bi.biSizeImage = 0; 
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bm.bmWidth * bi.biBitCount + 31) / 32) * 4 * bm.bmHeight;

	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize); // Allocates the specified number of bytes from the heap
	char *lpbitmap = (char *)GlobalLock(hDIB); // Locks a global memory object

	GetDIBits(hdc, H, 0, 
		(UINT)bm.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	LPCTSTR filename = SaveAs(); // get file name

	HANDLE hFile = CreateFile(filename, // create file
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// BitMapFileHeader
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER); // Image offset from the beginning of the file
	bmfHeader.bfSize = dwSizeofDIB; // size file
	bmfHeader.bfType = 0x4D42; // "BM" 

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFile);

	return 0;
}

void DrawGraph(HDC hdc, RECT rectClient,
	int numrow = 1)
{
	double OffsetY, OffsetX;
	int height, width;
	HPEN hpen;
	height = rectClient.bottom - rectClient.top; // height window
	width = rectClient.right - rectClient.left; // width window

	OffsetX = 50;
	OffsetY = GraphHeight - OffsetX - 50;

	hpen = CreatePen(PS_SOLID, 0, 0); // black pen 1px
	SelectObject(hdc, hpen);

	MoveToEx(hdc, 0, OffsetY, 0);		// y-axis
	LineTo(hdc, GraphWidth, OffsetY);
	MoveToEx(hdc, OffsetX, 0, 0);		// x-axis
	LineTo(hdc, OffsetX, GraphHeight);

	// vertical dashes
	for (int i = 1; i <= 10; i++) {
		MoveToEx(hdc, OffsetX + 100 * i, OffsetY - 5, 0);
		LineTo(hdc, OffsetX + 100 * i, OffsetY + 5);
	}

	// number x
	{
		SetTextColor(hdc, 0xFFFFFF00);
		TextOut(hdc, OffsetX - 10, GraphHeight - 40 - OffsetX, L"0", 1);
		TextOut(hdc, (GraphWidth / 10 + OffsetX - 15), GraphHeight - 40 - OffsetX, L"200", 3);
		TextOut(hdc, (GraphWidth / 10 * 2 + OffsetX - 20), GraphHeight - 40 - OffsetX, L"400", 3);
		TextOut(hdc, (GraphWidth / 10 * 3 + OffsetX - 25), GraphHeight - 40 - OffsetX, L"600", 3);
		TextOut(hdc, (GraphWidth / 10 * 4 + OffsetX - 30), GraphHeight - 40 - OffsetX, L"800", 3);
		TextOut(hdc, (GraphWidth / 10 * 5 + OffsetX - 40), GraphHeight - 40 - OffsetX, L"1000", 4);
		TextOut(hdc, (GraphWidth / 10 * 6 + OffsetX - 45), GraphHeight - 40 - OffsetX, L"1200", 4);
		TextOut(hdc, (GraphWidth / 10 * 7 + OffsetX - 50), GraphHeight - 40 - OffsetX, L"1400", 4);
		TextOut(hdc, (GraphWidth / 10 * 8 + OffsetX - 55), GraphHeight - 40 - OffsetX, L"1600", 4);
		TextOut(hdc, (GraphWidth / 10 * 9 + OffsetX - 60), GraphHeight - 40 - OffsetX, L"1800", 4);
		TextOut(hdc, (GraphWidth + OffsetX - 65), GraphHeight - 40 - OffsetX, L"2000", 4);
	}

	max = max*1.1;

	DeleteObject(hpen); // delete black pen
	hpen = CreatePen(PS_SOLID, 0, 0xFF); // red pen
	SelectObject(hdc, hpen);

	//BITBLT graph
	for (int i = 0; i < 9; i++) {
		int y = OffsetY - OffsetY* stats[i] / max;
		MoveToEx(hdc, OffsetX + 100 * i, y, 0);
		y = OffsetY - OffsetY* stats[i + 1] / max;
		LineTo(hdc, OffsetX + 100 * (i + 1), y);
	}

	DeleteObject(hpen); // delete red pen
	hpen = CreatePen(PS_SOLID, 0, RGB(0, 255, 0)); // green pen
	SelectObject(hdc, hpen);

	//setpixel graph
	for (int i = 0; i < 9; i++) {
		double y = OffsetY - OffsetY* stats[i + 10] / max;
		MoveToEx(hdc, OffsetX + 100 * i, (int)y, 0);
		y = OffsetY - OffsetY* stats[i + 11] / max;
		LineTo(hdc, OffsetX + 100 * (i + 1), (int)y);
		wchar_t m_reportFileName[256];
		swprintf_s(m_reportFileName, L"%d", stats[i + 11]);
		TextOut(hdc, 10, y - 10, m_reportFileName, 4); // number y
	}
}

LRESULT CALLBACK ChildProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (Message)
	{
	case WM_PAINT:
		hdc = BeginPaint(child, &ps);
		DrawGraph(hdc, ps.rcPaint); // draw graph
		EndPaint(child, &ps);
		break;
	case WM_DESTROY:
		return 0;
		break;
	default:
		return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}


int newWindow(HWND hwnd) {
	WNDCLASS w;
	memset(&w, 0, sizeof(WNDCLASS));
	w.lpfnWndProc = ChildProc;
	w.hInstance = hinst;
	w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	w.lpszClassName = L"ChildWClass";
	w.hCursor = LoadCursor(NULL, IDC_CROSS);
	RegisterClass(&w);
	child = CreateWindowEx(0, L"ChildWClass", (LPCTSTR)NULL,
		WS_CHILD | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, GraphWidth, GraphHeight, hwnd, NULL, hinst, NULL);
	ShowWindow(child, SW_NORMAL);
	UpdateWindow(child);
	return 0;
}








