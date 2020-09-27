#include "common.h"

PROC _secure_api_ptr[s_max_apis];

int secure_setup(void)
{
	int err = ~0;
	HMODULE module;
	char data[64];

	/*SECURE_API_SET(s_OutputDebugStringA, OutputDebugStringA);
	SECURE_API_SET(s_wvsprintfA, wvsprintfA);
	EnumModuleHash();
	MessageBox(0,"setup",0,0);*/

	VIRTUALIZER2_START
	{
		/* WINMM */
		if ((module = GetModule(0xE1FC70A3u)) == NULL ||
			SECURE_API_SET(s_PlaySoundA, GetExportAddress(module, (((int *)data)[0] = 'yalP', ((int *)data)[1] = 'nuoS', ((int *)data)[2] = 'Ad', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_timeBeginPeriod, GetExportAddress(module, (((int *)data)[0] = 'emit', ((int *)data)[1] = 'igeB', ((int *)data)[2] = 'rePn', ((int *)data)[3] = 'doi', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_timeGetDevcaps, GetExportAddress(module, (((int *)data)[0] = 'emit', ((int *)data)[1] = 'DteG', ((int *)data)[2] = 'aCve', ((int *)data)[3] = 'sp', data))) == (PROC)SECURE_API_XORKEY)
			goto $err;

		/* WS2_32 */
		if ((module = GetModule(0x0A484681u)) == NULL ||
			SECURE_API_SET(s_closesocket, GetExportAddress(module, (char *)3)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_connect, GetExportAddress(module, (char *)4)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_ioctlsocket, GetExportAddress(module, (char *)10)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_inet_addr, GetExportAddress(module, (char *)11)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_recv, GetExportAddress(module, (char *)16)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_select, GetExportAddress(module, (char *)18)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_send, GetExportAddress(module, (char *)19)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_setsockopt, GetExportAddress(module, (char *)21)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_shutdown, GetExportAddress(module, (char *)22)) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_WSAGetOverlappedResult, GetExportAddress(module, (((int *)data)[0] = 'GASW', ((int *)data)[1]='vOte', ((int *)data)[2] = 'alre', ((int *)data)[3] = 'depp', ((int *)data)[4] = 'useR', ((int *)data)[5] = 'tl', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_WSARecv, GetExportAddress(module, (((int *)data)[0] = 'RASW', ((int *)data)[1]='vce', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_WSASend, GetExportAddress(module, (((int *)data)[0] = 'SASW', ((int *)data)[1]='dne', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_WSAAsyncSelect, GetExportAddress(module, (((int *)data)[0] = 'AASW', ((int *)data)[1]='cnys', ((int *)data)[2] = 'eleS', ((int *)data)[3] = 'tc', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_WSAGetLastError, GetExportAddress(module, (((int *)data)[0] = 'GASW', ((int *)data)[1]='aLte', ((int *)data)[2] = 'rEts', ((int *)data)[3] = 'ror', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_WSASetLastError, GetExportAddress(module, (((int *)data)[0] = 'SASW', ((int *)data)[1]='aLte', ((int *)data)[2] = 'rEts', ((int *)data)[3] = 'ror', data))) == (PROC)SECURE_API_XORKEY)
			goto $err;

		/* KERNEL32 */
		if ((module = GetModule(0x6A4ABC5Bu)) == NULL ||
			SECURE_API_SET(s_GetProcAddress, GetExportAddress(module, (((int *)data)[0] = 'PteG', ((int *)data)[1] = 'Acor', ((int *)data)[2] = 'erdd', ((int *)data)[3] = 'ss', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_CloseHandle, GetExportAddress(module, (((int *)data)[0] = 'solC', ((int *)data)[1] = 'naHe', ((int *)data)[2] = 'eld', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_CreateFileA, GetExportAddress(module, (((int *)data)[0] = 'aerC', ((int *)data)[1] = 'iFet', ((int *)data)[2] = 'Ael', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_CreateMutexA, GetExportAddress(module, (((int *)data)[0] = 'aerC', ((int *)data)[1] = 'uMet', ((int *)data)[2] = 'Axet', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_DeviceIoControl, GetExportAddress(module, (((int *)data)[0] = 'iveD', ((int *)data)[1] = 'oIec', ((int *)data)[2] = 'tnoC', ((int *)data)[3] = 'lor', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_DisableThreadLibraryCalls, GetExportAddress(module, (((int *)data)[0] = 'asiD', ((int *)data)[1] = 'Telb', ((int *)data)[2] = 'aerh', ((int *)data)[3] = 'biLd', ((int *)data)[4] = 'yrar', ((int *)data)[5] = 'llaC', ((int *)data)[6] = 's', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_ExitProcess, GetExportAddress(module, (((int *)data)[0] = 'tixE', ((int *)data)[1] = 'corP', ((int *)data)[2] = 'sse', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_FlushInstructionCache, GetExportAddress(module, (((int *)data)[0] = 'sulF', ((int *)data)[1] = 'snIh', ((int *)data)[2] = 'curt', ((int *)data)[3] = 'noit', ((int *)data)[4] = 'hcaC', ((int *)data)[5] = 'e', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetCurrentProcess, GetExportAddress(module, (((int *)data)[0] = 'CteG', ((int *)data)[1] = 'erru', ((int *)data)[2] = 'rPtn', ((int *)data)[3] = 'seco', ((int *)data)[4] = 's', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetCurrentProcessId, GetExportAddress(module, (((int *)data)[0] = 'CteG', ((int *)data)[1] = 'erru', ((int *)data)[2] = 'rPtn', ((int *)data)[3] = 'seco', ((int *)data)[4] = 'dIs', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetCurrentThread, GetExportAddress(module, (((int *)data)[0] = 'CteG', ((int *)data)[1] = 'erru', ((int *)data)[2] = 'hTtn', ((int *)data)[3] = 'daer', ((int *)data)[4] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetLastError, GetExportAddress(module, (((int *)data)[0] = 'LteG', ((int *)data)[1] = 'Etsa', ((int *)data)[2] = 'rorr', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetLocalTime, GetExportAddress(module, (((int *)data)[0] = 'LteG', ((int *)data)[1] = 'laco', ((int *)data)[2] = 'emiT', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetModuleFileNameA, GetExportAddress(module, (((int *)data)[0] = 'MteG', ((int *)data)[1] = 'ludo', ((int *)data)[2] = 'liFe', ((int *)data)[3] = 'maNe', ((int *)data)[4] = 'Ae', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetNativeSystemInfo, GetExportAddress(module, (((int *)data)[0] = 'NteG', ((int *)data)[1] = 'vita', ((int *)data)[2] = 'sySe', ((int *)data)[3] = 'Imet', ((int *)data)[4] = 'ofn', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetProcessHeap, GetExportAddress(module, (((int *)data)[0] = 'PteG', ((int *)data)[1] = 'ecor', ((int *)data)[2] = 'eHss', ((int *)data)[3] = 'pa', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetSystemFirmwareTable, GetExportAddress(module, (((int *)data)[0] = 'SteG', ((int *)data)[1] = 'etsy', ((int *)data)[2] = 'riFm', ((int *)data)[3] = 'rawm', ((int *)data)[4] = 'baTe', ((int *)data)[5] = 'el', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetSystemInfo, GetExportAddress(module, (((int *)data)[0] = 'SteG', ((int *)data)[1] = 'etsy', ((int *)data)[2] = 'fnIm', ((int *)data)[3] = 'o', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetTickCount, GetExportAddress(module, (((int *)data)[0] = 'TteG', ((int *)data)[1] = 'Ckci', ((int *)data)[2] = 'tnuo', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetVolumeInformationA, GetExportAddress(module, (((int *)data)[0] = 'VteG', ((int *)data)[1] = 'mulo', ((int *)data)[2] = 'fnIe', ((int *)data)[3] = 'amro', ((int *)data)[4] = 'noit', ((int *)data)[5] = 'A', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GlobalMemoryStatusEx, GetExportAddress(module, (((int *)data)[0] = 'bolG', ((int *)data)[1] = 'eMla', ((int *)data)[2] = 'yrom', ((int *)data)[3] = 'tatS', ((int *)data)[4] = 'xEsu', ((int *)data)[5] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_HeapAlloc, GetExportAddress(module, (((int *)data)[0] = 'paeH', ((int *)data)[1] = 'ollA', ((int *)data)[2] = 'c', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_HeapCreate, GetExportAddress(module, (((int *)data)[0] = 'paeH', ((int *)data)[1] = 'aerC', ((int *)data)[2] = 'et', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_HeapDestroy, GetExportAddress(module, (((int *)data)[0] = 'paeH', ((int *)data)[1] = 'tseD', ((int *)data)[2] = 'yor', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_HeapFree, GetExportAddress(module, (((int *)data)[0] = 'paeH', ((int *)data)[1] = 'eerF', ((int *)data)[2] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_HeapReAlloc, GetExportAddress(module, (((int *)data)[0] = 'paeH', ((int *)data)[1] = 'lAeR', ((int *)data)[2] = 'col', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_LoadLibraryA, GetExportAddress(module, (((int *)data)[0] = 'daoL', ((int *)data)[1] = 'rbiL', ((int *)data)[2] = 'Ayra', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_OutputDebugStringA, GetExportAddress(module, (((int *)data)[0] = 'ptuO', ((int *)data)[1] = 'eDtu', ((int *)data)[2] = 'Sgub', ((int *)data)[3] = 'nirt', ((int *)data)[4] = 'Ag', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetCurrentDirectoryA, GetExportAddress(module, (((int *)data)[0] = 'CteS', ((int *)data)[1] = 'erru', ((int *)data)[2] = 'iDtn', ((int *)data)[3] = 'tcer', ((int *)data)[4] = 'Ayro', ((int *)data)[5] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetLastError, GetExportAddress(module, (((int *)data)[0] = 'LteS', ((int *)data)[1] = 'Etsa', ((int *)data)[2] = 'rorr', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetPriorityClass, GetExportAddress(module, (((int *)data)[0] = 'PteS', ((int *)data)[1] = 'roir', ((int *)data)[2] = 'Cyti', ((int *)data)[3] = 'ssal', ((int *)data)[4] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetProcessWorkingSetSize, GetExportAddress(module, (((int *)data)[0] = 'PteS', ((int *)data)[1] = 'ecor', ((int *)data)[2] = 'oWss', ((int *)data)[3] = 'nikr', ((int *)data)[4] = 'teSg', ((int *)data)[5] = 'eziS', ((int *)data)[6] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetThreadPriority, GetExportAddress(module, (((int *)data)[0] = 'TteS', ((int *)data)[1] = 'aerh', ((int *)data)[2] = 'irPd', ((int *)data)[3] = 'tiro', ((int *)data)[4] = 'y', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetUnhandledExceptionFilter, GetExportAddress(module, (((int *)data)[0] = 'UteS', ((int *)data)[1] = 'nahn', ((int *)data)[2] = 'deld', ((int *)data)[3] = 'ecxE', ((int *)data)[4] = 'oitp', ((int *)data)[5] = 'liFn', ((int *)data)[6] = 'ret', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_Sleep, GetExportAddress(module, (((int *)data)[0] = 'eelS', ((int *)data)[1] = 'p', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SleepEx, GetExportAddress(module, (((int *)data)[0] = 'eelS', ((int *)data)[1] = 'xEp', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_TerminateThread, GetExportAddress(module, (((int *)data)[0] = 'mreT', ((int *)data)[1] = 'tani', ((int *)data)[2] = 'rhTe', ((int *)data)[3] = 'dae', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_TerminateProcess, GetExportAddress(module, (((int *)data)[0] = 'mreT', ((int *)data)[1] = 'tani', ((int *)data)[2] = 'orPe', ((int *)data)[3] = 'ssec', ((int *)data)[4] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_VirtualProtect, GetExportAddress(module, (((int *)data)[0] = 'triV', ((int *)data)[1] = 'Plau', ((int *)data)[2] = 'etor', ((int *)data)[3] = 'tc', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_VirtualQuery, GetExportAddress(module, (((int *)data)[0] = 'triV', ((int *)data)[1] = 'Qlau', ((int *)data)[2] = 'yreu', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetPrivateProfileIntA, GetExportAddress(module, (((int *)data)[0] = 'PteG', ((int *)data)[1] = 'avir', ((int *)data)[2] = 'rPet', ((int *)data)[3] = 'lifo', ((int *)data)[4] = 'tnIe', ((int *)data)[5] = 'A', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_WritePrivateProfileStringA, GetExportAddress(module, (((int *)data)[0] = 'tirW', ((int *)data)[1] = 'irPe', ((int *)data)[2] = 'etav', ((int *)data)[3] = 'forP', ((int *)data)[4] = 'Seli', ((int *)data)[5] = 'nirt', ((int *)data)[6] = 'Ag', data))) == (PROC)SECURE_API_XORKEY)
			goto $err;

		/* USER32 */
		if ((module = GetModule(0x63C84283u)) == NULL ||
			SECURE_API_SET(s_ClientToScreen, GetExportAddress(module, (((int *)data)[0] = 'eilC', ((int *)data)[1] = 'oTtn', ((int *)data)[2] = 'ercS', ((int *)data)[3] = 'ne', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_CreateDialogParamW, GetExportAddress(module, (((int *)data)[0] = 'aerC', ((int *)data)[1] = 'iDet', ((int *)data)[2] = 'gola', ((int *)data)[3] = 'araP', ((int *)data)[4] = 'Wm', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_DestroyWindow, GetExportAddress(module, (((int *)data)[0] = 'tseD', ((int *)data)[1] = 'Wyor', ((int *)data)[2] = 'odni', ((int *)data)[3] = 'w', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetCursorPos, GetExportAddress(module, (((int *)data)[0] = 'CteG', ((int *)data)[1] = 'osru', ((int *)data)[2] = 'soPr', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetDlgItemInt, GetExportAddress(module, (((int *)data)[0] = 'DteG', ((int *)data)[1] = 'tIgl', ((int *)data)[2] = 'nIme', ((int *)data)[3] = 't', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetDlgItemTextA, GetExportAddress(module, (((int *)data)[0] = 'DteG', ((int *)data)[1] = 'tIgl', ((int *)data)[2] = 'eTme', ((int *)data)[3] = 'Atx', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetKeyState, GetExportAddress(module, (((int *)data)[0] = 'KteG', ((int *)data)[1] = 'tSye', ((int *)data)[2] = 'eta', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_GetSystemMenu, GetExportAddress(module, (((int *)data)[0] = 'SteG', ((int *)data)[1] = 'etsy', ((int *)data)[2] = 'neMm', ((int *)data)[3] = 'u', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_InsertMenuW, GetExportAddress(module, (((int *)data)[0] = 'esnI', ((int *)data)[1] = 'eMtr', ((int *)data)[2] = 'Wun', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_KillTimer, GetExportAddress(module, (((int *)data)[0] = 'lliK', ((int *)data)[1] = 'emiT', ((int *)data)[2] = 'r', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_MapVirtualKeyA, GetExportAddress(module, (((int *)data)[0] = 'VpaM', ((int *)data)[1] = 'utri', ((int *)data)[2] = 'eKla', ((int *)data)[3] = 'Ay', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_MsgWaitForMultipleObjects, GetExportAddress(module, (((int *)data)[0] = 'WgsM', ((int *)data)[1] = 'Ftia', ((int *)data)[2] = 'uMro', ((int *)data)[3] = 'pitl', ((int *)data)[4] = 'bOel', ((int *)data)[5] = 'tcej', ((int *)data)[6] = 's', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_MsgWaitForMultipleObjectsEx, GetExportAddress(module, (((int *)data)[0] = 'WgsM', ((int *)data)[1] = 'Ftia', ((int *)data)[2] = 'uMro', ((int *)data)[3] = 'pitl', ((int *)data)[4] = 'bOel', ((int *)data)[5] = 'tcej', ((int *)data)[6] = 'xEs', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_PeekMessageA, GetExportAddress(module, (((int *)data)[0] = 'keeP', ((int *)data)[1] = 'sseM', ((int *)data)[2] = 'Wega', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_PostMessageW, GetExportAddress(module, (((int *)data)[0] = 'tsoP', ((int *)data)[1] = 'sseM', ((int *)data)[2] = 'Wega', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_RegisterClassExA, GetExportAddress(module, (((int *)data)[0] = 'igeR', ((int *)data)[1] = 'rets', ((int *)data)[2] = 'salC', ((int *)data)[3] = 'AxEs', ((int *)data)[4] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SendDlgItemMessageW, GetExportAddress(module, (((int *)data)[0] = 'dneS', ((int *)data)[1] = 'IglD', ((int *)data)[2] = 'Mmet', ((int *)data)[3] = 'asse', ((int *)data)[4] = 'Weg', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetDlgItemInt, GetExportAddress(module, (((int *)data)[0] = 'DteS', ((int *)data)[1] = 'tIgl', ((int *)data)[2] = 'nIme', ((int *)data)[3] = 't', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetForegroundWindow, GetExportAddress(module, (((int *)data)[0] = 'FteS', ((int *)data)[1] = 'gero', ((int *)data)[2] = 'nuor', ((int *)data)[3] = 'niWd', ((int *)data)[4] = 'wod', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetTimer, GetExportAddress(module, (((int *)data)[0] = 'TteS', ((int *)data)[1] = 'remi', ((int *)data)[2] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetWindowTextW, GetExportAddress(module, (((int *)data)[0] = 'WteS', ((int *)data)[1] = 'odni', ((int *)data)[2] = 'xeTw', ((int *)data)[3] = 'Wt', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_SetWindowsHookExA, GetExportAddress(module, (((int *)data)[0] = 'WteS', ((int *)data)[1] = 'odni', ((int *)data)[2] = 'oHsw', ((int *)data)[3] = 'xEko', ((int *)data)[4] = 'A', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_ShowWindow, GetExportAddress(module, (((int *)data)[0] = 'wohS', ((int *)data)[1] = 'dniW', ((int *)data)[2] = 'wo', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_UpdateWindow, GetExportAddress(module, (((int *)data)[0] = 'adpU', ((int *)data)[1] = 'iWet', ((int *)data)[2] = 'wodn', ((int *)data)[3] = 0, data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_wvsprintfA, GetExportAddress(module, (((int *)data)[0] = 'psvw', ((int *)data)[1] = 'tnir', ((int *)data)[2] = 'Af', data))) == (PROC)SECURE_API_XORKEY)
			goto $err;

		/* PSAPI */
		if ((module = GetModule(0x01B82506Du)) == NULL ||
			SECURE_API_SET(s_GetProcessMemoryInfo, GetExportAddress(module, (((int *)data)[0] = 'PteG', ((int *)data)[1] = 'ecor', ((int *)data)[2] = 'eMss', ((int *)data)[3] = 'yrom', ((int *)data)[4] = 'ofnI', ((int *)data)[5] = 0, data))) == (PROC)SECURE_API_XORKEY)
			goto $err;

		/* AOSSDK */
		if ((module = GetModule(0x246AE265u)) == NULL ||
			SECURE_API_SET(s_Aossdk_SetAuthServerA, GetExportAddress(module, (((int *)data)[0] = 'ssoA', ((int *)data)[1] = 'S_kd', ((int *)data)[2] = 'uAte', ((int *)data)[3] = 'eSht', ((int *)data)[4] = 'revr', ((int *)data)[5] = 'A', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_Aossdk_GetMkdS4Object, GetExportAddress(module, (((int *)data)[0] = 'ssoA', ((int *)data)[1] = 'G_kd', ((int *)data)[2] = 'kMte', ((int *)data)[3] = 'O4Sd', ((int *)data)[4] = 'cejb', ((int *)data)[5] = 't', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_Aossdk_StartAosSDKA, GetExportAddress(module, (((int *)data)[0] = 'ssoA', ((int *)data)[1] = 'S_kd', ((int *)data)[2] = 'trat', ((int *)data)[3] = 'SsoA', ((int *)data)[4] = 'AKD', data))) == (PROC)SECURE_API_XORKEY ||
			SECURE_API_SET(s_Aossdk_TerminatePdA, GetExportAddress(module, (((int *)data)[0] = 'ssoA', ((int *)data)[1] = 'T_kd', ((int *)data)[2] = 'imre', ((int *)data)[3] = 'etan', ((int *)data)[4] = 'AdP', data))) == (PROC)SECURE_API_XORKEY)
			goto $err;

		/* OK */
		err = 0;

$err:;
	}
	VIRTUALIZER_END

	return err;
}