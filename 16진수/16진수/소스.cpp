#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>

void main()
{
	
	
	char str1[1024], str2[1024], str3[1024], *p = NULL;
	int sel, i, j, len;
	unsigned int num;

	SetConsoleTitle("16진수셈ㅋ");
	printf("1. 문자열 -> 16진수\n");
	printf("2. 16진수 -> 문자열\n");
	scanf("%d", &sel);
	
	//fflush(stdin);
	getchar();
	gets_s(str1, sizeof(str1));
	
	if (sel == 1)
	{ 
		for (i = 0; i < strlen(str1); ++i)
			printf("%02X ", str1[i] & 0xFF);
		printf(" %02X", strlen(str1));
	}
	else if (sel == 2)
	{
		for (i = 0, j = 0; i < strlen(str1); ++i, ++j)
		{
			
			if (str1[i] == ' ')
			{
				--j;
				continue;
			}
			str2[j] = str1[i];
		}
		str2[j] = '\0';
		ZeroMemory(str1, sizeof(str1));
		p = str2;
		len = strlen(p);
		for (i = 0; i < len / 2; ++i)
		{
			memcpy(str3, p, 2);
			num = strtol(str3, NULL, 16);
			str1[i] = num;
			*p++;
			*p++;

		}

		str1[i] = '\0';
		printf("%s", str1);
	}
	getchar();
	
}
