#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "resource.h"
#include <ShlObj.h>
#include <CommCtrl.h>
#include <Windows.h>
#include <stdio.h>
#include <Shlwapi.h>
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Shlwapi.lib")

#define true (0u == 0u)
#define false (0u != 0u)
#define bool unsigned int

typedef struct
{
	unsigned int h;
	unsigned int a;
	unsigned int b;
	unsigned int c;
	unsigned int d;
	unsigned int s;
}stats_t;


typedef struct
{
	unsigned int a : 2; //0: 무보정 1: 증가 2: 감소 3: 미사용
	unsigned int b : 2; 
	unsigned int c : 2; 
	unsigned int d : 2;
	unsigned int s : 2;
}nature_effect_t;


typedef struct
{
	int iSubItem;
	BOOL ascending[2];
}sortinfo_t;

void dbg(const char *fmt, ...);
UINT CalHP(UINT base_stat, UINT indiv_value, UINT effort_value, UINT level);
UINT CalOther(UINT base_stat, UINT indiv_value, UINT effort_value, UINT level, double nature_effect);
bool IsStatUV(const char *str);
bool IsStatZ(const char *str);
void Clipboard(const char *source);
unsigned int _strcpy(void *dst, const void *src);
void SelectLVItem(HWND hwnd, int sel);
