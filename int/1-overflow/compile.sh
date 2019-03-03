#!/usr/bin/env bash

DEBUG_INFO="-ggdb"

# -----------------------------------------------------------------------------

CC="gcc"
BNAME="int_ovrflw"
CFLAGS="-O2\
	-std=c11\
	-fomit-frame-pointer\
	-Wall\
	-Wmissing-prototypes\
	-Wstrict-prototypes\
	-Wextra\
	-Wpedantic"

SRC="int_ovrflw.c"

eecho() { >&2 echo $@; }

eecho "Building: ${SRC} -> ${BNAME}"; eecho

BL="${CC} ${D_DEBUG} -D_BNAME=\"${BNAME}\" ${DEBUG_INFO}\
	${CFLAGS}\
	${SRC}\
	-o ${BNAME}"

eecho "${BL}"; eecho

${BL}
