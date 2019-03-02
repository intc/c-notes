#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#define MAX_ARGS 2 + 1

#ifndef _BNAME
#define _BNAME "ppointer_addr"
#endif

int n_free(void **pp);
int chr_wipe_n_free(char **pp, int size_char);
int buf_alloc(char **bufu, int size_char);

int main(int argc, char *argv[]) {
			/* We agree to initialize allocatable pointers to NULL */
	char* a = NULL;
	char* b = NULL;

	int ab_size_char = sizeof(char) * 100;

	if ( argc != MAX_ARGS ) {
		fprintf(stderr,"Usage: %s [arg1] [arg2]\n", _BNAME);
		return 1;
	}

	buf_alloc(&a, ab_size_char);
	buf_alloc(&b, ab_size_char);

	printf("argv located at: 0x%lX\n\n", (uintptr_t) argv);
	for( int n = 0; n < argc; n++ ){
			/* %-22s ->   %-22: 22 chars wide, left aligned
			 * %#.18lX -> %#  : use alternative presentation ('#' adds 0x),
			 *            .18 : 18 chars wide (zero padded),
			 *            lX  : long usigned int in heX (ABCDEF) */
		printf("argv[%u]: %-22s @ %#.18lX\n", n, argv[n], (uintptr_t) &argv[n]);
	}
	printf("\n");

			/* &a will give pointer to the pointer */
	chr_wipe_n_free(&a, ab_size_char);

	printf("  b @ 0x%.18lX - before free\n", (uintptr_t) b );
	chr_wipe_n_free(&b, ab_size_char);
	printf("  b @ 0x%.18lX - after free\n", (uintptr_t) b );

	printf("\ntesting double free:\n");
	n_free( (void*) &b );
	
	return 0;
}

/* int n_free(void *pp, int debug):
 * In order to be able to set the freed pointer to NULL we need to use a pointer to pointer variable */
int n_free(void **pp) {
			/* Have to check the pointer to pointer first: */
	if ( pp == NULL ) {
		fprintf(stderr,
				"ERROR! pp == NULL!\n");
		return 1;
	}
			/* Now we can check the passed pointer: */
	if ( *pp == NULL ) {
		fprintf(stderr,
				"ERROR: *pp == NULL! (Double free attempt?)\n");
		return 1;
	}
#ifdef _DEBUG
			/* See p. 140 / C a Reference Manual (intptr_t) */
		fprintf(stderr,
				" pp @ 0x%.18lX - pointer to pointer\n",
				(uintptr_t) pp );
		fprintf(stderr,
				"*pp @ 0x%.18lX - before free & setting to null\n",
				(uintptr_t) *pp );
#endif
	free(*pp);
	*pp=NULL;
#ifdef _DEBUG
		fprintf(stderr,
				"*pp @ 0x%.18lX - after free & setting to null\n\n",
				(uintptr_t) *pp );
#endif
	return 0;
}

int chr_wipe_n_free(char **pp, int size_char){
	if ( pp == NULL ) {
		printf("chr ERROR! pp == NULL!\n"); return 1;
	}
	if ( *pp == NULL ) {
		printf("chr WARNING: *pp == NULL! (Double free attempt?)\n"); return 1;
	}
			/* This will write over all the allocated RAM before freeing it.
			 * ... Unless some optimisation will ride it over? */
	memset(*pp, 0xFF, size_char);
	n_free( (void*) pp);
	return 0;
}

int buf_alloc(char** bufu, int size_char){
			/* Pointer to pointer needs to point to non NULL address */
	if ( bufu == NULL ) {
		printf( "No bufu. No joy!\n" );
		return 1;
	}
			/* This is our "client" pointer which should have been set
			 * to NULL (by our own agreement ;) */
	if ( *bufu == NULL ) {
		*bufu = malloc(size_char); assert(*bufu != NULL );
	} else {
		printf( "Bufu has value. Something fishy? Check your code!\n" );
		assert(0);
	}
	return 0;
}

/*   Links:
 *
 *   + http://www.crasseux.com/books/ctutorial/argc-and-argv.html
 *   + https://github.com/angrave/SystemProgramming/wiki/C-Programming,-Part-3:-Common-Gotchas
 *   + Asserttions: https://blog.regehr.org/archives/1091
 *
 *   Notes:
 *
 *   (C) Antti Antinoja, 2019
 */

