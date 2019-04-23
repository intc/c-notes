#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _LUAJIT
#include "luajit.h"
#define LUA "luajit"
#else
#define LUA "lua"
#endif

int file_stat_check(const char * fn);
void lf_error_print(int load_stat, const char *fn);

int main(int argc, char *argv[]) {

	if ( argc != 2 ) {
		fprintf(stderr, "Usage example: e%s [luafile.lua]\n", LUA); return 1;
	}
	
	const char *fn = argv[1];
	if ( file_stat_check(fn) != 0 ) {
		fprintf(stderr, "%s - File not found\n", fn); return 1;
	}

	lua_State *lua = luaL_newstate();
			/* http://pgl.yoyo.org/luai/i/luaL_newstate */
	if ( lua == NULL ) {
		fprintf(stderr, "Error: Failed to allocate a new %s state.\n", LUA);
		return 1;
	}

	luaL_openlibs(lua);
			/* http://pgl.yoyo.org/luai/i/luaL_openlibs */
	int load_stat = luaL_loadfile(lua, fn);
			/* http://pgl.yoyo.org/luai/i/luaL_loadfile */
	if ( load_stat != 0 ) {
		lf_error_print(load_stat, fn);
		return -1;
	}

#ifdef _LUAJIT
	int jit_status = luaJIT_setmode(lua, 0, LUAJIT_MODE_ENGINE|LUAJIT_MODE_ON);
			/* https://luajit.org/ext_c_api.html */
			/* This probably is enabled by default? */
	if ( jit_status != 1 ) {
		fprintf(stderr, "JIT setmode failed: %i\n", jit_status);
		return 1;
	}
#endif

	fprintf(stderr, "Running %s with %s:\n", fn, LUA);
	int run_stat = lua_pcall(lua, 0, 0, 0);
			/* http://pgl.yoyo.org/luai/i/lua_pcall */
	if ( run_stat != 0 ) {
		fprintf(stderr, "Error: %s\n", lua_tostring(lua, -1));
			/* http://pgl.yoyo.org/luai/i/lua_tostring */
	}
}

int file_stat_check(const char *fn) {
	struct stat st;
	return stat(fn, &st);
}

void lf_error_print(int load_stat, const char *fn) {
	fprintf(stderr, "ERROR: %s - ", fn);
	switch ( load_stat ) {

		case LUA_ERRSYNTAX:
			fprintf(stderr, "Synax error.\n"); break;

		case LUA_ERRMEM:
			fprintf(stderr, "Memory allocation error.\n"); break;

		case LUA_ERRFILE:
			fprintf(stderr, "Failed to read the file.\n"); break;

		default:
			fprintf(stderr, "Unknown error.\n");
	}
}

/*   Check:
 *   + man 2 stat for stat usage and struct stat
 *
 *   Links:
 *   + http://luajit.org/ext_c_api.html
 *   + http://pgl.yoyo.org/luai/i/about
 *
 *   Compilation:
 *   Check ./compile.sh
 *
 *   Notes:
 *   Tested on 64 bit Gentoo Linux
 *
 *   (C) Antti Antinoja, 2019
 */

