#include<windows.h>
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

// Prototypes for functions below
LPCTSTR getFileName();
int saveBitmap(HBITMAP bm);
LRESULT CALLBACK handleWindowEvents(HWND, UINT, WPARAM, LPARAM);
void setPixelDisplay(HWND hwnd, HBITMAP hBitmap);
void setBlt(HWND hWnd, HBITMAP hBitmap);
void runTests(HWND hwnd);

bool isLoaded = false;
HBITMAP hBitmap;

int WINAPI WinMain(HINSTANCE hInstance, //дескриптор экземпляра приложения
	HINSTANCE hPrevInstance, // в Win32 не используется
	LPSTR lpszCmdParam, // нужен для запуска окна в режиме командной строки
	int nCmdShow) { // режим отображения окна
	HWND hWnd; // создаём дескриптор будущего окошка
	WNDCLASS WndClass; // создаём экземпляр, для обращения к членам класса WNDCLASS
	MSG Msg; // создём экземпляр структуры MSG для обработки сообщений
	wchar_t szClassName[] = L"Lab1"; // строка с именем класса

	// Set window attributes
	WndClass.style = CS_HREDRAW | CS_VREDRAW; // стиль класса окошка (Перерисовка всего окна при изменении высоты или ширины)
	WndClass.lpfnWndProc = handleWindowEvents; // Указатель на оконную процедуру
	WndClass.cbClsExtra = NULL; // число освобождаемых байтов за структурой класса окна
	WndClass.cbWndExtra = NULL; // число освобождаемых байтов при создании экземпляра окна
	WndClass.hInstance = hInstance; //Дескриптор экземпляра, который содержит оконную процедуру для класса.
	WndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO); // декриптор пиктограммы
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW); // дескриптор курсора
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // дескриптор кисти для закраски фона окна
	WndClass.lpszMenuName = L"Menu"; // указатель на имя меню
	WndClass.lpszClassName = szClassName; // указатель на имя класса

	// Register window class
	if (!RegisterClass(&WndClass)) {
		// в случае отсутствия регистрации класса :
		MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
		return 0; // возвращаем, следовательно, выходим из WinMain
	}

	// Create window
	hWnd = CreateWindow(szClassName, // имя класса
		L"Image", // имя окошка 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,  // режимы отображения окошка
		CW_USEDEFAULT, CW_USEDEFAULT, // позиция окошка по осям х и y
		CW_USEDEFAULT, // ширина окошка
		NULL, // высота окошка (раз дефолт в ширине, то писать не нужно)
		NULL,// дескриптор родительского окна
		hInstance, // дескриптор экземпляра приложения
		NULL);// ничего не передаём из WndProc

	// Create menu
	HMENU MainMenu = CreateMenu();
	AppendMenu(MainMenu, MF_STRING, OPEN, TEXT("Open file"));
	AppendMenu(MainMenu, MF_STRING, SAVE_AS, TEXT("Save as"));
	AppendMenu(MainMenu, MF_STRING, APPLY_BITBLT, TEXT("Change color by BitBlt"));
	AppendMenu(MainMenu, MF_STRING, START_TESTS, TEXT("Run tests"));
	AppendMenu(MainMenu, MF_STRING, APPLY_SETPIXEL, TEXT("GetPixel"));
	SetMenu(hWnd, MainMenu);

	if (!hWnd) {
		// в случае некорректного создания окошка (неверные параметры и тп):
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK);
		return NULL;
	}

	// Show window, start handling his messages
	ShowWindow(hWnd, SW_SHOWDEFAULT); // отображаем окошко
	UpdateWindow(hWnd); // обновляем окошко
	while (GetMessage(&Msg, NULL, NULL, NULL)) { // извлекаем сообщения из очереди, посылаемые фу-циями, ОС
		TranslateMessage(&Msg); // интерпретируем сообщения
		DispatchMessage(&Msg); // передаём сообщения обратно ОС
	}

	return Msg.wParam; // возвращаем код выхода из приложения
}

