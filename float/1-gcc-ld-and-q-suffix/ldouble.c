#include<stdio.h>

int main(void) {

	printf("Size of long double: %lu bits\n", sizeof(long double) * 8);

	long double  a=9999999999999999.0Q;             /* Note Q-suffix! */
	long double  b=9999999999999998.0Q;

	long double   c = a - b;

	printf("%.1LF - %.1LF = %.1LF\n", a, b, c);

	return 0;

}

/*   Links
 *   + https://gcc.gnu.org/onlinedocs/gcc/Floating-Types.html
 *
 *   Notes
 *   -
 *
 *   (C) Antti Antinoja, 2019
 */
