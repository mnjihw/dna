#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>





LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
LPSTR lpszClass="리듬 프로그램";
HWND hMP = FindWindow(NULL,"MapleStory");


typedef struct {
	unsigned int xs, ys;
	unsigned int data[1];
} image_t;

HMODULE gdi_module;
ULONG_PTR gdi_token;


int print2clip(char *source)  
{
   int ok = OpenClipboard(NULL);
   if (!ok) return 0;

 

 HGLOBAL clipbuffer;
 char * buffer;


 EmptyClipboard();
 clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(source)+1);
 buffer = (char*)GlobalLock(clipbuffer);
 strcpy(buffer, source);
 GlobalUnlock(clipbuffer);
 SetClipboardData(CF_TEXT,clipbuffer);
 CloseClipboard();

 return 1;
} 



int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,
		  100,100,1000,600,
		  NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);
	
	while(GetMessage(&Message,0,0,0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC     hdc;
	PAINTSTRUCT   ps;
	static HDC   hMemDC;
	static HBITMAP  hBitmap;

	switch(iMessage) 
	{
	case WM_CREATE:
		hdc=GetDC(hWnd);
		hMemDC = CreateCompatibleDC(hdc); 
		hBitmap = (HBITMAP)LoadImage((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),"박봄.bmp", IMAGE_BITMAP,1280, 1024,LR_LOADFROMFILE);
		SelectObject(hMemDC, hBitmap);
		CreateWindow("button","메이플 서버핑",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,20,20,100,25,hWnd,(HMENU)0,g_hInst,NULL);
		CreateWindow("button","일일리상",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,150,20,100,25,hWnd,(HMENU)1,g_hInst,NULL);
		CreateWindow("button","고고리상",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,280,20,100,25,hWnd,(HMENU)2,g_hInst,NULL);
		CreateWindow("button","리듬 롤 선픽",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,410,20,100,25,hWnd,(HMENU)3,g_hInst,NULL);
		CreateWindow("button","▶MBC◀",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,540,20,100,25,hWnd,(HMENU)4,g_hInst,NULL);
		CreateWindow("button","아이템매니아",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,670,20,100,25,hWnd,(HMENU)5,g_hInst,NULL);


		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		
	


		case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case 0:
			system("ping kornet.net -t");
			break;


		case 3:
			ShellExecute(NULL,"open","C:\\Users\\지환\\Desktop\\리듬 프로그램\\소스\\리듬 롤 선픽\\리듬 롤 선픽 원본.ahk",NULL,NULL,SW_SHOWNORMAL);
			break;
		case 4:
			ShellExecute(NULL,"open","http://cafe.naver.com/mabeco",NULL,NULL,SW_SHOWNORMAL);
			break;
		case 5:
			ShellExecute(NULL,"open","http://www.itemmania.com",NULL,NULL,SW_SHOWNORMAL);
			break;
		}
		break;
	


		case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, -300, -400, 1280, 1024, hMemDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;





	case WM_CHAR:
		TCHAR input;
		static char a[50];
		HDC hdc;
		hdc=GetDC(hWnd);
		input=(TCHAR)wParam;
		wsprintf(a,TEXT("입력한 문자:%c"),input);
		TextOut(hdc,50,50,a,strlen(a));
		break;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}