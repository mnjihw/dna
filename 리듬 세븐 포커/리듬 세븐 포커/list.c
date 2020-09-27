#include "common.h"

void create_list(list_t **pList)
{
	if ((*pList = (list_t*)malloc(sizeof(list_t))))
		memset(*pList, 0, sizeof(list_t));
	else
	{
		exit(0);
	}
}

void add_element(list_t *list, node_t element, BOOL sort)
{
	node_t *last_node, *new_node;

	if (list)
	{
		new_node = (node_t*)malloc(sizeof(node_t));

		if (new_node)
		{
			*new_node = element;
			new_node->next = NULL;

			last_node = &list->head;

			if (sort)
			{
				while (last_node->next)
				{
					if ((last_node->next->number < new_node->number) || ((last_node->next->number == new_node->number) && (last_node->next->suit < new_node->suit)))
					{
						new_node->next = last_node->next;
						last_node->next = new_node;
						break;
					}
					last_node = last_node->next;
				}
				if (!last_node->next)
					last_node->next = new_node;
			}
			else
			{
				while (last_node->next)
					last_node = last_node->next;
				last_node->next = new_node;
			}
			++list->count;
		}
	}
}



void insert_element(list_t *list, int position, node_t element)
{
	int i;
	node_t *prev_node, *new_node;

	if (list)
	{
		if (position >= 0 && position <= list->count)
		{
			new_node = (node_t*)malloc(sizeof(node_t));

			if (new_node)
			{
				*new_node = element;
				new_node->next = NULL;

				prev_node = &list->head;
				for (i = 0; i < position; ++i)
					prev_node = prev_node->next;

				new_node->next = prev_node->next;
				prev_node->next = new_node;
				++list->count;
			}
		}
	}
}


void remove_element(list_t *list, int position)
{
	int i;
	node_t *node, *del_node;

	if (list)
	{
		if (position >= 0 && position < list->count)
		{
			node = &list->head;
			for (i = 0; i < position; ++i)
				node = node->next;

			del_node = node->next;
			node->next = del_node->next;
			free(del_node);
			--list->count;
		}
	}
}


node_t* get_element(list_t *list, int position)
{
	node_t *node, *ret_node = NULL;
	int i;

	if (list)
	{
		if (position >= 0 && position < list->count)
		{
			node = &list->head;
			for (i = 0; i <= position; ++i)
				node = node->next;
			ret_node = node;
		}
	}
	return ret_node;
}

void clear_list(list_t *list)
{
	if (list)
	{
		while (list->count > 0)
			remove_element(list, 0);
	}
}


void delete_list(list_t *list)
{
	if (list)
	{
		clear_list(list);
		free(list);
	}
}