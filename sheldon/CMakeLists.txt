set(SRC_DIR src)
set(BUILTIN_DIR builtins)

set(
        SOURCES
        ${SRC_DIR}/main.c
        ${SRC_DIR}/command.c
        ${SRC_DIR}/parse.c
        ${SRC_DIR}/input.c
        ${SRC_DIR}/prompt.c
        ${SRC_DIR}/utils.c
        ${SRC_DIR}/exec.c
        ${SRC_DIR}/jobs.c

)

set(
        BUILTINS
        ${BUILTIN_DIR}/cd.c
        ${BUILTIN_DIR}/echo.c
        ${BUILTIN_DIR}/ls.c
        ${BUILTIN_DIR}/pinfo.c
        ${BUILTIN_DIR}/env.c
)


add_executable(
        sheldon
        ${SOURCES}
        ${BUILTINS}
)
