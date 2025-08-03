#pragma once

#include <windows.h>
#include <stdbool.h>
#include <wchar.h>

typedef struct node
{
	const wchar_t* file_name;
	bool           is_dir;
	struct node*   next;
} node;


node* init_node         (const wchar_t* file_name, bool is_dir);
void  push_node         (node** head_node, node* this_node);
void  pop_node          (node** head_node);
void  flush_linked_list (node* head_node);
