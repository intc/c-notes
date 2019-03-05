#!/usr/bin/env bash

DEBUG_INFO="-ggdb"
# -----------------------------------------------------------------------------

CC="gcc"
BNAME="tcreate"
CFLAGS="-O2\
	-std=c11\
	-fomit-frame-pointer\
	-Wall\
	-Wmissing-prototypes\
	-Wstrict-prototypes\
	-Wextra\
	-Wpedantic"
LIBS="-lpthread"
SRC="${BNAME}.c"

if [ -z "${DEBUG_INFO}" ] ; then
	CFLAGS="-O2 ${CFLAGS}"
	D_DEBUG="-DNDEBUG" # <- This disables assertions.
else
	D_DEBUG="-D_DEBUG"
fi

eecho() { >&2 echo $@; }

eecho "Building: ${SRC} -> ${BNAME}"; eecho

BL="${CC} ${D_DEBUG} -D_BNAME=\"${BNAME}\" ${DEBUG_INFO}\
	${CFLAGS}\
	${LIBS}\
	${SRC}\
	-o ${BNAME}"

eecho "${BL}"; eecho

${BL}
