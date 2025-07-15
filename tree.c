#include <apiset.h>
#include <fileapi.h>
#include <handleapi.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
#include <winnt.h>
#include <fcntl.h>
#include <io.h>

enum PROGRAM_CODES
{
	MALLOC_FAILURE   = 67,
	FUNCTION_FAILURE = 69,
	HANDLE_FAILURE   = 420,
	FUNCTION_SUCCESS = 0
};

typedef struct node
{
	const wchar_t* file_name;
	bool 		   is_dir;
	struct node*   next;
} node;


// filo linked list
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
	*head_node = this_node;
}


void pop_node(node** head_node)
{
	node* temp_node = (*head_node)->next;
	free((void*)(*head_node)->file_name);
	free(*head_node);

	*head_node = temp_node;
}


int walk(const wchar_t* directory_path, const wchar_t* prefix)
{
	wchar_t search_directory_path[MAX_PATH];
	if (_snwprintf_s(search_directory_path, MAX_PATH, _TRUNCATE, L"%s\\*", directory_path) == -1)
	{
		wprintf(L"_snwprintf_s() failed in walk()");
		return FUNCTION_FAILURE;
	}

	WIN32_FIND_DATAW directory_data;
	HANDLE directory_handle = FindFirstFileW(search_directory_path, &directory_data);
	if (directory_handle == INVALID_HANDLE_VALUE)
	{
		wprintf(L"handle failure");
		return HANDLE_FAILURE;
	}

	node*  head_node      = NULL;
	size_t directory_size = 0;
	do
	{
		wchar_t* this_file_name     = directory_data.cFileName;
		bool     is_this_file_a_dir = false;

		if (directory_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			is_this_file_a_dir = true;

		if ((wcscmp(L".", this_file_name) != 0) && (wcscmp(L"..", this_file_name) != 0))
		{
			node* new_node = init_node(this_file_name, is_this_file_a_dir);
			if(!new_node)
			{
				wprintf(L"malloc failed for the creation of a new node");
				return MALLOC_FAILURE;
			}
			push_node(&head_node, new_node);
			directory_size += 1;
		}

	} while (FindNextFileW(directory_handle, &directory_data));


	CloseHandle(directory_handle);
	if (!head_node)
		return 0;


	wchar_t* tree_branch     = NULL;
	wchar_t* indent_guide    = NULL;
	wchar_t  next_prefix    [MAX_PATH];
	wchar_t  new_search_path[MAX_PATH];
	for (int i = 0; i < directory_size; i++)
	{
		if (i == directory_size-1)
		{
			tree_branch  = L"└── ";
			indent_guide = L"    ";
		}
		else
		{
			tree_branch  = L"├── ";
			indent_guide = L"│   ";
		}
		wprintf(L"%s%s%s\n", prefix, tree_branch, head_node->file_name);


		if (head_node->is_dir)
		{
			if (_snwprintf_s(new_search_path, MAX_PATH, _TRUNCATE, L"%s\\%s", directory_path, head_node->file_name) == -1)
			{
				wprintf(L"_snwprintf_s() failed before initiating a recursive function call");
				return FUNCTION_FAILURE;
			}

			if (_snwprintf_s(next_prefix, MAX_PATH, _TRUNCATE, L"%s%s", prefix, indent_guide) == -1)
			{
				wprintf(L"next_prefix truncated prior to a recursive function call");
				return FUNCTION_FAILURE;
			}

			walk(new_search_path, next_prefix);
		}

		pop_node(&head_node);

	}

	return FUNCTION_SUCCESS;
}


int wmain(int argc, wchar_t* argv[])
{
	// set windows 10 cmd output to unicode-16
	_setmode(_fileno(stdout), _O_U16TEXT);;
	wchar_t* directory_name = NULL;
	if (argc > 1)
	{
		// standardize the formatting of how paths are recieved by walk()
		if (wcscmp(argv[1], L"C:\\"))
			directory_name = L"C:";
		else
			directory_name = argv[1];
	}
	else
		directory_name = L".";

	int return_code = walk(directory_name, L"");
	if (return_code == HANDLE_FAILURE)
	{
		wprintf(L"Either the WinAPI failed, or your input contained a path that was not valid.");
		wprintf(L"Make sure to copy and paste the path from File Explorer, don't write it out.\n");
	}

	getwchar();
	return 0;
}
