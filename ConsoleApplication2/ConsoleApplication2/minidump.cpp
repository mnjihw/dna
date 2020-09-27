#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "dbghelp.lib")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>

static FILE *file_;
static LPTOP_LEVEL_EXCEPTION_FILTER previous_handler_;
static BOOL sym_initialized_;

void minidump_log(const char *fmt, ...) {
	char s[1024 + 1];

	if (_vsnprintf(s, sizeof(s), fmt, (va_list)((char *)&fmt + sizeof(void *))) < 0)
		s[sizeof(s) - 1u] = 0u;

	if (file_)
		fputs(s, file_);

	fputs(s, stdout);
}

BOOL CALLBACK EnumLoadedModules(PCTSTR ModuleName, DWORD64 ModuleBase, ULONG ModuleSize, PVOID UserContext) {
#ifdef _WIN64
	minidump_log("%016I64X %s\n", ModuleBase, ModuleName);
#else
	minidump_log("%08I64X %s\n", ModuleBase, ModuleName);
#endif
	return TRUE;
}

LONG WINAPI HandleException(EXCEPTION_POINTERS *ExceptionInfo) {
	if (ExceptionInfo) {
		char s[1024];
		GetLocalTime((SYSTEMTIME *)s);
		sprintf(&s[GetModuleFileNameA(NULL, s, sizeof(s) - 21u)], ".%04u%02u%02u.%02u%02u%02u.log",
			((SYSTEMTIME *)s)->wYear, ((SYSTEMTIME *)s)->wMonth, ((SYSTEMTIME *)s)->wDay,
			((SYSTEMTIME *)s)->wHour, ((SYSTEMTIME *)s)->wMinute, ((SYSTEMTIME *)s)->wSecond);
		file_ = fopen(s, "w");
		minidump_log("%s\n", s);
		if (1) {
			MINIDUMP_EXCEPTION_INFORMATION info;
			info.ThreadId = GetCurrentThreadId();
			info.ExceptionPointers = ExceptionInfo;
			info.ClientPointers = FALSE;
			strcpy(&s[strlen(s) - 3u], "dmp");
			{
				HANDLE handle = CreateFileA(s, GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
				if (handle != INVALID_HANDLE_VALUE) {
					MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), handle, MiniDumpNormal, &info, NULL, NULL);
					CloseHandle(handle);
				}
			}
		}
		{
			const char *e = NULL;
			switch (ExceptionInfo->ExceptionRecord->ExceptionCode) {
			case EXCEPTION_ACCESS_VIOLATION:			e = "EXCEPTION_ACCESS_VIOLATION"; break;
			case EXCEPTION_DATATYPE_MISALIGNMENT:		e = "EXCEPTION_DATATYPE_MISALIGNMENT"; break;
			case EXCEPTION_BREAKPOINT:					e = "EXCEPTION_BREAKPOINT"; break;
			case EXCEPTION_SINGLE_STEP:					e = "EXCEPTION_SINGLE_STEP"; break;
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:		e = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"; break;
			case EXCEPTION_FLT_DENORMAL_OPERAND:		e = "EXCEPTION_FLT_DENORMAL_OPERAND"; break;
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:			e = "EXCEPTION_FLT_DIVIDE_BY_ZERO"; break;
			case EXCEPTION_FLT_INEXACT_RESULT:			e = "EXCEPTION_FLT_INEXACT_RESULT"; break;
			case EXCEPTION_FLT_INVALID_OPERATION:		e = "EXCEPTION_FLT_INVALID_OPERATION"; break;
			case EXCEPTION_FLT_OVERFLOW:				e = "EXCEPTION_FLT_OVERFLOW"; break;
			case EXCEPTION_FLT_STACK_CHECK:				e = "EXCEPTION_FLT_STACK_CHECK"; break;
			case EXCEPTION_FLT_UNDERFLOW:				e = "EXCEPTION_FLT_UNDERFLOW"; break;
			case EXCEPTION_INT_DIVIDE_BY_ZERO:			e = "EXCEPTION_INT_DIVIDE_BY_ZERO"; break;
			case EXCEPTION_INT_OVERFLOW:				e = "EXCEPTION_INT_OVERFLOW"; break;
			case EXCEPTION_PRIV_INSTRUCTION:			e = "EXCEPTION_PRIV_INSTRUCTION"; break;
			case EXCEPTION_IN_PAGE_ERROR:				e = "EXCEPTION_IN_PAGE_ERROR"; break;
			case EXCEPTION_ILLEGAL_INSTRUCTION:			e = "EXCEPTION_ILLEGAL_INSTRUCTION"; break;
			case EXCEPTION_NONCONTINUABLE_EXCEPTION:	e = "EXCEPTION_NONCONTINUABLE_EXCEPTION"; break;
			case EXCEPTION_STACK_OVERFLOW:				e = "EXCEPTION_STACK_OVERFLOW"; break;
			case EXCEPTION_INVALID_DISPOSITION:			e = "EXCEPTION_INVALID_DISPOSITION"; break;
			case EXCEPTION_GUARD_PAGE:					e = "EXCEPTION_GUARD_PAGE"; break;
			case EXCEPTION_INVALID_HANDLE:				e = "EXCEPTION_INVALID_HANDLE"; break;
			/*case EXCEPTION_POSSIBLE_DEADLOCK:			e = "EXCEPTION_POSSIBLE_DEADLOCK"; break;*/
			case CONTROL_C_EXIT:						e = "CONTROL_C_EXIT"; break;
			case 0xC0000409u:							e = "Stack buffer overflow"; break;
			case 0xE06D7363u:							e = "Microsoft C++ Exception"; break;
			}
			if (e)
				minidump_log("%p %s\n", ExceptionInfo->ExceptionRecord->ExceptionAddress, e);
			else
				minidump_log("%p %08X\n", ExceptionInfo->ExceptionRecord->ExceptionAddress, ExceptionInfo->ExceptionRecord->ExceptionCode);
		}
		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
			if (ExceptionInfo->ExceptionRecord->ExceptionInformation[0] == 0u)
				minidump_log("attempt to read the inaccessible data at %p\n", (void *)ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
			else if (ExceptionInfo->ExceptionRecord->ExceptionInformation[0] == 1u)
				minidump_log("attempt to write the inaccessible data at %p\n", (void *)ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
			else if (ExceptionInfo->ExceptionRecord->ExceptionInformation[0] == 8u)
				minidump_log("data execution prevention (DEP) violation at %p\n", (void *)ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
		if (sym_initialized_) {
			STACKFRAME64 stack;
			ZeroMemory(&stack, sizeof(stack));
			minidump_log("Call Stack\n");
#ifdef _WIN64
			{
				CONTEXT *context = ExceptionInfo->ContextRecord;
				stack.AddrPC.Offset = context->Rip;
				stack.AddrPC.Mode = AddrModeFlat;
				stack.AddrFrame.Offset = context->Rbp;
				stack.AddrFrame.Mode = AddrModeFlat;
				stack.AddrStack.Offset = context->Rsp;
				stack.AddrStack.Mode = AddrModeFlat;
				while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), GetCurrentThread(), &stack, context, NULL, NULL, NULL, NULL)) {
#else /* !defined(_WIN64) */
			{
				CONTEXT *context = ExceptionInfo->ContextRecord;
				stack.AddrPC.Offset = context->Eip;
				stack.AddrPC.Mode = AddrModeFlat;
				stack.AddrFrame.Offset = context->Ebp;
				stack.AddrFrame.Mode = AddrModeFlat;
				stack.AddrStack.Offset = context->Esp;
				stack.AddrStack.Mode = AddrModeFlat;
				while (StackWalk64(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &stack, context, NULL, NULL, NULL, NULL)) {
#endif
					DWORD64 offset;
					((SYMBOL_INFO *)s)->SizeOfStruct = sizeof(SYMBOL_INFO);
					((SYMBOL_INFO *)s)->MaxNameLen = sizeof(s) - sizeof(SYMBOL_INFO);
					if (SymFromAddr(GetCurrentProcess(), stack.AddrPC.Offset, &offset, (SYMBOL_INFO *)s)) {
						DWORD displacement;
						IMAGEHLP_LINE64 line;
						line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
#ifdef _WIN64
						if (SymGetLineFromAddr64(GetCurrentProcess(), stack.AddrPC.Offset, &displacement, &line))
							minidump_log("%016I64X %s+0x%I64X (%s:%u)\n", stack.AddrPC.Offset, ((SYMBOL_INFO *)s)->Name, offset, line.FileName, line.LineNumber);
						else
							minidump_log("%016I64X %s+0x%I64X\n", stack.AddrPC.Offset, ((SYMBOL_INFO *)s)->Name, offset);
					} else
						minidump_log("%016I64X\n", stack.AddrPC.Offset);
#else /* !defined(_WIN64) */
						if (SymGetLineFromAddr64(GetCurrentProcess(), stack.AddrPC.Offset, &displacement, &line))
							minidump_log("%08I64X %s+0x%I64X (%s:%u)\n", stack.AddrPC.Offset, ((SYMBOL_INFO *)s)->Name, offset, line.FileName, line.LineNumber);
						else
							minidump_log("%08I64X %s+0x%I64X\n", stack.AddrPC.Offset, ((SYMBOL_INFO *)s)->Name, offset);
					} else
						minidump_log("%08I64X\n", stack.AddrPC.Offset);
#endif
				}
			}
			minidump_log("Loaded modules\n");
			EnumerateLoadedModules64(GetCurrentProcess(), (PENUMLOADED_MODULES_CALLBACK64)EnumLoadedModules, NULL);
		}
		if (file_) {
			fflush(file_);
			fclose(file_);
		}
	}
	MessageBoxA(NULL, "CRASH", NULL, MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), 0u);
	return EXCEPTION_EXECUTE_HANDLER;
}

void minidump_atexit(void) {
	if (sym_initialized_) {
		SymCleanup(GetCurrentProcess());
		sym_initialized_ = FALSE;
	}
	SetUnhandledExceptionFilter(previous_handler_);
}

void minidump_setup(void) {
	atexit(minidump_atexit);
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
	previous_handler_ = SetUnhandledExceptionFilter(HandleException);
	if (sym_initialized_ = SymInitialize(GetCurrentProcess(), NULL, TRUE))
		SymSetOptions(SymGetOptions() | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_EXACT_SYMBOLS | SYMOPT_NO_PROMPTS /*| SYMOPT_FAVOR_COMPRESSED*/);
}