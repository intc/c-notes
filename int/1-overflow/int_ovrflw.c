#include<stdio.h>
#include<stdint.h>

int main(void){

	printf("\"long g=n*1024;\"\n");
	for (int32_t n=2097150; n<2097155; n++){
		long g=n*1024;  /* This will overflow after 2 iterations */
		printf("g: %li n: %i\n", g, n);
	}

	printf("\n\"long g=n*1024L;\"\n");
	for (int32_t n=2097150; n<2097155; n++){
		long g=n*1024L; /* Works as intended */
		printf("g: %li n: %i\n", g, n);
	}

	printf("\nint %lu bits.\n", sizeof(int)*8);
	printf("long int %lu bits.\n", sizeof(long)*8);

	return 0;
}

