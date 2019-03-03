#include<stdio.h>
#include<stdint.h>

int main(void){

	printf("Size of int32_t .: %lu bits\n", sizeof(int32_t)*8);
	printf("Size of int .....: %lu bits\n", sizeof(int)*8);
	printf("Size of long ....: %lu bits\n\n", sizeof(long)*8);

	printf("\"long g=n*1024; /* Undefined behaviour!! */\"\n");
	for (int32_t n=2097150; n<2097155; n++){
		long g=n*1024; /* Undefined behaviour!! */
		printf("g: %li n: %i\n", g, n);
	}

	printf("\n\"long g=n*1024L;\"\n");
	for (int32_t n=2097150; n<2097155; n++){
		long g=n*1024L; /* Works as intended */
		printf("g: %li n: %i\n", g, n);
	}

	printf("\n\"long g=(long)n*1024;\"\n");
	for (int32_t n=2097150; n<2097155; n++){
		long g=(long)n*1024; /* Works as intended */
		printf("g: %li n: %i\n", g, n);
	}

	return 0;
}

/*   Links:
 *   + Fixed-width integers: https://en.wikipedia.org/wiki/C_data_types#Fixed-width_integer_types
 *
 *   Notes:
 *   + tested with gcc 7.3:
 *   ...
 *   "long g=n*1024;"
 *   g: 2147481600 n: 2097150
 *   g: 2147482624 n: 2097151
 *   g: -2147483648 n: 2097152
 *   g: -2147482624 n: 2097153
 *   g: -2147481600 n: 2097154
 *   ...
 *
 *   + tested with gcc 8.2:
 *   ...
 *   "long g=n*1024;"
 *   g: 2147481600 n: 2097150
 *   g: 2147482624 n: 2097151
 *   g: 2147483648 n: 2097152
 *   g: 2147484672 n: 2097153
 *   g: 2147485696 n: 2097154
 *   ...
 *
 *   With current CFLAGS gcc 8.2 actually gives a worning:
 *   int_ovrflw.c:8:11: warning: iteration 2 invokes undefined behavior [-Waggressive-loop-optimizations]
 *
 *   (C) Antti Antinoja, 2019
 */
