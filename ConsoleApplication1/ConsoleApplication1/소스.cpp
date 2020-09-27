#include <stdio.h>
#include <Windows.h>

void main()
{
	int i;
	HWND hMsgbox,hWnd,hEdit;
	getchar();
	hWnd = GetForegroundWindow();
	do
	hWnd = GetNextWindow(hWnd,GW_HWNDNEXT);
	while(!IsWindowVisible(hWnd));
	hEdit = GetDlgItem(hWnd,1);
	if(hEdit == NULL)
		return;
	printf("작동중...");
	for(int i=0;;++i)
	{
		SendMessage(hEdit,WM_CHAR,i+49,0);
		Sleep(50);
		SendMessage(hEdit,WM_KEYDOWN,VK_RETURN,0);
		Sleep(50);
		hMsgbox = FindWindow(NULL,"대화쪽지");
		if(hMsgbox)
			SendMessage(hMsgbox,WM_CLOSE,0,0);
		if(i==126)
			i=0;

	}



}
