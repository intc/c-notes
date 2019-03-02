#!/usr/bin/env bash

DEBUG="true"

# -----------------------------------------------------------------------------

CC="gcc"
BNAME="ppointer_addr"
CFLAGS="-O2\
    -std=c11\
    -fomit-frame-pointer\
    -Wall\
    -Wmissing-prototypes\
    -Wstrict-prototypes\
    -Wextra\
    -Wpedantic"
SRC="ppointer_addr.c"

if [ "${DEBUG}" == "true" ]; then
	echo "Applying debugging symbols and activating debugging code"
	DEBUG_SYM="-ggdb"
	D_DEBUG="-D_DEBUG"
	STRIP="no"
fi

echo "Building: ${SRC} -> ${BNAME}"

${CC} ${D_DEBUG} -D_BNAME=\"${BNAME}\" ${DEBUG_SYM}\
	${CFLAGS}\
	${SRC}\
	-o ${BNAME}

if [ "${STRIP}" != "no" ]; then
	echo "Stripping ./${BNAME}"; strip ./${BNAME}
fi
