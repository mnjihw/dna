#include "common.h"

#define BLOCK_ALIGNMENT1 16u
#define BLOCK_ALIGNMENT2 64u
#define BLOCK_DATA_COUNT1 128u
#define BLOCK_DATA_COUNT2 1003u /* 608u */
#define BLOCK_DATA_SIZE1 (BLOCK_ALIGNMENT1 * BLOCK_DATA_COUNT1)
#define BLOCK_DATA_SIZE2 (BLOCK_ALIGNMENT2 * BLOCK_DATA_COUNT2)
#define BLOCK_DATA_SIZE (BLOCK_DATA_SIZE1 + BLOCK_DATA_SIZE2)

typedef struct memblock_s {
	struct memblock_s *next, *prev;
	unsigned int hash, size, count, unfill, used, maxused;
	char data[BLOCK_DATA_SIZE];
} memblock_t;

typedef struct meminfo_s {
	memblock_t *block;
	unsigned int size;
	unsigned int magic;
} meminfo_t;

typedef struct memblock_large_s {
	struct memblock_large_s *next, *prev;
	meminfo_t info;
} memblock_large_t;

static HANDLE heap;
static memblock_large_t *large_block_list;
static memblock_t *block_list[BLOCK_DATA_COUNT1 + BLOCK_DATA_COUNT2 + 1];

static unsigned int hash2size(unsigned int hash)
{
	if (hash <= BLOCK_DATA_COUNT1)
		return (hash * BLOCK_ALIGNMENT1);
	else
		return (BLOCK_DATA_SIZE1 + (hash - BLOCK_DATA_COUNT1) * BLOCK_ALIGNMENT2);
}

static unsigned int size2hash(unsigned int size)
{
	if (size <= BLOCK_DATA_SIZE1)
		return ((size + BLOCK_ALIGNMENT1 - 1u) / BLOCK_ALIGNMENT1);
	else
		return (BLOCK_DATA_COUNT1 + (size - BLOCK_DATA_SIZE1 + BLOCK_ALIGNMENT2 - 1u) / BLOCK_ALIGNMENT2);
}

void* malloc_alloc(unsigned int size)
{
	unsigned int hash;
	memblock_t *block;
	meminfo_t *info;

	ASSERT(size && !(size & ~0x7FFFFFFFu));
	hash = size2hash(size);

	if (hash2size(hash) > BLOCK_DATA_SIZE - sizeof(meminfo_t)) {
		memblock_large_t *p = (memblock_large_t *)secure_api_ptr[s_HeapAlloc](heap, 0, sizeof(memblock_large_t) + size);

		if (p == NULL)
			panic("%s() %ubytes", "HeapAlloc", size);

		p->next = large_block_list;
		p->prev = NULL;
		if (large_block_list)
			large_block_list->prev = p;
		large_block_list = p;

		p->info.block = NULL;
		p->info.size = (unsigned int)size;
		*(unsigned int *)((unsigned int)&p->info.magic + size) = 0xDEADC0DEu;
		return (void *)&p->info.magic;
	}

	if ((block = block_list[hash]) == NULL) {
		if ((block = block_list[0]) == NULL) {
			unsigned int i;
			for (i = 1024; i; --i) {
				if ((block = (memblock_t *)secure_api_ptr[s_HeapAlloc](heap, 0, sizeof(memblock_t))) == NULL)
					panic("%s() %ubytes", "HeapAlloc", size);
				block->next = block_list[0];
				block_list[0] = block;
			}
			if ((block = (memblock_t *)secure_api_ptr[s_HeapAlloc](heap, 0, sizeof(memblock_t))) == NULL)
				panic("%s() %ubytes", "HeapAlloc", size);
		} else
			block_list[0] = block_list[0]->next;
		block->next = NULL;
		block->prev = NULL;
		block->hash = hash;
		block->size = (unsigned int)(hash2size(hash) + sizeof(meminfo_t));
		block->count = (unsigned int)(BLOCK_DATA_SIZE / block->size);
		block->unfill = 0xFFFFFFFFu;
		block->used = 0;
		block->maxused = 0;
		block_list[hash] = block;
	}

	ASSERT(block->hash == hash &&
		block->size <= BLOCK_DATA_SIZE &&
		block->count > block->used &&
		block->count >= block->maxused &&
		block->used <= block->maxused);

	if (block->unfill == 0xFFFFFFFFu) {
		info = (meminfo_t *)&block->data[block->size * block->maxused];
		++block->used;
		++block->maxused;
	} else {
		info = (meminfo_t *)&block->data[block->size * block->unfill];
		block->unfill = info->size;
		++block->used;
	}

	/* FIXME:
	 * 빈 슬롯이 없는경우 블럭 리스트에서 제외 하는데
	 * 제외된 블럭에서 발생한 메모리 누수는 잡지 못하게 되는 버그가 생김
	 * (해결방법: 전역 블럭 리스트 노트를 따로 추가)
	 */
	if (block->count == block->used) {
		if (block_list[hash] == block)
			block_list[hash] = block->next;
		if (block->next) block->next->prev = block->prev;
		if (block->prev) block->prev->next = block->next;
		/*block->next = NULL;
		block->prev = NULL;*/
	}

	info->block = block;
	info->size = (unsigned int)size;
	*(unsigned int *)((unsigned int)&info->magic + size) = 0xDEADC0DEu;
	return (void *)&info->magic;
}

