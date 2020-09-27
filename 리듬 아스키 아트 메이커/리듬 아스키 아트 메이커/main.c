#include "common.h"


VOID MessageLoop()
{
	MSG Message;

	while (GetMessage(&Message, 0, 0, 0) > 0)
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}

void get_ascii_art(const char *filename, char *dst)
{
	char ascii[] = { '#', '#', '@', '@', '%', '%', '=', '=', '+', '+', '*', ':', '-', '.', ' ' };
	HBITMAP hBitmap;
	BITMAP bmp;
	BITMAPINFOHEADER bmih;
	unsigned int idx = 0;
	int size, width, height, padding, totalsize;
	char *output, *image;
	RGBTRIPLE *pixel;
	HDC hdc;
	
	hBitmap = LoadImage(GetModuleHandle(NULL), "test.bmp", IMAGE_BITMAP, 80, 80, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBitmap, sizeof(BITMAP), &bmp);

	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = bmp.bmWidth;
	bmih.biHeight = bmp.bmHeight;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;

	hdc = GetDC(NULL);
	GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, NULL, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);

	if (bmih.biBitCount != 24)
	{
		MessageBox(0, "24비트 아님", 0, 0);
		return;
	}


	width = bmih.biWidth;
	height = bmih.biHeight;
	padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;
	size = bmih.biSizeImage;

	if (size == 0)
		size = (width * PIXEL_SIZE + padding) * height;


	image = malloc(size);
	GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, image, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);



	totalsize = (width * 2 + 2) * height + 1;
	output = malloc(totalsize);

	for (int y = height - 1; y >= 0; --y)
	{
		for (int x = 0; x < width; ++x)
		{

			pixel = (RGBTRIPLE*)&image[(x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y)];


			char c = ascii[(pixel->rgbtRed + pixel->rgbtGreen + pixel->rgbtBlue) / PIXEL_SIZE * sizeof(ascii) / 256];
			output[idx++] = c;
			output[idx++] = c;

		}
		*(unsigned short*)&output[idx] = '\xD\xA', idx += 2;
	}
	output[idx] = '\0';
	Clipboard(output);
	free(output);
	ReleaseDC(NULL, hdc);
	MessageBox(0, "복사됐음", 0, 0);
	free(image);
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char buf[10240];

	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1: 
			get_ascii_art("test.bmp", buf);
			break;
		}
		break;
	case WM_INITDIALOG:
		
		break;
	case WM_DROPFILES:
		dbg("ㅎㅇ");
		
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return 0;
}




int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR);
		return 0;
	}

	typedef BOOL(WINAPI *ChangeWindowMessageFilter_Type)(UINT message, DWORD dwFlag);

	ChangeWindowMessageFilter_Type fnChangeWindowMessageFilter = (ChangeWindowMessageFilter_Type)GetProcAddress(GetModuleHandle("user32.dll"), "ChangeWindowMessageFilter");

	if (fnChangeWindowMessageFilter)
	{
		fnChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
		fnChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
		fnChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	}
	
	CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	MessageLoop();
	return 0;
}
