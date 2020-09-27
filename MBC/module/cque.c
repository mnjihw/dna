#include "common.h"

//#define USE_CRITICAL_SECTION

cque_t* cque_alloc(unsigned int size)
{
	cque_t *cque;

	VIRTUALIZER1_START
	{
		cque = (void *)SECURE_API(s_HeapAlloc)((HANDLE)SECURE_API(s_GetProcessHeap)(), 0, (size > sizeof(((cque_t *)0)->data)) ? ((sizeof(cque_t) - sizeof(((cque_t *)0)->data)) + size) : sizeof(cque_t));

#ifdef USE_CRITICAL_SECTION
		/* to avoid exception on low-memory conditions, ORing 0x80000000 is allocate some buffers on initialize. */
		if (InitializeCriticalSectionAndSpinCount(&cque->cs, 0x80000000 | 4096) == 0)
			InitializeCriticalSection(&cque->cs);
#endif

		cque->size[0] = 0;
		cque->size[1] = size;
		cque->pos[0] = 0;
		cque->pos[1] = 0;
	}
	VIRTUALIZER_END

	return cque;
}

void cque_free(cque_t *cque)
{
	VIRTUALIZER1_START
	{
#ifdef USE_CRITICAL_SECTION
		DeleteCriticalSection(&cque->cs);
#endif
	SECURE_API(s_HeapFree)((HANDLE)SECURE_API(s_GetProcessHeap)(), 0, cque);
	}
	VIRTUALIZER_END
}

void cque_clear(cque_t *cque)
{
	VIRTUALIZER1_START
	{
#ifdef USE_CRITICAL_SECTION
		EnterCriticalSection(&cque->cs);
#endif
		cque->size[0] = 0;
		cque->pos[0] = 0;
		cque->pos[1] = 0;
#ifdef USE_CRITICAL_SECTION
		LeaveCriticalSection(&cque->cs);
#endif
	}
	VIRTUALIZER_END
}

unsigned int cque_peek(cque_t *cque, void *data, unsigned int size)
{
	unsigned int a;

#ifdef USE_CRITICAL_SECTION
	EnterCriticalSection(&cque->cs);
#endif
	if (size > cque->size[0])
		size = cque->size[0];
	if (size) {
		if ((a = cque->size[1] - cque->pos[0]) != 0) {
			if (size > a) {
				memcpy(data, &cque->data[cque->pos[0]], a);
				memcpy(&((unsigned char *)data)[a], cque->data, size - a);
			} else
				memcpy(data, &cque->data[cque->pos[0]], size);
		} else
			memcpy(data, cque->data, size);
	}
#ifdef USE_CRITICAL_SECTION
	LeaveCriticalSection(&cque->cs);
#endif
	return size;
}

unsigned int cque_read(cque_t *cque, void *data, unsigned int size)
{
	unsigned int a;

#ifdef USE_CRITICAL_SECTION
	EnterCriticalSection(&cque->cs);
#endif
	if (size > cque->size[0])
		size = cque->size[0];
	if (size) {
		cque->size[0] -= size;
		if ((a = cque->size[1] - cque->pos[0]) != 0) {
			if (size > a) {
				memcpy(data, &cque->data[cque->pos[0]], a);
				memcpy(&((unsigned char *)data)[a], cque->data, size - a);
				cque->pos[0] = size - a;
			} else {
				memcpy(data, &cque->data[cque->pos[0]], size);
				cque->pos[0] += size;
			}
		} else {
			memcpy(data, cque->data, size);
			cque->pos[0] = size;
		}
	}
#ifdef USE_CRITICAL_SECTION
	LeaveCriticalSection(&cque->cs);
#endif
	return size;
}

unsigned int cque_write(cque_t *cque, const void *data, unsigned int size)
{
	unsigned int a;

#ifdef USE_CRITICAL_SECTION
	EnterCriticalSection(&cque->cs);
#endif
	if (cque->size[1] - cque->size[0] >= size) {
		if (size) {
			cque->size[0] += size;
			if ((a = cque->size[1] - cque->pos[1]) != 0) {
				if (size > a) {
					memcpy(&cque->data[cque->pos[1]], data, a);
					memcpy(cque->data, &((unsigned char *)data)[a], size - a);
					cque->pos[1] = size - a;
				} else {
					memcpy(&cque->data[cque->pos[1]], data, size);
					cque->pos[1] += size;
				}
			} else {
				memcpy(cque->data, data, size);
				cque->pos[1] = size;
			}
		}
	} else
		size = 0;
#ifdef USE_CRITICAL_SECTION
	LeaveCriticalSection(&cque->cs);
#endif
	return size;
}