LRESULT CALLBACK handleWindowEvents(HWND hwnd, // дескриптор окошка
	UINT message, // сообщение, посылаемое ОС
	WPARAM wParam, // параметры
	LPARAM lParam) { // сообщений, для последующего обращения
	switch (message) {
	case WM_PAINT: {
		BITMAP bm; // БИТОВОЕ ИЗОБРАЖЕНИЕ
		PAINTSTRUCT ps; // структура, содержащая информацию о клиентской области (размеры, цвет и тп)
		HDC hDC = BeginPaint(hwnd, &ps); // инициализируем контекст устройства

		HDC hCompatibleDC = CreateCompatibleDC(hDC); // создаём контекст устройства в памяти
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap); // выбираем объект в заданный контекст устройства
		GetObject(hBitmap, sizeof(bm), &bm); // получаем информацию из логического объкта
		BitBlt(hDC, NULL, NULL, bm.bmWidth, bm.bmHeight, hCompatibleDC, NULL, NULL, SRCCOPY); // передаём битовые блоки данных
		SelectObject(hCompatibleDC, hOldBitmap);
		DeleteDC(hCompatibleDC);
		EndPaint(hwnd, &ps);
		break;
	}

	case WM_COMMAND: { // обработка команд
		if (LOWORD(wParam) == OPEN) { //открытие
			LPCTSTR fileName = getFileName(); // Получаем имя файла
			if (fileName != NULL){
				hBitmap = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE | LR_CREATEDIBSECTION); // загрузка картинки
				InvalidateRect(hwnd, NULL, TRUE); //добавляет прямоугольник к обновляемому региону заданного окна
				isLoaded = true;
			}
		}
		if (LOWORD(wParam) == SAVE_AS) { // сохранение
			saveBitmap(hBitmap);
			break;
		}
		if (LOWORD(wParam) == START_TESTS) { // вызов тестирования
			runTests(hwnd);
			break;
		}
		if (LOWORD(wParam) == APPLY_BITBLT) { // смена цветов
			if (!isLoaded) {
				break;
			}
			setBlt(hwnd, hBitmap);
		}
		if (LOWORD(wParam) == APPLY_SETPIXEL) { // setpixel
			if (!isLoaded) {
				break;
			}
			setPixelDisplay(hwnd, hBitmap);
		}
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0); // отправляем WinMain() сообщение WM_QUIT
		break;
	}
	default: {
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	}
	return 0;
}

void runTests(HWND hwnd) {
	HBITMAP cpy;
	time_t start_time;
	time_t stats[20];
	DOUBLE y[10];
	for (int type = 0; type<2; type++) {
		printf(type ? "pixel:  " : "bitblt: ");
		for (int i = 1; i <= 10; i++) {
			cpy = (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, i * 200, i * 200, 0);
			start_time = time(NULL);
			switch (type) {
			case 0:
				setBlt(hwnd, cpy);
				break;
			case 1:
				setPixelDisplay(hwnd, cpy);
				break;
			}
			stats[type * 10 + (i - 1)] = time(NULL) - start_time;


			printf("%02d\t", stats[type * 10 + (i - 1)]);
			DeleteObject(cpy);
		}
		printf("\n");
	}
	for (int i = 0; i < 10; i++){
		y[i] = stats[i] / stats[i + 10] * 100;
	}
}

void setBlt(HWND hwnd, // дескриптор окошка
	HBITMAP hBitmap) { // картинка

	BITMAP bm; // битовое изображение
	HDC hCompatibleDC = CreateCompatibleDC(NULL); // создаём контекст устройства в памяти совместимый с текущим экраном приложения
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap); // выбираем объект в заданный контекст устройства

	GetObject(hBitmap, sizeof(bm), &bm); // извлекаем информацию для заданного графического обьекта

	SelectObject(hCompatibleDC, GetStockObject(DC_BRUSH)); //выбираем кисть с заданным цветом
	SetDCBrushColor(hCompatibleDC, RGB(255, 0, 255)); // устанавливаем цвет кисти убирающий зелённый цвет

	BitBlt(hCompatibleDC, 0, 0, bm.bmWidth, bm.bmHeight, hCompatibleDC, 0, 0, MERGECOPY); // изменение цвета
	SetDCBrushColor(hCompatibleDC, RGB(0, 64, 0)); // устанавливаем цвет кисти убирающий зелённый цвет
	BitBlt(hCompatibleDC, 0, 0, bm.bmWidth, bm.bmHeight, hCompatibleDC, 0, 0, PATINVERT); // изменение цвета
	SelectObject(hCompatibleDC, hOldBitmap);
	DeleteDC(hCompatibleDC);
	InvalidateRect(hwnd, NULL, TRUE);

}

