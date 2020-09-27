#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <locale.h>
#pragma comment(lib, "winmm.lib")

extern void init_rand(unsigned int seed);
extern unsigned int gen_rand();

#if 0
#define TOUPPER(x) (('a' <= (x) && (x) <= 'z') ? ((x - 'a') + 'A') : (x))

char* kstristr(const char *haystack, const char *needle) {
	int i, j, k[64];

	if (!*needle)
		return (char *)haystack;

	/*k = (int *)_alloca(sizeof(int) * (strsize(needle) + 1u));*/
	*k = -1;
	j = -1;
	i = 0;

	do {
		if (i == 63)
			__asm int 3;
		while (j > -1 && TOUPPER(needle[i]) != TOUPPER(needle[j]))
			j = k[j];
		++i, ++j;
		k[i] = j;
	} while (needle[i]);

	for (i = 0, j = 0; haystack[i];) {
		while (j >= 0 && TOUPPER(haystack[i]) != TOUPPER(needle[j]))
			j = k[j];
		++i, ++j;
		if (!needle[j])
			return (char *)haystack + i - j;
	}

	return NULL;
}
#endif

unsigned int breakloop;

DWORD WINAPI SoundThread(LPVOID *arg)
{
	unsigned int i;

	while (!breakloop)
	{
		PlaySound((LPCSTR)arg, GetModuleHandle(0), SND_FILENAME | SND_SYNC);
		for (i = 0; i < 10 && !breakloop; ++i)
			Sleep(100);
	}
	breakloop = 0;
	return 0;
}
static __declspec(naked) unsigned int __fastcall byteswap_ulong(unsigned int value)
{
	__asm
	{
		mov eax, ecx
		bswap eax
		retn
	}
}


void trimstring(char *str)
{
	unsigned int i;

	for (i = 0; i < strlen(str); ++i)
	{
		if (str[i] == '\'' || str[i] == '(' || str[i] == ')')
			memcpy(&str[i], &str[i + 1], strlen(str) - i), --i;
	}
}

typedef struct _node
{
	char nick[25];
	char comment[201];
	struct node *next;
}node;

typedef struct _list
{
	node *head;
	node *tail;
	node *cur;
	int count;
}List;

void NodeInit(List *plist)
{
	plist->head = NULL;
	plist->tail = NULL;
	plist->cur = NULL;
	plist->count = 0;
}

void NodeInsert(List *plist, char *data1, char *data2)
{
	node *newnode = (node*)malloc(sizeof(node));
	strcpy(newnode->nick, data1);
	strcpy(newnode->comment, data2);
	newnode->next = NULL;
	
	if (plist->head == NULL)
		plist->head = newnode;
	else
		plist->tail->next = newnode;
	plist->tail = newnode;
	++plist->count;
}

void NodeDeleteAll(List *plist)
{
	node *tempnode;
	node *next;

	tempnode = plist->head;

	while (tempnode)
	{
		next = tempnode->next;
		free(tempnode);
		tempnode = next;
	}
	plist->count = 0;
	plist->head = NULL;
}

const wchar_t CHOSUNG_LIST[] = 
{
	'ㄱ', 'ㄲ', 'ㄴ', 'ㄷ', 'ㄸ', 'ㄹ', 'ㅁ', 'ㅂ', 'ㅃ',
	'ㅅ', 'ㅆ', 'ㅇ' , 'ㅈ', 'ㅉ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ'
};


const wchar_t JUNGSUNG_LIST[] = 
{
	'ㅏ', 'ㅐ', 'ㅑ', 'ㅒ', 'ㅓ', 'ㅔ',
	'ㅕ', 'ㅖ', 'ㅗ', 'ㅘ', 'ㅙ', 'ㅚ',
	'ㅛ', 'ㅜ', 'ㅝ', 'ㅞ', 'ㅟ', 'ㅠ',
	'ㅡ', 'ㅢ', 'ㅣ'
};