unsigned int cque_recv_prepare(cque_t *cque, WSABUF *wsabuf)
{
	unsigned int result, size, a;

#ifdef USE_CRITICAL_SECTION
	EnterCriticalSection(&cque->cs);
#endif
	if ((size = cque->size[1] - cque->size[0]) != 0) {
		if ((a = cque->size[1] - cque->pos[1]) != 0) {
			if (size > a) {
				wsabuf[0].len = (DWORD)a;
				wsabuf[0].buf = (CHAR *)&cque->data[cque->pos[1]];
				wsabuf[1].len = (DWORD)(size - a);
				wsabuf[1].buf = (CHAR *)cque->data;
				result = 2;
			} else {
				wsabuf[0].len = (DWORD)size;
				wsabuf[0].buf = (CHAR *)&cque->data[cque->pos[1]];
				result = 1;
			}
		} else {
			wsabuf[0].len = (DWORD)size;
			wsabuf[0].buf = (CHAR *)cque->data;
			result = 1;
		}
	} else
		result = 0;
#ifdef USE_CRITICAL_SECTION
	LeaveCriticalSection(&cque->cs);
#endif
	return result;
}

unsigned int cque_recv_ok(cque_t *cque, unsigned int size)
{
	unsigned int a;

#ifdef USE_CRITICAL_SECTION
	EnterCriticalSection(&cque->cs);
#endif
	if (cque->size[1] - cque->size[0] >= size) {
		if (size) {
			cque->size[0] += size;
			if ((a = cque->size[1] - cque->pos[1]) != 0) {
				if (size > a)
					cque->pos[1] = size - a;
				else
					cque->pos[1] += size;
			} else
				cque->pos[1] = size;
		}
		size = cque->size[1] - cque->size[0];
	} else
		size = 0;
#ifdef USE_CRITICAL_SECTION
	LeaveCriticalSection(&cque->cs);
#endif
	return size;
}

unsigned int cque_send_prepare(cque_t *cque, WSABUF *wsabuf)
{
	unsigned int result, size, a;

#ifdef USE_CRITICAL_SECTION
	EnterCriticalSection(&cque->cs);
#endif
	if ((size = cque->size[0]) != 0) {
		if ((a = cque->size[1] - cque->pos[0]) != 0) {
			if (size > a) {
				wsabuf[0].len = (DWORD)a;
				wsabuf[0].buf = (CHAR *)&cque->data[cque->pos[0]];
				wsabuf[1].len = (DWORD)(size - a);
				wsabuf[1].buf = (CHAR *)cque->data;
				result = 2;
			} else {
				wsabuf[0].len = (DWORD)size;
				wsabuf[0].buf = (CHAR *)&cque->data[cque->pos[0]];
				result = 1;
			}
		} else {
			wsabuf[0].len = (DWORD)size;
			wsabuf[0].buf = (CHAR *)cque->data;
			result = 1;
		}
	} else
		result = 0;
#ifdef USE_CRITICAL_SECTION
	LeaveCriticalSection(&cque->cs);
#endif
	return result;
}

unsigned int cque_send_ok(cque_t *cque, unsigned int size)
{
	unsigned int a;

#ifdef USE_CRITICAL_SECTION
	EnterCriticalSection(&cque->cs);
#endif
	if (size > cque->size[0])
		size = cque->size[0];
	if (size) {
		cque->size[0] -= size;
		if ((a = cque->size[1] - cque->pos[0]) != 0) {
			if (size > a)
				cque->pos[0] = size - a;
			else
				cque->pos[0] += size;
		} else
			cque->pos[0] = size;
	}
	size = cque->size[0];
#ifdef USE_CRITICAL_SECTION
	LeaveCriticalSection(&cque->cs);
#endif
	return size;
}