void setPixelDisplay(HWND hwnd, HBITMAP hBitmap) {
	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm); // получаем информацию о изображении

	HDC hCompatibleDC = CreateCompatibleDC(NULL); //создаём контекст устройства в памяти совместимый с текущим экраном приложения
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);  //  выбираем объект в заданный контекст устройства
																		 //BitBlt(hCompatibleDC, 0, 0, bm.bmWidth, bm.bmHeight, hCompatibleDC, 0, 0, SRCCOPY);
	for (int i = 0; i < bm.bmWidth; i++) {
		for (int j = 0; j < bm.bmHeight; j++) {
			SetPixel(hCompatibleDC, i, j,
				GetPixel(hCompatibleDC, i, j) & 0xFF00FF | 0x004000);
		}
	}
	SelectObject(hCompatibleDC, hOldBitmap);
	saveBitmap(hBitmap);
	InvalidateRect(hwnd, NULL, TRUE);
	DeleteDC(hCompatibleDC);
	DeleteObject(hOldBitmap);
}

LPCTSTR getFileName() {
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn)); 
	ofn.lStructSize = sizeof(ofn); // длина структыр в байтах
	ofn.hwndOwner = NULL; // Дескриптор окна, которое владеет блоком диалога. 
	ofn.lpstrFile = new WCHAR[1000]; //
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 1000;  // размер буфера
	ofn.lpstrFilter = L"BMP\0*.bmp\0"; // фильтры
	ofn.nFilterIndex = 1;  // индекс фильтр
	ofn.lpstrFileTitle = NULL; // Указатель на буфер, который принимает имя файла и расширение
	ofn.nMaxFileTitle = 0; // Определяет размер буфера, в  TCHARs, указанного элементом lpstrFileTitle
	ofn.lpstrInitialDir = NULL; // Указатель на строку, которая может определить начальный каталог
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
	GetSaveFileName(&ofn);

	return ofn.lpstrFile;
}




int saveBitmap(HBITMAP H) {
	BITMAP bm; // битовое изображение
	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;
	HDC hdc = CreateCompatibleDC(NULL); // создаём контекст устройства в памяти совместимый с текущим экраном приложения
	GetObject(hBitmap, sizeof(bm), &bm); // извлекаем информацию для заданного графического обьекта
	// BitMapInfoHeader
	bi.biSize = sizeof(BITMAPINFOHEADER); //Длина заголовка
	bi.biWidth = bm.bmWidth; //Ширина изображения
	bi.biHeight = bm.bmHeight;//Высота изображения
	bi.biPlanes = 1;//Число плоскостей
	bi.biBitCount = 32;//Глубина цвета
	bi.biCompression = BI_RGB; //Тип компрессии 
	bi.biSizeImage = 0; //Размер изображения
	bi.biXPelsPerMeter = 0;//Горизонтальное разрешение
	bi.biYPelsPerMeter = 0;//Вертикальное разрешение
	bi.biClrUsed = 0;//Число используемых цветов
	bi.biClrImportant = 0;//Число основных цветов

	DWORD dwBmpSize = ((bm.bmWidth * bi.biBitCount + 31) / 32) * 4 * bm.bmHeight;

	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize); // Выделяем перемещаемый блок памяти
	char *lpbitmap = (char *)GlobalLock(hDIB); // Фиксируем блок памяти, возвращая его указатель

	GetDIBits(hdc, H, 0, // запись в буфер рисунка
		(UINT)bm.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	LPCTSTR filename = SaveAs(); // получение имени

	HANDLE hFile = CreateFile(filename, // создание файла
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// BitMapFileHeader
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER); //Смещение изображения от начала файла
	bmfHeader.bfSize = dwSizeofDIB; // Размер файла
	bmfHeader.bfType = 0x4D42; //Сигнатура "BM" 

	// запись
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




