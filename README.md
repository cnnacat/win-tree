super simple program (but do not use, simply for educational purposes only)

default path is '.' (current directory of which the exe would be in) but can take a cli argument of a path so it starts its search there (gotta run the cli with cmd cd'd into the directory the .exe is in though)


acknowledgement to @kddnewton for his posix implementation in c "https://github.com/kddnewton/tree" since i based my implementation on his version

known bugs:
no checks implemented to check whether the program has read-access for directories

todo:
implement output sorting
