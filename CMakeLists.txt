cmake_minimum_required(VERSION 3.17)
project(sheldon C)

set(CMAKE_C_STANDARD 99)

add_executable(
        sheldon src/main.c src/command.c src/command.h src/parse.c src/parse.h src/input.c src/input.h
        builtins/cd.c builtins/cd.h src/builtins.h src/utils.c src/utils.h src/exec.c src/exec.h builtins/echo.c
        builtins/echo.h src/jobs.c src/jobs.h builtins/ls.c builtins/ls.h builtins/pinfo.c builtins/pinfo.h
        builtins/env.c builtins/env.h
        src/prompt.c src/prompt.h)