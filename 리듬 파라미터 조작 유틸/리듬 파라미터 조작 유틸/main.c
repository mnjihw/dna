#include "common.h"

unsigned int g_data_type_sel, g_is_hex;
HWND g_hWnd, g_hList;
DWORD g_pid;
LPVOID g_func;
CREATE_PROCESS_DEBUG_INFO g_cpdi;
BYTE g_original_byte;

struct
{
	HANDLE hEvent;
	unsigned int id;
}mystruct;


void dump(const void *data, unsigned int size)
{
	unsigned int i, j, k, z;
	char buffer[128];

	for (z = 0, i = 0; i < size; i += 16u) {
		k = 0;

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size) {
				for (; j < 16u; ++j) {
					*(unsigned int *)&buffer[k] = '    ';
					k += 3u;
				}
				break;
			}
			buffer[k + 0u] = "0123456789ABCDEF"[((unsigned char *)data)[i + j] >> 4];
			buffer[k + 1u] = "0123456789ABCDEF"[((unsigned char *)data)[i + j] & 15];
			buffer[k + 2u] = ' ';
			k += 3u;
		}

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size)
				break;
			if (z == 0) {
				if (((unsigned char *)data)[i + j] > 31u && ((unsigned char *)data)[i + j] < 127u)
					buffer[k++] = ((unsigned char *)data)[i + j];
				/* FIXME: i + j + 1u가 size보다 큰 경우, 운이 아주 나쁘면 memory access violation 발생 */
				else if (((((unsigned char *)data)[i + j] >= 0xB0u && ((unsigned char *)data)[i + j] <= 0xC8u) && (((unsigned char *)data)[i + j + 1u] >= 0xA1u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu)) ||
					((((unsigned char *)data)[i + j] >= 0x81u && ((unsigned char *)data)[i + j] <= 0xA0u) && ((((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x5Au) || (((unsigned char *)data)[i + j + 1u] >= 0x61u && ((unsigned char *)data)[i + j + 1u] <= 0x7Au) || (((unsigned char *)data)[i + j + 1u] >= 0x81u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu))) ||
					((((unsigned char *)data)[i + j] >= 0xA1u && ((unsigned char *)data)[i + j] <= 0xC5u) && ((((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x5Au) || (((unsigned char *)data)[i + j + 1u] >= 0x61u && ((unsigned char *)data)[i + j + 1u] <= 0x7Au) || (((unsigned char *)data)[i + j + 1u] >= 0x81u && ((unsigned char *)data)[i + j + 1u] <= 0xA0u))) ||
					(((unsigned char *)data)[i + j] == 0xC6u && (((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x52u)) ||
					(((unsigned char *)data)[i + j] == 0xA4u && (((unsigned char *)data)[i + j + 1u] >= 0xA1u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu))) {
					if (j != 15u) {
						*(unsigned short *)&buffer[k] = *(unsigned short *) &((unsigned char *)data)[i + j];
						k += 2u;
						j += 1u;
					}
					else {
						buffer[k++] = '.';
						z = 1;
					}
				}
				else
					buffer[k++] = '.';
			}
			else {
				buffer[k++] = '.';
				z = 0;
			}
		}

		
		OutputDebugStringA((*(unsigned int *)&buffer[k] = 0, buffer));
		
			
	}
}


void dbg(const char *fmt, ...)
{
	char data[1024];
	wvsprintf(data, fmt, (va_list)((unsigned int)&fmt + sizeof(void*)));
	OutputDebugString(data);
}
void OnCreateProcessDebugEvent(LPDEBUG_EVENT pde)
{
	char dll_name[64], func_name[64];

	GetDlgItemText(g_hWnd, IDC_EDIT1, dll_name, sizeof(dll_name));
	GetDlgItemText(g_hWnd, IDC_EDIT2, func_name, sizeof(func_name));

	if (!(g_func = GetProcAddress(GetModuleHandle(dll_name), func_name)))
	{
		DebugActiveProcessStop(g_pid);
		MessageBox(0, "함수 찾기 실패!", 0, 0);
		return;
	}
	memcpy(&g_cpdi, &pde->u.CreateProcessInfo, sizeof(CREATE_PROCESS_DEBUG_INFO));
	ReadProcessMemory(g_cpdi.hProcess, g_func, &g_original_byte, 1, NULL);
	WriteProcessMemory(g_cpdi.hProcess, g_func, "\xCC", 1, NULL);
	FlushInstructionCache(g_cpdi.hProcess, g_func, 1);
}


BOOL OnExceptionDebugEvent(LPDEBUG_EVENT pde)
{
	CONTEXT ctx;
	PEXCEPTION_RECORD per = &pde->u.Exception.ExceptionRecord;
	LPVOID func, remote_string;
	unsigned int i, param_arr[16] = { 0 , }, bytes;
	unsigned int numberof_parameters, ordinal;
	char buf[4096], buf2[256];
	DWORD protect, value, address, read;
	BOOL bSuccess;
	t_disasm da;

	if (per->ExceptionCode == EXCEPTION_BREAKPOINT && per->ExceptionAddress == g_func)
	{
		WriteProcessMemory(g_cpdi.hProcess, g_func, &g_original_byte, 1, NULL);
		FlushInstructionCache(g_cpdi.hProcess, g_func, 1);

		ctx.ContextFlags = CONTEXT_CONTROL;
		GetThreadContext(g_cpdi.hThread, &ctx);
		
		func = g_func;

		i = 0;

		while (ReadProcessMemory(g_cpdi.hProcess, func, buf, sizeof(buf), &read))
		{
			while (1)
			{
				bytes = Disasm(&((BYTE*)func)[i], sizeof(buf), (ulong)&((BYTE*)func)[i], &da, DISASM_FILE);

				if (da.error || (strstr(da.result, "RET") && !strstr(da.result, "RETF")))
				{
					if(((BYTE*)func)[i] == 0xC2)
						numberof_parameters = *(WORD*)&((BYTE*)func)[i + 1] >> 2;
					else if (((BYTE*)func)[i] == 0xC3)
					{
						if ((*(unsigned short*)(*(DWORD*)ctx.Esp)) == 0xC483)
							numberof_parameters = (*(unsigned short*)(*(DWORD*)ctx.Esp + 2)) >> 2;
						else
							numberof_parameters = 0;
					}
					goto $end;
				}
				
				i += bytes;
				
			}
		}
	$end:;

		SendMessage(g_hList, LB_RESETCONTENT, 0, 0);
		for (i = 0; i < numberof_parameters; ++i)
		{
			ReadProcessMemory(g_cpdi.hProcess, (LPCVOID)(ctx.Esp + 4 * (i + 1)), &param_arr[i], 4, NULL);
			wsprintf(buf, "[ESP+%Xh]: %p %u%s arg: value=%08u(0x%08X)", 4 * (i + 1), ctx.Esp + 4 * (i + 1), i + 1, ((i + 1) == 2 ? "nd" : ((i + 1) == 3 ? "rd" : "st")), param_arr[i], param_arr[i]);
			SendMessage(g_hList, LB_ADDSTRING, 0, (LPARAM)buf);

		}
		
		EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON2), TRUE);
		EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON3), TRUE);

		do
		{
			WaitForSingleObject(mystruct.hEvent, INFINITE);
			if (mystruct.id == IDC_BUTTON2)
			{
				if (!GetDlgItemText(g_hWnd, IDC_EDIT4, buf, sizeof(buf)))
					MessageBox(0, "바꿀 인자를 입력해주세요.", "알림", MB_OK);
				else if ((ordinal = GetDlgItemInt(g_hWnd, IDC_EDIT3, &bSuccess, FALSE)), !bSuccess)
					MessageBox(0, "바꿀 인자가 몇 번째인지 입력해주세요.", "알림", MB_OK);
				else
				{
					switch (g_data_type_sel)
					{
					case IDC_RADIO1: //값
						value = strtol(buf, NULL, g_is_hex ? 16 : 10);
						VirtualProtectEx(g_cpdi.hProcess, (LPVOID)(ctx.Esp + 4 * ordinal), 4, PAGE_READWRITE, &protect);
						WriteProcessMemory(g_cpdi.hProcess, (LPVOID)(ctx.Esp + 4 * ordinal), &value, 4, NULL);
						VirtualProtectEx(g_cpdi.hProcess, (LPVOID)(ctx.Esp + 4 * ordinal), 4, protect, &protect);
						break;
					case IDC_RADIO2: //문자열(포인터)
						remote_string = VirtualAllocEx(g_cpdi.hProcess, NULL, strlen(buf) + 1, MEM_COMMIT, PAGE_READWRITE); //마지막인자 모름 수정
						VirtualProtectEx(g_cpdi.hProcess, (LPVOID)(ctx.Esp + 4 * ordinal), strlen(buf) + 1, PAGE_READWRITE, &protect);
						WriteProcessMemory(g_cpdi.hProcess, remote_string, buf, strlen(buf) + 1, 0);
						WriteProcessMemory(g_cpdi.hProcess, (LPVOID)(ctx.Esp + 4 * ordinal), &remote_string, 4, 0);
						VirtualProtectEx(g_cpdi.hProcess, (LPVOID)(ctx.Esp + 4 * ordinal), 4, protect, &protect);
						break;
					case IDC_RADIO3: //더블포인터
						value = strtol(buf, NULL, g_is_hex ? 16 : 10);
						VirtualProtectEx(g_cpdi.hProcess, *(DWORD**)(ctx.Esp + 4 * ordinal), 4, PAGE_READWRITE, &protect);
						WriteProcessMemory(g_cpdi.hProcess, *(DWORD**)(ctx.Esp + 4 * ordinal), &value, 4, 0);
						VirtualProtectEx(g_cpdi.hProcess, *(DWORD**)(ctx.Esp + 4 * ordinal), 4, protect, &protect);
						break;
					}
					wsprintf(buf2, "%u번째 인자를 %s로 바꿨음", ordinal, buf);
					SendMessage(g_hList, LB_ADDSTRING, 0, (LPARAM)buf2);
				}
			}
			else if (mystruct.id == IDC_BUTTON4)
			{
				if (!GetDlgItemText(g_hWnd, IDC_EDIT5, buf, sizeof(buf)))
				{
					MessageBox(0, "포인터 값을 확인할 주소를 입력해주세요.", 0, 0);
					continue;
				}
				
				address = strtol(buf, NULL, 16);
				dbg("주소 %p", address);
				value = 0;
				//VirtualProtectEx(g_cpdi.hProcess, (LPVOID)address, 4, PAGE_READWRITE, &protect);
				switch (g_data_type_sel)
				{
				case IDC_RADIO1: //값
					ReadProcessMemory(g_cpdi.hProcess, (LPCVOID)address, &value, 4, 0);
					wsprintf(buf, "%p-->%u(0x%X)", address, value, value);
					break;
				case IDC_RADIO2: //문자열(포인터)
					ReadProcessMemory(g_cpdi.hProcess, (LPCVOID)address, buf2, 32, 0);
					wsprintf(buf, "%p-->%s", address, buf2);
					break;
				case IDC_RADIO3:
					break;
				}
				
				//VirtualProtectEx(g_cpdi.hProcess, (LPVOID)address, 4, protect, &protect);
				
				SendMessage(g_hList, LB_ADDSTRING, 0, (LPARAM)buf);
			}
		} while (mystruct.id == IDC_BUTTON2 || mystruct.id == IDC_BUTTON4);
	
		ctx.Eip = (DWORD)g_func;
		SetThreadContext(g_cpdi.hThread, &ctx);

		ContinueDebugEvent(pde->dwProcessId, pde->dwThreadId, DBG_CONTINUE);
		SwitchToThread();
		WriteProcessMemory(g_cpdi.hProcess, g_func, "\xCC", 1, NULL);
		FlushInstructionCache(g_cpdi.hProcess, g_func, 1);
		EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON2), FALSE);
		EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON3), FALSE);

		return TRUE;
	}
	return FALSE;
}


