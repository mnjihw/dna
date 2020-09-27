#include "common.h"


list_t* create_list()
{
	list_t *list;

	if ((list = (list_t*)malloc(sizeof(list_t))))
		memset(list, 0, sizeof(list));
	return list;
}



void remove_element(list_t *list, unsigned int pos)
{
	unsigned int i, count;
	node_t *prev, *target_node;

	if (!list)
		return;

	count = list->count;

	if (pos >= 0 && pos < count)
	{
		if (pos == 0)
		{
			target_node = list->head;
			list->head = list->head->next;
			free(target_node);
			target_node = NULL;
		}
		else
		{
			prev = list->head;

			for (i = 0; i < pos - 1; ++i)
				prev = prev->next;
			target_node = prev->next;
			prev->next = target_node->next;
			free(target_node);
			target_node = NULL;
		}
		--list->count;
	}
}

void clear_list(list_t *list)
{
	while (list && list->count > 0)
		remove_element(list, 0);
}

void delete_list(list_t *list)
{
	if (list)
	{
		clear_list(list);
		free(list);
	}
}

void add_element(list_t *list, node_t element)
{
	node_t *last_node, *new_node;

	if (!list)
		return;
	if (!(new_node = (node_t*)malloc(sizeof(node_t))))
		return;
	*new_node = element;
	new_node->next = NULL;

	if (list->count == 0)
		list->head = new_node;
	else
	{
		last_node = list->head;
		while (last_node->next)
			last_node = last_node->next;
		last_node->next = new_node;
	}
	++list->count;
}

void insert_element(list_t *list, unsigned int pos, node_t element)
{
	unsigned int i;
	node_t *prev, *new_node;

	if (!list)
		return;
	if (!(new_node = (node_t*)malloc(sizeof(node_t))))
		return;
	*new_node = element;

	if (pos == 0)
	{
		new_node->next = list->head;
		list->head = new_node;
	}
	else
	{
		prev = list->head;
		for (i = 0; i < pos - 1; ++i)
			prev = prev->next;
		new_node->next = prev->next;
		prev->next = new_node;
	}
	++list->count;
}