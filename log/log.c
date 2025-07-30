#include "../unicode.h"
#include "log.h"

log_node* init_log_node(const wchar_t* message)
{
	log_node* new_log_node = (log_node*)malloc(sizeof(log_node));
	if (!new_log_node)
	{
		wprintf(L"malloc fail in init_node");
		return NULL;
	}

	new_log_node->message = _wcsdup(message);
	new_log_node->next    = NULL;

	return new_log_node;
}


void push_log_node(log_node** head_log_node, log_node* this_log_node)
{
	this_log_node->next = *head_log_node;
	*head_log_node      = this_log_node;
}


void pop_log_node(log_node** head_log_node)
{
	log_node* temp_error_node = (*head_log_node)->next;
	free((void*)(*head_log_node)->message);
	free(*head_log_node);

	*head_log_node = temp_error_node;
}


void output_log_file(log_node** head_log_node)
{
	if (!(*head_log_node))
		return;

	if (!CreateDirectoryW(L"C:\\tree_output", NULL) 
		&& GetLastError() != ERROR_ALREADY_EXISTS)
	{
		wprintf(L"Failed to create path for logging: %d\n", GetLastError());
		return;
	}

	HANDLE file_handle = CreateFileW(
		L"C:\\tree_output\\error.txt",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (file_handle == INVALID_HANDLE_VALUE)	
	{
		wprintf(L"Failed to create initialize logging file handle: %d\n", GetLastError());
		return;
	}

	do
	{
		size_t bytes = wcslen((*head_log_node)->message) * sizeof(wchar_t);
		WriteFile(file_handle, (*head_log_node)->message, bytes, NULL, NULL);
		pop_log_node(head_log_node);

	} while(*head_log_node);

	CloseHandle(file_handle);
}
