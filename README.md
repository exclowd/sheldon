#SHELDON

It's a basic shell. 
The following functions are implemented

* echo, cd, pwd
* ';' separated commands
* quoting strings is allowed all over the shell
* ability to launch process in the foreground and background
* ls with flags -a -l with colored output
* pinfo to see process info
* Runs all the commands

### Assumptions:
* No assumptions in input_string size.
* Multiline echo
* Username is truncated to 100 digits
* ~ expansion is supported in echo also, use quotes for character '~'

###  TODO
- [ ]  Implement History through GNU readline
- [ ]  Add support for '()' in the parser
- [ ]  Convert the process list into a hash map

##### File Structure
* main.c : main loop of the shell.
* input_string.c : reads input_string from stdin.
* parse.c : parses the input_string and tokenizes into commands.
* _command.c : Takes a _command and breaks it into _command _name, _args etc. 
Loads the _command into _command list.
* exec.c : executes the _command by calling the respective functions.
* builtins.h : module implementation of all the builtins
* utils.c : general utilities and macros to use everywhere
* cd.c : handles cd and pwd builtins
* echo.c : handles echo builtin
* ls.c : handles ls builtin
