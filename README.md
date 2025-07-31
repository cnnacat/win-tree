# A relatively simple import of GNU's tree to Windows

A project that I procrastinated on HARD while trying to learn about file i/o, Win32 API, recursion, linked lists and C. 
Should work PROPERLY, unlike the first release of this program.

# Design

Design choices included in this program was how I'd like my tree program to look like, so..

- Sorted output
- Prints out the names of restricted directories, but does NOT access them
- Prints basically out literally anything, so long as it can get its name

# Project specifications

- Uses a FILO linked list, but funnily enough the O(n) sorting algorithm basically converts it to a FIFO linked list
- Outputs an error (well, more like a log) log at the end of execution in C:\tree_output if --log / -L is one of the CLI arguments
- Really nothing special, it's a tree program

# How to run
Either download the source files and run cmake or download the executable directly.

Takes an OPTIONAL --directory / -D CLI argument if you want to print a tree of a specific directory.
Without any arguments, the program will print a tree of the directory that it's in.

Takes an OPTIONAL --log / -L CLI argument if you want to print any errors that the program runs into, since any errors except path not found for the entry point directory will be ignored during the execution of the program.


# Acknowledgement

I pulled some tricks out of kddnewton's implementation of GNU's tree in C. Check it out, he's got a whole repo full of tree implementations in all differerent languages.

<https://github.com/kddnewton/tree/tree/main>