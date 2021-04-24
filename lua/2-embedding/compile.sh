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

eecho() { >&2 echo $@; }

LUAJIT=$(pkg-config luajit --cflags --libs)

LUA_NAMES=(
	"lua5.1"
	"lua"
)

for n in "${LUA_NAMES[@]}" ; do echo "# Lua: trying pkg-config ${n}"; pkg-config ${n} && LUA_NAME=$n && break ; done

if ! [ -z "${LUA_NAME}" ] ; then
	eecho "# Found ${LUA_NAME}"
	LUASTD=$(pkg-config ${LUA_NAME} --cflags --libs)
fi

if [ -z "${LUAJIT}" ]; then
	if [ -z "${LUASTD}" ]; then
		eecho "ERROR: pkg-config failed on both Lua and Luajit. Can not contine." ; exit 1
	else
		eecho "# Using standard Lua!"
		LUA="${LUASTD}"
		BNAME="elua"
	fi
else
	eecho "# Using luajit!"
	LUA="-D_LUAJIT ${LUAJIT}"
	BNAME="eluajit"
fi

${CC} ${DEBUG} ${LUA} ${CFLAGS} ${SRC} -o ${BNAME} && eecho "# Binary is ./${BNAME}"
