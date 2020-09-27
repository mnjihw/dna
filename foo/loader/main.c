#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <io.h>
#include <ShlObj.h>


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	HKEY hkey = 0; 
	HWND hMP;
	DWORD cbData = 255, dwType;
	char MaplePath[256], buf[256], buf2[256];
	int count, i;
	void *addr = 0;
	HANDLE hThread = 0;
	WIN32_FIND_DATA fd;
	HANDLE hFind;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 켜주세요.", "알림", MB_ICONERROR);
		return 0;
	}

	GetSystemDirectory(buf, sizeof(buf));
	wsprintf(buf2, "%s\\packet.dll", buf);

	if (_access(buf2, 0) == -1)
	{
		MessageBox(0, "packet.dll을 찾지 못했습니다. WinPcap을 설치해주세요", "알림", MB_ICONERROR);
		return 0;
	}


	hMP = FindWindow("MapleStoryClass", NULL);
	if (hMP)
		PostMessage(hMP, WM_CLOSE, 0, 0);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wizet\\MapleStory", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS, NULL, &hkey, NULL);

	if (RegQueryValueEx(hkey, "ExecPath", NULL, &dwType, (LPBYTE) MaplePath, &cbData) != ERROR_SUCCESS)
	{
		RegCloseKey(hkey);
		wsprintf(buf, "%s() %u%s", "RegQueryValueEx", GetLastError(), "\n관리자 권한으로 켜보시거나 포맷 직후라면\n메이플을 직접 한번이라도 켠 후에 다시 켜주세요.");
		MessageBox(0, buf, 0, MB_ICONERROR);
		return 0;
	}
	RegCloseKey(hkey);


	SetCurrentDirectory(MaplePath);

	wsprintf(buf, "%s\\*.avi", MaplePath);

	if ((hFind = FindFirstFile(buf, &fd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			wsprintf(buf, "%s\\%s", MaplePath,  fd.cFileName);
			DeleteFile(buf);
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	

	strcpy(buf, MaplePath);
	strcat(buf, "\\HShield\\hshield.log");
	DeleteFile(buf);
	strcpy(buf, MaplePath);
	strcat(buf, "\\HSield\\supdate.log");
	DeleteFile(buf);
	strcpy(buf, MaplePath);
	strcat(buf, "\\HSield\\HSUpChk.log");
	DeleteFile(buf);
	strcat(MaplePath, "\\MapleStory.exe GameLaunching");


	if (!CreateProcess(NULL, MaplePath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
	{
		wsprintf(buf, "%s() %d", "CreateProcess", GetLastError());
		MessageBox(0, buf, 0, 0);
		return 0;
	}
	

	GetModuleFileName(NULL, buf, sizeof(buf));

	count = strlen(buf);
	for (i = count - 1; i > 0; --i)
		if (buf[i] == '\\')
			break;

	buf[i] = '\0';
	SetCurrentDirectory(buf);

	if (_access("foo.dll", 0) == 0)
		strcpy(&buf[i], "\\foo.dll");
	else
	{
		MessageBox(0, "DLL 인젝션 실패", 0, MB_ICONERROR);
		return 0;
	}

	count = strlen(buf) + 1;

	__try
	{
		addr = VirtualAllocEx(pi.hProcess, 0, count, MEM_COMMIT, PAGE_READWRITE);
		WriteProcessMemory(pi.hProcess, addr, buf, count, 0);
		if ((hThread = CreateRemoteThread(pi.hProcess, 0, 0, (LPTHREAD_START_ROUTINE) GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"), addr, 0, 0)))
			WaitForSingleObject(hThread, INFINITE);
	}
	__finally
	{
		if (addr)
			VirtualFreeEx(pi.hProcess, addr, 0, MEM_RELEASE);
		if (hThread)
			CloseHandle(hThread);
		CloseHandle(pi.hProcess);
		ResumeThread(pi.hThread);
	}

	

	return 0;
}