const wchar_t JONGSUNG_LIST[] = 
{
	' ', 'ㄱ', 'ㄲ', 'ㄳ', 'ㄴ', 'ㄵ', 'ㄶ', 'ㄷ',
	'ㄹ', 'ㄺ', 'ㄻ', 'ㄼ', 'ㄽ', 'ㄾ', 'ㄿ', 'ㅀ',
	'ㅁ', 'ㅂ', 'ㅄ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅊ',
	'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ'
};

typedef struct
{
	int f;
	int m;
	int l;
}hangul_t;

void hangul_disassemble(const wchar_t wc, hangul_t *param)
{
	if (wc >= 0xAC00 && wc <= 0xD7A3)
	{
		int wc2 = (int)wc - 0xAC00;
		param->l = wc2 % 28;
		wc2 /= 28;
		param->m = wc2 % 21;
		param->f = wc2 / 21;
	}
	else
	{
		param->f = -1;
		param->m = -1;
		param->l = -1;
	}
}

wchar_t hangul_assemble(hangul_t param)
{
	return 0xAC00 + ((param.f * 21) + param.m) * 28 + param.l;
}


void main()
{
	Sleep(1500);
	HWND hwnd = GetForegroundWindow();
	while (1)
	{
		PostMessage(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(2612, 1301));
		printf("보내는중");
		Sleep(500);
	}
	return;
	
	unsigned char name[650][13];
	char buf[128];
	char input[128];
	unsigned int i = 0, sel;
	FILE *fp;

	SetConsoleTitle("리듬 포켓몬 울음소리 맞히기");
	init_rand(GetCurrentProcessId() ^ 0xBADF00Du);

	fp = fopen("포켓몬 목록.txt", "rt");
	if (fp == NULL)
	{
		printf("ㅄ");
		return;
	}
	
	while (!feof(fp))
	{
		if (fgets(name[i], sizeof(name[0]), fp) == 0)
			break;
		if (name[i][strlen(name[i]) - 1] == '\n')
			name[i][strlen(name[i]) - 1] = '\0';
		++i;
	}
	fclose(fp);

$lable:;
	printf("0 : 모든 세대\n1. 1세대\n2. 2세대\n3. 3세대\n4. 4세대\n5. 5세대\n6. 원하는 포켓몬 울음소리 듣기\n입력 : ");
	scanf("%u", &sel);
	
	while (1)
	{
		if(sel == 0)
			i = gen_rand() % 649 + 1;
		else if (sel == 1)
			i = gen_rand() % 151 + 1;
		else if (sel == 2)
			i = gen_rand() % 100 + 152;
		else if (sel == 3)
			i = gen_rand() % 135 + 252;
		else if (sel == 4)
			i = gen_rand() % 107 + 387;
		else if (sel == 5)
			i = gen_rand() % 156 + 494;
		else if (sel == 6)
		{
			printf("이름 입력 : ");
			scanf_s("%s", input, sizeof(input));
			for (i = 0; i < _countof(name); ++i)
			{
				if (_stricmp(input, name[i]) == 0)
				{
					wsprintf(buf, "울음소리\\%03u.wav", i + 1);
					PlaySound(buf, GetModuleHandle(0), SND_FILENAME | SND_ASYNC);
					printf("\n");
					goto $lable;
				}
			}
					
		}
		else
		{
			printf("제대로 입력해주세요.\n");
			goto $lable;
		}
		wsprintf(buf, "울음소리\\%03u.wav", i);
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SoundThread, buf, 0, 0));

		
		printf("이 포켓몬의 이름은? : ");
		scanf_s("%s", input, sizeof(input));
		breakloop = 1; 
		
		if (_stricmp(input, name[i - 1]) == 0)
			printf("정답!\n");
		else
			printf("틀림! 답은 \'%s\'\n", name[i - 1]);
	}
	
}