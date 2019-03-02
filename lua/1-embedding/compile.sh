#!/usr/bin/env bash

# -----------------------------------------------------------------------------

CC="gcc"
# DEBUG="-ggdb"
CFLAGS="-O2\
	-std=c11\
	-fomit-frame-pointer\
	-Wall\
	-Wmissing-prototypes\
	-Wstrict-prototypes\
	-Wextra\
	-Wpedantic"

SRC="embed-lua.c"

LUAJIT=$(pkg-config luajit --cflags --libs)
LUASTD=$(pkg-config lua --cflags --libs)

eecho() { >&2 echo $@; }

if [ -z "${LUAJIT}" ]; then
	eecho "Luajit was not found by pkg-config. Trying standard Lua."
	if [ -z "${LUASTD}" ]; then
		eecho "ERROR: Lua was not found by pkg-config. Can not contine."
		exit 1
	else
		eecho "Using standard Lua!"
		LUA="${LUASTD}"
		BNAME="elua"
	fi
else
	LUA="-D_LUAJIT ${LUAJIT}"
	BNAME="eluajit"
fi

${CC} ${DEBUG} ${LUA} ${CFLAGS} ${SRC} -o ${BNAME}
