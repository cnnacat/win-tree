#include "unicode.h"

#include "ll/ll.h"
#include "tree.h"
#include "log/log.h"
#include "libs/getopt.h"

// I literally never use this, at least effectively
enum PROGRAM_CODES
{
	MALLOC_FAILURE   = 67,
	FUNCTION_FAILURE = 69,
	HANDLE_FAILURE   = 420,
	FUNCTION_SUCCESS = 0,
	ACCESS_DENIED    = 1337
};


void hang()
{
	wprintf(L"\nPress ENTER to quit: ");
	getwchar();
}


void get_cli_args(
	int       argc, 
	wchar_t*  argv[], 
	wchar_t** directory_path, 
	int*      log_flag)
{

	int get_opt_return_code;
	int option_index;

	while (true)
	{
		static struct option long_option[] =
		{
			{L"log", no_argument, 0, L'L'},
			{L"directory", optional_argument, 0, L'D'},
			{0, 0, 0, 0}
		};

		get_opt_return_code = getopt_long_w(argc, argv, L"D::", long_option, &option_index);
		if (get_opt_return_code == -1)
			break;

		switch(get_opt_return_code){
		
		case L'D':
			// Standardize the format of the C:
			if (wcscmp(L"C:\\", optarg) == 0)
				*directory_path = L"C:";
			else
				*directory_path = _wcsdup(optarg);

			break;

		case L'L':
			*log_flag = true;
			break;

		case L'?':
			break;

		default:
			abort();
		}
	}

	// Set default path if not given
	if (*directory_path == NULL)
		*directory_path = L".";
}

// "Directory Path" refers to the literal path of a directory
// "Search Path" or anything that sounds similar refers to %s\\*, where %s is the
// directory path, because otherwise FindFirstFile refuses to index into it.

void traverse(
	wchar_t*         directory_path,
	WIN32_FIND_DATAW file_data,
	HANDLE           file_handle, 
	wchar_t*         prefix, 
	log_node**       log_head)
{

	// Since the check on whether the program has access to a directory was prior to this
	// function call, assume read-only priveleges for the directory given in the parameter.

	node* head_node = NULL;
	do
	{
		wchar_t* file_name = file_data.cFileName;
		bool     dir       = false;

		// Skip the directories which are reparse points and hidden directories 
		bool is_directory = file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		bool is_reparse_point = file_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT;
		bool is_hidden = (file_data.dwFileAttributes & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) == (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN);

		if (is_directory && !is_reparse_point && !is_hidden)
			dir = true;

		if ((wcscmp(L".", file_name) != 0) && (wcscmp(L"..", file_name) != 0))
		{
			node* new_file_node = init_node(file_name, dir);
			if (!new_file_node)
			{
				wprintf(L"Failed to allocate memory for a node for file: %s", file_name);
				return;
			}

			if (!head_node
				|| wcscmp(new_file_node->file_name, head_node->file_name) < 0)
				push_node(&head_node, new_file_node);

			else if (wcscmp(new_file_node->file_name, head_node->file_name) > 0)
			{
				node* current_node;
				for (
					current_node = head_node; 
					current_node->next && wcscmp(new_file_node->file_name, current_node->file_name) > 0; 
					current_node = current_node->next
					);

				new_file_node->next = current_node->next;
				current_node->next  = new_file_node;
			}
		}

	} while (FindNextFileW(file_handle, &file_data));

	if (!head_node)
		return;

	// I can probably break the following code after this comment into
	// its own function. 

	wchar_t* tree_branch  = NULL;
	wchar_t* indent_guide = NULL;
	wchar_t next_prefix       [MAX_PATH];
	wchar_t new_directory_path[MAX_PATH];
	wchar_t new_search_path   [MAX_PATH];

	while (head_node)
	{
		if (head_node->next == NULL)
		{
			tree_branch  = L"└── ";
			indent_guide = L"    ";	  
		}
		else
		{
			tree_branch  = L"├── ";
			indent_guide = L"│   ";
		}

		if (head_node->is_dir)
		{
			// Create the search path for FindFirstFile purposes
			if (_snwprintf_s(
				new_search_path, 
				MAX_PATH, 
				_TRUNCATE, 
				L"%s\\%s\\*", 
				directory_path, 
				head_node->file_name) == -1)	
			{
				wprintf(L"Truncation occured while trying to create the search path for the directory: %s", head_node->file_name);
				return;
			}

			// Create the directory path so futher function calls can create their search path
			if (_snwprintf_s(
				new_directory_path, 
				MAX_PATH, 
				_TRUNCATE, 
				L"%s\\%s", 
				directory_path, 
				head_node->file_name) == -1)
			{
				wprintf(L"Truncation occured while trying to create the directory path for the directory: %s", head_node->file_name);
				return;
			}

			// Create the whatever the fuck is a good name to call this; The indent? Spacing? For tree printing purposes
			if (_snwprintf_s(
				next_prefix, 
				MAX_PATH,
				_TRUNCATE, 
				L"%s%s", 
				prefix, 
				indent_guide) == -1)
			{
				wprintf(L"Truncation occured while trying to create the next indent guide for directory: %s", head_node->file_name);
				return;
			}


			WIN32_FIND_DATAW new_file_data;
			
			HANDLE new_file_handle = FindFirstFileW(new_search_path, &new_file_data);
			if (new_file_handle == INVALID_HANDLE_VALUE)
			{
				DWORD error = GetLastError();
				if (error == ERROR_ACCESS_DENIED)
				{
					wchar_t message_buffer[4096] = L"Access Denied: ";
					StringCchCatW(
						message_buffer, 
						4096, 
						new_directory_path);

					log_node* new_log_node = init_log_node(message_buffer);
					push_log_node(log_head, new_log_node);
					continue;
				}
				else if (error == ERROR_PATH_NOT_FOUND)
				{
					wchar_t message_buffer[4096] = L"Path not found: ";
					StringCchCatW(
						message_buffer, 
						4096, 
						new_directory_path);

					log_node* new_log_node = init_log_node(message_buffer);
					push_log_node(log_head, new_log_node);
					continue; 
				}
			}

			wprintf(
				L"%s%s%s\n", 
				prefix, 
				tree_branch, 
				head_node->file_name);

			traverse(
				new_directory_path, 
				new_file_data, 
				new_file_handle, 
				next_prefix, 
				log_head);
		}
		else
			wprintf(
				L"%s%s%s\n", 
				prefix, 
				tree_branch, 
				head_node->file_name);

		pop_node(&head_node);		
	}

	return;
}


