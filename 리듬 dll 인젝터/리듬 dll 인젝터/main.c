#include <Windows.h>
#include <io.h>
#include <ShlObj.h>
#include "resource.h"
#pragma comment(lib, "Comctl32.lib")

VOID MessageLoop();
int (WINAPI *MessageBoxTimeout)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds);
HINSTANCE g_hInst;
HWND g_hWnd, g_hWnd2, g_hEdit1, g_hEdit2;
char exepath[MAX_PATH];
char dllpath[MAX_PATH];
int ismaple;


LRESULT CALLBACK MyEditWndProc1(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	int i, j;

	switch (iMessage)
	{
	case WM_DROPFILES:
		j = DragQueryFile((HDROP) wParam, -1, 0, 0);
		if (j != 1)
		{
			MessageBoxTimeout(g_hWnd, "파일은 하나만 선택 가능합니다.", "알림", MB_ICONINFORMATION, 0, 2000);
			return 0;
		}
		for (i = 0; i < j; ++i)/* 파일 1개로 제한 */
			DragQueryFile((HDROP) wParam, i, exepath, sizeof(exepath));
		DragFinish((HDROP) wParam);
		if (memcmp(&exepath[strlen(exepath) - 3], "exe", 3) != 0)
		{
			memset(exepath, 0, sizeof(exepath));
			MessageBoxTimeout(g_hWnd, "exe만 드래그 앤 드롭 가능합니다.", "알림", MB_ICONINFORMATION, 0, 2000);
			return DefSubclassProc(hWnd, iMessage, wParam, lParam);
		}
		if (memcmp(&exepath[strlen(exepath) - 14], "MapleStory.exe", 14) == 0) //MapleStory.exe (사이즈 14)
			ismaple = 1;
		else
			ismaple = 0;
		SetWindowText(g_hEdit1, exepath);
		break;
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);

}

LRESULT CALLBACK MyEditWndProc2(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	int i, j;

	switch (iMessage)
	{
	case WM_DROPFILES:
		j = DragQueryFile((HDROP) wParam, -1, 0, 0);
		if (j != 1)
		{
			MessageBoxTimeout(g_hWnd, "파일은 하나만 선택 가능합니다.", "알림", MB_ICONINFORMATION, 0, 2000);
			return 0;
		}
		for (i = 0; i < j; ++i)/* 파일 1개로 제한 */
			DragQueryFile((HDROP) wParam, i, dllpath, sizeof(dllpath));
		DragFinish((HDROP) wParam);
		if (memcmp(&dllpath[strlen(dllpath) - 3], "dll", 3) != 0)
		{
			memset(dllpath, 0, sizeof(dllpath));
			MessageBoxTimeout(g_hWnd, "dll만 드래그 앤 드롭 가능합니다.", "알림", MB_ICONINFORMATION, 0, 2000);
			return DefSubclassProc(hWnd, iMessage, wParam, lParam);
		}
		SetWindowText(g_hEdit2, dllpath);
		break;

	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}

void dbg(const char *fmt, ...)
{
	char data[1024];
	wvsprintf(data, fmt, (va_list)((unsigned int)&fmt + sizeof(void*)));
	OutputDebugString(data);
}



VOID WaitUntil(int(*func)(DWORD[]), DWORD param[], DWORD dwMilliseconds)
{
	MSG msg;
	DWORD start;


	start = GetTickCount();

	while (dwMilliseconds ? (GetTickCount() - start < dwMilliseconds) : 1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				PostQuitMessage(msg.wParam);
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (func && func(param))
				break;
			Sleep(10);
		}
	}
}

int func(DWORD param[])
{

	if (WaitForSingleObject((HANDLE)param[0], 0) == WAIT_OBJECT_0)
		return 1;
	else
		return 0;
}


void MySetImeMode(HWND hEdit, BOOL bHan)

