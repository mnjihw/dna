#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "imm32.lib")



void main()
{
	HANDLE token, duplicatedToken;

	if (!LogonUser("user1", ".", "zxc021", LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &token))
	{
		printf("Logon failed!\n");
		return;
	}
	if (!DuplicateToken(token, SecurityImpersonation, &duplicatedToken))
	{
		printf("Duplication failed!\n");
		CloseHandle(token);
		return;
	}
	



	
	CloseHandle(token);
}