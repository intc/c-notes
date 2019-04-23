#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>

#ifdef _LUAJIT
#include "luajit.h"
#define LUA "luajit"
#else
#define LUA "lua"
#endif

#define LUA_FILE "./lua.lua"
# define TRUE 1
# define FALSE 0

int file_stat_check(const char * fn);
void lf_error_print(int load_stat, const char *fn);
static int lua_sleep(lua_State *Ls);
static int lua_string_print(lua_State *);
static int lua_string_reverse(lua_State *);

int main(void) {
	
	const char fn[] = LUA_FILE;
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

	lua_register(lua, "sleep", lua_sleep);
	lua_register(lua, "string_print", lua_string_print);
	lua_register(lua, "string_reverse", lua_string_reverse);

	fprintf(stderr, "Running %s with %s:\n", fn, LUA);
	int run_stat = lua_pcall(lua, 0, 0, 0);
			/* http://pgl.yoyo.org/luai/i/lua_pcall */
	if ( run_stat != 0 ) {
		fprintf(stderr, "Error: %s\n", lua_tostring(lua, -1));
			/* http://pgl.yoyo.org/luai/i/lua_tostring */
	}

	return 0;
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

static int lua_sleep(lua_State *Ls){
	int msec  = luaL_checkinteger(Ls, 1);
	poll(NULL, 0, msec);
	return 1;
}

static int lua_string_print(lua_State *Ls){
	const char *str = luaL_checkstring(Ls, 1);
	printf(str);
	return 0;
}

static int lua_string_reverse(lua_State *Ls){
	const char *str = luaL_checkstring(Ls, 1);
	size_t len = strlen(str);
	size_t x = 0;
	int add_nl = FALSE;
	char *new_str = malloc( sizeof(char) * len + 1 );
	if ( str[len-1] == '\n' ) {
		len--;
		add_nl = TRUE;
	}
	while ( len-- ) {
		new_str[x] = str[len];
		x++;
	}
	if ( add_nl ) {
		new_str[x] = '\n';
		x++;
	}
	new_str[x]='\0';
	lua_pushstring(Ls, new_str);
	free(new_str);
	return 1;
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

