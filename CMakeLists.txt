cmake_minimum_required(VERSION 3.17)
project(sheldon C)

set(CMAKE_C_STANDARD 99)

add_executable(sheldon main.c command.c command.h parse.c parse.h input.c input.h builtins/cd.c builtins/cd.h builtins.h utils.c utils.h exec.c exec.h builtins/echo.c builtins/echo.h jobs.c jobs.h builtins/ls.c builtins/ls.h builtins/pinfo.c builtins/pinfo.h builtins/fg.c builtins/fg.h builtins/bg.c builtins/bg.h builtins/env.c builtins/env.h)