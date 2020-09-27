#define _CRT_SECURE_NO_WARNINGS
#include "resource.h"
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "Msimg32.lib")

enum { CLUB, HEART, DIAMOND, SPADE };
typedef enum { TOP, ONE_PAIR, TWO_PAIR, TRIPLE, STRAIGHT, BACK_STRAIGHT, MOUNTAIN, FLUSH, FULL_HOUSE, FOUR_CARD, STF, RTF }RANK;

typedef struct node
{
	int number;
	int suit;
	struct node *next;
}node_t;

typedef struct
{
	node_t head;
	int count;
}list_t;

typedef struct
{
	RANK rank;
	int top;
	int suit;
}result_t;

typedef struct
{
	HDC hdc, hMemDC;
	HBITMAP hMemBitmap, hOldBitmap;
	HWND hWnd;
	HBRUSH hBrush, hOldBrush;
}mystruct_t;

void create_list(list_t **pList);
node_t* get_element(list_t *list, int position);
void add_element(list_t *list, node_t element, BOOL sort);
void insert_element(list_t *list, int position, node_t element);
void remove_element(list_t *list, int position);
void delete_list(list_t *list);
void clear_list(list_t *list);
void dbg(const char *fmt, ...);