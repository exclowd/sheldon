

add_executable(sheldon
        src/main.c
        src/command.c src/command.h
        src/parse.c src/parse.h
        src/input.c src/input.h
        src/prompt.c src/prompt.h
        src/utils.c src/utils.h
        src/exec.c src/exec.h
        src/jobs.c src/jobs.h
        src/builtins.h
        builtins/cd.c builtins/cd.h
        builtins/echo.c builtins/echo.h
        builtins/ls.c builtins/ls.h
        builtins/pinfo.c builtins/pinfo.h
        builtins/env.c builtins/env.h
        )