{

	HIMC hImc;

	hImc = ImmGetContext(hEdit);



	if (bHan == TRUE) {

		ImmSetConversionStatus(hImc, IME_CMODE_NATIVE, IME_SMODE_NONE);

	}
	else {

		ImmSetConversionStatus(hImc, 0, IME_SMODE_NONE);

	}



	ImmReleaseContext(hEdit, hImc);

}
#pragma comment(lib, "imm32.lib")
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	void *addr;
	HANDLE hThread;
	DWORD written;
	OPENFILENAME ofn;
	char buf[1024] = { 0, };

	switch (iMessage)
	{
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		g_hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		g_hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		SetWindowSubclass(g_hEdit1, MyEditWndProc1, 0, 0); 
		SetWindowSubclass(g_hEdit2, MyEditWndProc2, 1, 0);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1)));
		MessageBoxTimeout = (int (WINAPI *)(HWND, LPCSTR, LPCSTR, UINT, WORD, DWORD))GetProcAddress(GetModuleHandle("user32.dll"), "MessageBoxTimeoutA");
		MySetImeMode(g_hEdit1, 1);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			wsprintf(buf, "test123");
			dbg("ㅇㅇ %p", buf);
			MessageBox(0, buf, 0, 0);
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = exepath;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXTENSIONDIFFERENT;
			ofn.lpstrFilter = "실행 파일 (*.exe)\0*.exe\0";
			ofn.lpstrDefExt = "exe";
			if (GetOpenFileName(&ofn) != 0)
				SetWindowText(g_hEdit1, exepath);
			if (memcmp(&exepath[strlen(exepath) - 14], "MapleStory.exe", 14) == 0) //MapleStory.exe (사이즈 14)
				ismaple = 1;
			else
				ismaple = 0;
			break;
		case IDC_BUTTON2:
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = dllpath;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXTENSIONDIFFERENT;
			ofn.lpstrFilter = "동적 링크 라이브러리 (*.dll)\0*.dll\0";
			ofn.lpstrDefExt = "dll";
			if (GetOpenFileName(&ofn) != 0)
				SetWindowText(g_hEdit2, dllpath);
			break;
		case IDC_BUTTON3:
			if (GetWindowTextLength(g_hEdit1) == 0 || GetWindowTextLength(g_hEdit2) == 0)
			{
				MessageBoxTimeout(g_hWnd, "파일 경로가 비었습니다. 열기 버튼으로 선택하거나\n드래그 앤 드롭으로 에디트에 파일을 끌어놓으세요.", "알림", MB_ICONINFORMATION, 0, 2000);
				break;
			}
			GetWindowText(g_hEdit1, exepath, sizeof(exepath));
			GetWindowText(g_hEdit2, dllpath, sizeof(dllpath));
			if (_access(exepath, 0) != 0)
			{
				MessageBoxTimeout(g_hWnd, "실행 파일의 경로가 잘못됐습니다.", "알림", MB_ICONINFORMATION, 0, 2000);
				break;
			}
			if (_access(dllpath, 0) != 0)
			{
				MessageBoxTimeout(g_hWnd, "dll 파일의 경로가 잘못됐습니다.", "알림", MB_ICONINFORMATION, 0, 2000);
				break;
			}
			if (ismaple)
				wsprintf(buf, "%s %s", exepath, "GameLaunching");
			memset(&si, 0, sizeof(si));
			si.cb = sizeof(STARTUPINFO);
			if (ismaple)
			{
				if (!CreateProcess(NULL, buf, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					wsprintf(buf, "%s() %d", "CreateProcess", GetLastError());
					MessageBox(g_hWnd, buf, 0, 0);
					break;
				}
			}
			else
			{
				if (!CreateProcess(NULL, exepath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					wsprintf(buf, "%s() %d", "CreateProcess", GetLastError());
					MessageBox(g_hWnd, buf, 0, 0);
					return 0;
				}
			}
			if (!(addr = VirtualAllocEx(pi.hProcess, 0, strlen(dllpath) + 1, MEM_COMMIT, PAGE_READWRITE)))
			{
				wsprintf(buf, "%s() %u", "VirtualAllocEx", GetLastError());
				MessageBox(g_hWnd, buf, 0, MB_ICONERROR);
				return 0;
			}


			if (!WriteProcessMemory(pi.hProcess, addr, dllpath, strlen(dllpath), &written))
			{
				wsprintf(buf, "%s() %u", "WriteProcessMemory", GetLastError());
				MessageBox(g_hWnd, buf, 0, MB_ICONERROR);
				return 0;
			}

			Sleep(500);
			if (!(hThread = CreateRemoteThread(pi.hProcess, 0, 0, (LPTHREAD_START_ROUTINE) GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"), addr, 0, 0)))
			{
				wsprintf(buf, "%s() %u", "CreateRemoteThread", GetLastError());
				MessageBox(g_hWnd, buf, 0, MB_ICONERROR);
				return 0;
			}
			WaitUntil(func, (DWORD[]) { (DWORD)hThread }, 0);

			//ResumeThread(pi.hThread);
			CloseHandle(hThread);
			VirtualFreeEx(pi.hProcess, addr, 0, MEM_RELEASE);
			MessageBoxTimeout(g_hWnd, "dll 인젝션 완료!", "알림", MB_ICONINFORMATION, 0, 2000);
			break;
		}
		break;
	case WM_CLOSE:
		RemoveWindowSubclass(g_hEdit1, MyEditWndProc1, 0);
		RemoveWindowSubclass(g_hEdit1, MyEditWndProc2, 1);
		DestroyWindow(hDlg);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

VOID MessageLoop()
{
	MSG Message;

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}
void aa();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

	g_hInst = hInstance;

	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 켜 주세요.", "알림", MB_ICONERROR);
		//return 0;
	}
#if 0
	__asm
	{
		push 72h;
		push 65746C69h;
		push 46656761h;
		push 7373654Dh;
		push 776F646Eh;
		push 69576567h;
		push 6E616843h;//ChangeWindowMessageFilter

		push 6C6Ch;
		push 642E3233h;
		push 72657375h;//user32.dll

		lea eax, dword ptr[esp + 0Ch];
		push eax;
		lea eax, dword ptr[esp + 4];
		push eax;
		call GetModuleHandleA;

		push eax;
		call GetProcAddress;

		mov esi, eax;
		test esi, esi;
		je $end;

		push 1;
		push 233h;
		call esi;

		push 1;
		push 4Ah;
		call esi;

		push 1;
		push 49h;
		call esi;

	$end:;
	}
#endif
	typedef BOOL(WINAPI *ChangeWindowMessageFilter_Type)(UINT message, DWORD dwFlag);

	ChangeWindowMessageFilter_Type fnChangeWindowMessageFilter = (ChangeWindowMessageFilter_Type) GetProcAddress(GetModuleHandle("user32.dll"), "ChangeWindowMessageFilter");

	if (fnChangeWindowMessageFilter)
	{
		fnChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
		fnChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
		fnChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	}
	
	
	CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();
	
	return 0;
}


