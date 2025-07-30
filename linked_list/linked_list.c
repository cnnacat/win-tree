#include "linked_list.h"


node* init_node(const wchar_t* file_name, bool is_dir)
{
	node* new_node = (node*)malloc(sizeof(node));
	if (!new_node)
	{
		wprintf(L"malloc fail in init_node");
		return NULL;
	}

	new_node->file_name      = _wcsdup(file_name);
	new_node->is_dir         = is_dir;
	new_node->next           = NULL;

	return new_node;
}


void push_node(node** head_node, node* this_node)
{
	this_node->next = *head_node;
	*head_node      = this_node;
}


void pop_node(node** head_node)
{
	node* temp_node = (*head_node)->next;
	free((void*)(*head_node)->file_name);
	free(*head_node);

	*head_node = temp_node;
}