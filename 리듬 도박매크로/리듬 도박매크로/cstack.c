#include "common.h"

int cstack_isempty(cstack_int_t *s)
{
	return s->top == -1;
}

int cstack_isfull(cstack_int_t *s)
{
	return s->top >= MAX_STACK_SIZE - 1;
}

void cstack_push_int(cstack_int_t *s, int value)
{
	if (cstack_isfull(s))
		return;
	s->data[++s->top] = value;
}


void cstack_push_double(cstack_double_t *s, double value)
{
	if (cstack_isfull((cstack_int_t*) s))
		return;
	s->data[++s->top] = value;
}


int cstack_pop_int(cstack_int_t *s)
{
	if (cstack_isempty(s))
		return -1;
	else
		return s->data[s->top--];
}

double cstack_pop_double(cstack_double_t *s)
{
	if (cstack_isempty((cstack_int_t*) s))
		return -1;
	else
		return s->data[s->top--];
}



int cstack_peek_int(cstack_int_t *s)
{
	if (cstack_isempty(s))
		return -1;
	else
		return s->data[s->top];
}

double cstack_peek_double(cstack_double_t *s)
{
	if (cstack_isempty((cstack_int_t*) s))
		return -1; 
	else
		return s->data[s->top];
}

cstack_int_t* cstack_alloc()
{
	cstack_int_t *s;

	s = (cstack_int_t*) malloc(sizeof(cstack_int_t));
	s->top = -1;

	return s;
}

void cstack_free(cstack_int_t *s)
{
	free(s);
}