void DebugLoop()
{
	DEBUG_EVENT de;

	while (WaitForDebugEvent(&de, INFINITE))
	{
		if (de.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
			OnCreateProcessDebugEvent(&de);
		else if (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
		{
			if (OnExceptionDebugEvent(&de))
				continue;
		}
		else if (de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
			break;
		ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
	}
}

DWORD WINAPI DebugLoopThread(LPVOID arg)
{
	DebugActiveProcess(*(DWORD*)arg);
	DebugSetProcessKillOnExit(FALSE);
	DebugLoop();

	return 0;
}


BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char dll_name[64], func_name[64], buf[128];
	FARPROC func;


	switch (iMessage)
	{
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		g_hList = GetDlgItem(hDlg, IDC_LIST1);
		mystruct.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON2), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON3), FALSE);
		CheckDlgButton(hDlg, IDC_RADIO1, 1);
		g_data_type_sel = IDC_RADIO1;
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"ESP");
		SendDlgItemMessage(hDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)"+");
		SendDlgItemMessage(hDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)"-");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, 0, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO2, CB_SETCURSEL, 0, 0);

		////////////////////////////////
		SetDlgItemText(hDlg, IDC_EDIT1, "user32.dll");
		SetDlgItemText(hDlg, IDC_EDIT2, "MessageBoxA");

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RADIO1:
		case IDC_RADIO2:
		case IDC_RADIO3:
			g_data_type_sel = LOWORD(wParam);
			break;
		case IDC_BUTTON1: //시작
			GetDlgItemText(hDlg, IDC_EDIT1, dll_name, sizeof(dll_name));
			GetDlgItemText(hDlg, IDC_EDIT2, func_name, sizeof(func_name));
			if (!(func = GetProcAddress(GetModuleHandle(dll_name), func_name)))
			{
				wsprintf(buf, "%s!%s() 찾지 못함!", dll_name, func_name);
				MessageBox(0, buf, 0, 0);
				return TRUE;
			}
			GetWindowThreadProcessId(FindWindow(0, "리듬 dll 인젝터"), &g_pid);
			if(!g_pid)
			{
				MessageBox(0, "윈도우 찾지 못함!", 0, 0);
				return TRUE;
			}
			EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON1), FALSE);
			SendMessage(g_hList, LB_RESETCONTENT, 0, 0);
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DebugLoopThread, &g_pid, 0, 0));
			break;
		case IDC_BUTTON2:
		case IDC_BUTTON3:
		case IDC_BUTTON4:
			mystruct.id = LOWORD(wParam);
			SetEvent(mystruct.hEvent);
			break;
		case IDC_BUTTON5:
			dbg("DF");
			dbg("%d", GetLastError());
			break;
		}
		break;
	case WM_CLOSE:
		CloseHandle(mystruct.hEvent);
		if(g_pid)
			DebugActiveProcessStop(g_pid);
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
	MSG msg;

	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsWindow(g_hWnd) || !IsDialogMessage(g_hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();
	return 0;
}