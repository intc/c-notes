#!/usr/bin/env bash

DEBUG_INFO="-ggdb"

# -----------------------------------------------------------------------------

CC="gcc"
BNAME="float128"
CFLAGS="-std=c11\
    -fomit-frame-pointer\
    -Wall\
    -Wmissing-prototypes\
    -Wstrict-prototypes\
    -Wextra\
    -Wpedantic"
LIBS="-lquadmath"

SRC="${BNAME}.c"

if [ -z "${DEBUG_INFO}" ] ; then
	CFLAGS="-O2 ${CFLAGS}"
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
