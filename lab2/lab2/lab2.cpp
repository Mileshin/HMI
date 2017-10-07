#include <windows.h>
#include <iostream> 
#include <fstream>

// Human readable signals for window action handlers
#define OPEN			1
#define SAVE_AS			2	
#define APPLY_BITBLT	3

// Prototypes for functions below
LPCTSTR getFileName();
int saveBitmap(HBITMAP bm);
LRESULT CALLBACK handleWindowEvents(HWND, UINT, WPARAM, LPARAM);
void doublingBlue(HWND hwnd, HBITMAP hBitmap);
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
	wchar_t szClassName[] = L"Lab2"; // Class  name
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
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,  // The style of the window being created
		CW_USEDEFAULT, CW_USEDEFAULT, // position
		CW_USEDEFAULT, NULL, // width and height
		NULL, // no parent
		hInstance, // A handle to the instance of the module to be associated with the window
		NULL);

	// Create menu
	HMENU MainMenu = CreateMenu();
	AppendMenu(MainMenu, MF_STRING, OPEN, TEXT("Open file"));
	AppendMenu(MainMenu, MF_STRING, SAVE_AS, TEXT("Save as"));
	AppendMenu(MainMenu, MF_STRING, APPLY_BITBLT, TEXT("Change color"));
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
			if (fileName != NULL) {
				hBitmap = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				InvalidateRect(hwnd, NULL, TRUE); // add a rectangle to the updated region of the specified window
				isLoaded = true;
			}
		}
		if (LOWORD(wParam) == SAVE_AS) { // save picture
			if (!isLoaded) {
				break;
			}
			saveBitmap(hBitmap);
			break;
		}
		if (LOWORD(wParam) == APPLY_BITBLT) { // change color 
			if (!isLoaded) {
				break;
			}
			doublingBlue(hwnd, hBitmap);
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


void doublingBlue(HWND hwnd, HBITMAP hBitmap) {
	BITMAP bmp;
	HDC hCompatibleDC = CreateCompatibleDC(NULL); // creates a memory device context (DC) compatible with the specified device
	
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap); /**/
	
	GetObject(hBitmap, sizeof(BITMAP), &bmp);

	UINT cClrBits = (UINT)(bmp.bmPlanes * bmp.bmBitsPixel); // Convert the color format to a count of bits. 
	UINT biSizeImage = ((bmp.bmWidth * cClrBits + 31) & ~31) / 8 * bmp.bmHeight; // border alignment 4 bytes
	
	// create  new bmp
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bmp.bmWidth;
	bmi.bmiHeader.biHeight = bmp.bmHeight;
	bmi.bmiHeader.biPlanes = bmp.bmPlanes;
	bmi.bmiHeader.biBitCount = bmp.bmBitsPixel;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = biSizeImage;
	bmi.bmiHeader.biClrImportant = 0;

	LPBYTE lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, biSizeImage); // memory pointer
	GetDIBits(hCompatibleDC, hBitmap, 0, bmp.bmHeight, lpBits, &bmi, DIB_RGB_COLORS); // hBitmap -> lpBits
	WORD buf;
	for (long i = 0; i < bmp.bmWidth * bmp.bmHeight * 3; i += 3) { 
		buf = lpBits[i];
		buf <<= 1; //doubling of blue color

		if (buf > 255) { // border check
			buf = 255;
		}
		lpBits[i] = buf;
	}

	SetDIBits(hCompatibleDC, hBitmap, 0, bmp.bmHeight, lpBits, &bmi, DIB_RGB_COLORS); // lpBits -> hBitmap  

	SelectObject(hCompatibleDC, hOldBitmap);

	GlobalFree((HGLOBAL)lpBits);
	DeleteDC(hCompatibleDC);
	DeleteObject(hOldBitmap);
	InvalidateRect(hwnd, NULL, TRUE);
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

	if (GetOpenFileName(&ofn)) {
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
	ofn.Flags = OFN_PATHMUSTEXIST;

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














