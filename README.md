#SHELDON

It's a basic shell. 
The following functions are implemented

* `echo`, `cd`, `pwd`
* ';' separated commands
* quoting strings is allowed all over the shell
* `ls` with flags `-a -l` with colored output
* `pinfo` to see process info
* Runs all the commands
* job handling through `fg`, `bg`, `kjob`, `jobs` and `overkill`
* handles signals like `Ctrl-Z` and `Ctrl-C`
* `exit` to exit the shell
* `setenv` and `unsetenv` environment variables, use `getenv` to print their value
* Pipleline support and I/O redirection


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
* command.c : Takes a command and breaks it into command _name, args etc. 
Loads the command into command list.
* exec.c : executes the command by calling the respective functions.
* builtins.h : module implementation of all the builtins
* utils.c : general utilities and macros to use everywhere
* cd.c : handles cd and pwd builtins
* echo.c : handles echo builtin
* ls.c : handles ls builtin
* env.c : handles setting and unsetting of environment variables
* jobs.c : handles the job control of the program
* prompt.c : handles the prompt functionality of the shell