void malloc_free(void *ptr)
{
	meminfo_t *info;

	if (ptr)
		if (info = (meminfo_t *)((unsigned int)ptr - (unsigned int)&((meminfo_t *)0)->magic), info->size && !(info->size & ~0x7FFFFFFFu)) {
			if (*(unsigned int *)((unsigned int)&info->magic + info->size) == 0xDEADC0DEu) {
				if (info->block) {
					memblock_t *block = info->block;
					if (((unsigned int)info - (unsigned int)block) > (unsigned int)&((memblock_t *)0)->data[-1] && ((unsigned int)info - (unsigned int)block) < sizeof(memblock_t)) {
						info->size = 0;
						if (block->count == block->used) {
							block->next = block_list[block->hash];
							block->prev = NULL;
							if (block_list[block->hash])
								block_list[block->hash]->prev = block;
							block_list[block->hash] = block;
						}
						if (--block->used == 0) {
							if (block_list[block->hash] == block)
								block_list[block->hash] = block->next;
							if (block->next) block->next->prev = block->prev;
							if (block->prev) block->prev->next = block->next;
							block->next = block_list[0];
							block_list[0] = block;
						} else {
							info->size = block->unfill;
							block->unfill = (unsigned int)(((unsigned int)info - (unsigned int)block->data) / block->size);
						}
					} else
						dbg("%s %p", "invalid block", ptr);
				} else {
					memblock_large_t *p = (memblock_large_t *)((unsigned int)ptr - (unsigned int)&((memblock_large_t *)0)->info.magic);
					p->info.size = 0;
					if (p == large_block_list)
						large_block_list = large_block_list->next;
					if (p->next) p->next->prev = p->prev;
					if (p->prev) p->prev->next = p->next;
					if (secure_api_ptr[s_HeapFree](heap, 0, p) == 0)
						dbg("%s %p", "HeapFree()", ptr);
				}
			} else
				dbg("%s %p", "overflowed pointer", ptr);
		} else
			dbg("%s %p", "invalid pointer", ptr);
}

unsigned int malloc_size(void *ptr)
{
	meminfo_t *info;

	if (ptr)
		if (info = (meminfo_t *)((unsigned int)ptr - (unsigned int)&((meminfo_t *)0)->magic), info->size && !(info->size & ~0x7FFFFFFFu))
			if (*(unsigned int *)((unsigned int)&info->magic + info->size) == 0xDEADC0DEu)
				return info->size;

	return 0;
}

void malloc_init(void)
{
	VIRTUALIZER1_START
	{
		if ((heap = (HANDLE)secure_api_ptr[s_HeapCreate](HEAP_NO_SERIALIZE, 0, 0)) != NULL) {
			large_block_list = NULL;
			memset(block_list, 0, sizeof(block_list));
		} else
			secure_api_ptr[s_ExitProcess](0);
	}
	VIRTUALIZER_END
}

void malloc_term(void)
{
	VIRTUALIZER1_START
	{
		secure_api_ptr[s_HeapDestroy](heap);
		heap = NULL;
	}
	VIRTUALIZER_END
}