int wmain(
	int      argc, 
	wchar_t* argv[])
{

	// set cmd output to unicode
	_setmode(_fileno(stdout), _O_U16TEXT);

	int      log_flag       = false;
	wchar_t* directory_path = NULL;

	get_cli_args(
		argc, 
		argv, 
		&directory_path, 
		&log_flag);


	WIN32_FIND_DATAW origin_file_data;
	wchar_t          origin_search_directory_path[MAX_PATH];

	if (_snwprintf_s(
		origin_search_directory_path, 
		MAX_PATH, 
		_TRUNCATE, 
		L"%s\\*", 
		directory_path) == -1)
	{
		wprintf(L"Truncation occured in wmain for origin_search_directory_path");
		exit(FUNCTION_FAILURE);
	}

	HANDLE origin_search_handle = FindFirstFileW(origin_search_directory_path, &origin_file_data);
	if (origin_search_handle == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		if (error == ERROR_ACCESS_DENIED)
			wprintf(L"Access denied to %s\n", directory_path);

		if (error == ERROR_PATH_NOT_FOUND)
			wprintf(L"Path not found to %s\n", directory_path);

		hang();
		exit(FUNCTION_FAILURE);
	}
	
	wprintf(L"Searching path: '%s'\n", directory_path);

	log_node* log_head = NULL;
	// give traverse the path, the prefix, and the error node's head
	traverse(
		directory_path, 
		origin_file_data, 
		origin_search_handle, 
		L"",
		&log_head);


	if (log_flag)
	{	
		wprintf(L"\nLog output in C:\\tree_output \n\n");
		
		if (!log_head)
		{
			wchar_t message[4096] = L"No errors while searching path: ";
			StringCchCatW(message, 
				4096, 
				directory_path);

			log_node* no_err = init_log_node(message);
			push_log_node(&log_head, no_err);
		}

		// this frees all malloc-ed memory in the error's linked list
		output_log_file(&log_head);
	}


	FindClose(origin_search_handle);
	hang();	
	return 0;
}
