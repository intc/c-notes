#!/usr/bin/env bash

DEBUG_INFO="-ggdb"
# -----------------------------------------------------------------------------

CC="gcc"
BNAME="erat"
CFLAGS="-O2\
	-std=c11\
	-fomit-frame-pointer\
	-Wall\
	-Wmissing-prototypes\
	-Wstrict-prototypes\
	-Wextra"
#	-Wpedantic"
LIBS="-lpthread -lm"
DEF="-D_NO_RACE" # <- Comment this out to see how it behaves *without* mutex.
SRC="${BNAME}.c"

if [ -z "${DEBUG_INFO}" ] ; then
	CFLAGS="-O2 ${CFLAGS}"
	D_DEBUG="-DNDEBUG" # <- This disables assertions.
else
	D_DEBUG="-D_DEBUG"
fi

eecho() { >&2 echo $@; }

eecho "Building: ${SRC} -> ${BNAME}"; eecho

BL="${CC} ${D_DEBUG} -D_BNAME=\"${BNAME}\" ${DEBUG_INFO} ${DEF}\
	${CFLAGS}\
	${LIBS}\
	${SRC}\
	-o ${BNAME}"

eecho "${BL}"; eecho

${BL}
