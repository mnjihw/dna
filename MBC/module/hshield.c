#include "common.h"

unsigned int secure_value_B;
static void *hshield_base;
static PROC hshield_servicecall;

int __stdcall hshield_myservicecall(unsigned int code, void *in, void *out)
{
	int ret;

	// 추측중..
	// code = 4  : AhnHS_Initialize -> ordinal #4
	// code = 5  : AhnHS_StartService
	// code = 6  : AhnHS_StopService
	// code = 7  : AhnHS_Uninitialize
	// code = 13 : MakeAckMsg() -> oridinal #16

	// param[8]까진 보장되는데, 그다음부턴 버전에 따라서 보장안됨
	// [ 0] : hs communication function (__stdcall, args: 3)
	// [ 1] : hs version? (9919:baram, 9924:maple)
	// [ 2] : application code (92A868D874779E2D8F4183C5:baram, F41D67364DCFCB0838D866E2:maple, F13D69B759A234FB9C9D7AFF:maplet)
	// [ 3] : hs operation mode (0xE883DBE,0xE083DBE:baram, 0xA823DBE,0x8A0221AE,0x0A8221AE:maple, 0x8883DBE:qplay)
		//       24       16        8        0zㅔ
		// 00001110 10001000 00111101 10111110 (baram before)
		// 00001110 00001000 00111101 10111110 (baram after)
		// 00001010 10000010 00111101 10111110 (maple 0xA823DBE)
		// 10001010 00000010 00100001 10101110 (maple 0x8A0221AE)
		// 00001010 10000010 00100001 10101110 (maple 0x0A8221AE)
		// 10001000 10001000 00111101 10111110 (qplay)
		// 00000000 00001000 00111101 10111110 (nage)
		// 01000000 00001000 00111101 10111110 (kart)
		// 10011011 10001001 00111101 10111110 (????)
		//                *->GetDC(17)차단
		//               *->좃됨(18)
		//            *-> 0x10901이 옴(21).. 바로 종료 -,.-
		//           *-> 0x10501이 안옴(22)
		//          *-> 중복실행 체크(23)
		//       *-> 문제 발견시 3회 호출후 강제종료(25)
		//      *-> 핵쉴드 TrayIcon(26)
		//     *-> VM 체크(27)
	// [ 4] : FFFF0328 (g_dwSDKver)
	// [ 5] : 00000001
	// [ 6] : HSHield Directory
	// [ 7] : function (GetTickCount) [CDECL]
	// [ 8] : function (timeGetTime) [CDECL]
	// [ 9] : function (QueryPerformanceCounter) [STDCALL]
	// [10] : function (QueryPerformanceFrequency) [STDCALL]
	// [11] : value (must be > 0x5403A032)
	// [12] : 0x5050101
	// [13] : EhSvc Handle

	VIRTUALIZER3_START
	{
		if (code == 4) {		
			HookExportProc(hshield_base, (char *)10, hshield_servicecall);
			((unsigned int *)in)[3] &= ~((1 << 17) | (1 << 23));
		}
		ret = hshield_servicecall(code, in, out);
		//dbg("HS #%u ret=%p out=%p", code, ret, *(unsigned int *)out);
	}
	VIRTUALIZER_END

	return ret;
}

HMODULE WINAPI MyLoadLibraryA(LPCSTR lpLibFileName)
{
	static volatile skip;
	HMODULE hr;
	char data[16];

	if ((hr = (HMODULE)SECURE_API(s_LoadLibraryA)(lpLibFileName)) != NULL && skip == 0 && HIWORD(lpLibFileName) && kstrstr(lpLibFileName, (*(int *)&data[0] = 'vSHE', *(int *)&data[4] = 'ld.c', *(int *)&data[8] = 'l', data)) && _InterlockedExchange(&skip, ~0) == 0) {
		VIRTUALIZER3_START
		{
			hshield_base = hr;
			hshield_servicecall = HookExportProc(hr, (char *)10, hshield_myservicecall);
		}
		VIRTUALIZER_END
	}

	return hr;
}

int Aossdk_SetAuthServerA(int a1)
{
	return 1;
}

int Aossdk_GetMkdS4Object(int a1)
{
	static void *vtable_ref, *vtable[] = {
		"\x33\xC0\xC2\x10\x00", /* StartKeyCrypt@10 */
		"\x33\xC0\xC2\x04\x00", /* StopKeyCrypt@4 */
		"\x33\xC0\xC2\x04\x00", /* StartKeyCrypt@4 */
		"\x33\xC0\xC2\x04\x00", /* StopKeyCrypt@4 */
	};
	
	VIRTUALIZER2_START
	{
		vtable_ref = vtable;
		a1 = (int)&vtable_ref;
	}
	VIRTUALIZER_END

	return a1;
}

int Aossdk_StartAosSDKA(int a1, int a2, int a3, int a4)
{
	return 1;
}

int Aossdk_TerminatePdA(int a1, int a2)
{
	return 1;
}

void hshield_hook(void)
{
	unsigned int i;
	PROC fn;

	VIRTUALIZER3_START
	{
		if ((secure_value_B ^ (init_rand(SECURE_API(s_GetCurrentProcessId)() ^ 0xBADF00Du), gen_rand() ^ gen_rand())) == 0x04171992u) {
			for (fn = SECURE_API(s_LoadLibraryA); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)MyLoadLibraryA);
			for (fn = SECURE_API(s_Aossdk_SetAuthServerA); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)Aossdk_SetAuthServerA);
			for (fn = SECURE_API(s_Aossdk_GetMkdS4Object); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)Aossdk_GetMkdS4Object);
			for (fn = SECURE_API(s_Aossdk_StartAosSDKA); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)Aossdk_StartAosSDKA);
			for (fn = SECURE_API(s_Aossdk_TerminatePdA); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)Aossdk_TerminatePdA);
		}
	}
	VIRTUALIZER_END
}