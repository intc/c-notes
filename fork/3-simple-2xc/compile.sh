#!/usr/bin/env bash

DEBUG_INFO="-ggdb"
# -----------------------------------------------------------------------------

CC="gcc"
BNAME="simple2c"
CFLAGS="-O2\
	-std=c11\
	-fomit-frame-pointer\
	-Wall\
	-Wmissing-prototypes\
	-Wstrict-prototypes\
	-Wextra\
	-Wpedantic\
	-D_POSIX_SOURCE\
	-D_GNU_SOURCE"
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
