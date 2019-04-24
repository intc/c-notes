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
#define TRUE 1
#define FALSE 0

#define U8BNEXT 0x80 /* 0y10000000 */
			/* utf-8 mask: */
#define U8B1_2M 0xe0 /* 0y11100000 */
#define U8B1_3M 0xf0 /* 0y11110000 */
#define U8B1_4M 0xf8 /* 0y11111000 */
			/* utf-8 values for multibyte characters: */
#define U8B1_2  0xc0 /* 0y11000000 */
#define U8B1_3  0xe0 /* 0y11100000 */
#define U8B1_4  0xf0 /* 0y11110000 */

int file_stat_check(const char * fn);
void lf_error_print(int load_stat, const char *fn);
			/* Registered for LUA: */
static int lua_sleep(lua_State *Ls);
static int lua_string_print(lua_State *);
static int lua_string_reverse(lua_State *);
			/* Debug: */
void print_string_bytes(const char *);
void print_char_bits(char);
			/* UTF-8: */
int has_utf8_bytes(const char *, int);

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
			/* Register C functions for LUA: */
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

static int lua_sleep(lua_State *Ls) {
	int msec  = luaL_checkinteger(Ls, 1);
	poll(NULL, 0, msec);
	return 1;
}

static int lua_string_print(lua_State *Ls) {
	const char *str = luaL_checkstring(Ls, 1);
	printf(str);
	return 0;
}

static int lua_string_reverse(lua_State *Ls) {
	const char *str = luaL_checkstring(Ls, 1);
	size_t len = strlen(str);
	size_t n_pos = len - 1;
	char *new_str = malloc( sizeof(char) * len + 1 );
			/* One needs to terminate the string with NULL character: */
	new_str[len] = '\0';
			/* In case the original string ends with new line
			 * we terminate the new string also with new line: */
	if ( str[n_pos] == '\n' ) {
		new_str[n_pos] = '\n'; n_pos--;
	}
			/* Set our goal (max) */
	size_t max = n_pos;
	size_t s_pos = 0;
	while ( s_pos <= max ) {
			/* We need to add 1 for length (n_pos is index): */
		size_t utf_bytes = has_utf8_bytes(str+s_pos, n_pos+1);
		if ( utf_bytes ) {
			/* We are here because a variable width (utf-8) character was detected. */
			size_t utf_pos = utf_bytes;
			while( utf_pos-- ) {
				new_str[n_pos] = str[s_pos+utf_pos];
				n_pos--;
			}
			/* Advance s_pos by the amount the utf8 character consumed: */
			s_pos = s_pos + utf_bytes;
		} else {
			new_str[n_pos] = str[s_pos];
			n_pos--;
			s_pos++;
		}
	}
	lua_pushstring(Ls, new_str);
	free(new_str);
	return 1;
}

void print_char_bits(char c){
	for (int b=7; b>=0; b--) {
		if ( c & ( 1 << b ) ) printf("1"); else printf("0");
	}
}

void print_string_bytes(const char *s){
	int len=strlen(s);
	for (int n=0; n<len; n++) {
		printf("s[%i]: ", n);
		print_char_bits(s[n]);
		printf("- %c\n", s[n]);
	}
}

int has_utf8_bytes(const char *s, int len){
			/* Keep compiler silent since we
			 * don't have any validations on
			 * place yet: */
	len=len;
			/* We have a multibyte character? */
	if ( s[0] & U8BNEXT ) {
			/* Check the lenth of the multibyte character: */
		if ( (s[0] & U8B1_2M) == U8B1_2 ) return 2;
		if ( (s[0] & U8B1_3M) == U8B1_3 ) return 3;
		if ( (s[0] & U8B1_4M) == U8B1_4 ) return 4;
	}
	return 0;
}

/*   Check:
 *   + man 2 stat for stat usage and struct stat
 *
 *   Links:
 *   + http://luajit.org/ext_c_api.html
 *   + http://pgl.yoyo.org/luai/i/about
 *   + https://en.wikipedia.org/wiki/UTF-8
 *
 *   Compilation:
 *   Check ./compile.sh
 *
 *   Notes:
 *   Tested on 64 bit Gentoo Linux
 *
 *   (C) Antti Antinoja, 2019
 */

