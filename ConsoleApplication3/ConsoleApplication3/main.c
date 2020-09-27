#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "imm32.lib")



void main()
{
	HWND hWnd = FindWindow("Chrome_WidgetWin_1", NULL);
	//HWND hWnd = FindWindow("MapleStoryClass", NULL);
	
	if (hWnd)
	{
		SetForegroundWindow(hWnd);
		Sleep(300);
		
		//hWnd = ImmGetDefaultIMEWnd(hWnd);
		//PostMessage(hWnd, WM_IME_CHAR, '¸®', 0);

		INPUT input[2];
		ZeroMemory(input, sizeof(input));
		input[0].type = INPUT_KEYBOARD;
		input[1].type = INPUT_KEYBOARD;

		char a[] = "WKWMD";
		for (int i = 0; i < strlen(a); ++i)
		{
			input[0].ki.dwFlags = 0;
			input[1].ki.dwFlags = 0;
			input[0].ki.wVk = VK_SHIFT;
			input[1].ki.wVk = a[i];
			SendInput(2, input, sizeof(INPUT));
			input[0].ki.dwFlags = 2;
			input[1].ki.dwFlags = 2;
			SendInput(2, input, sizeof(INPUT));
			
		}
		
		

		

	}
	else
		printf("DD");

	

}