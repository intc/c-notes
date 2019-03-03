#include <stdio.h>
#include <quadmath.h>
#include <string.h>
#define F128_SZ 64

int main(void) {

	printf("Size of __float128: %lu bits\n", sizeof(__float128) * 8);

	__float128 f[3];

	f[0] = 2345678999999999999999999999999999.0Q;             /* Note Q-suffix! */
	f[1] = 2345678999999999999999980000000010.5Q;
	f[2] = f[0] - f[1];

	char str[3][F128_SZ];
	size_t p = 0;

	for ( unsigned n = 0; n < 3; n++ ) {
		quadmath_snprintf(str[n], F128_SZ, "%.1Qf", f[n]);
		size_t t = strlen(str[n]); if ( p < t ) p = t;
	}
	
	char frmt[100];
	snprintf(frmt, sizeof(frmt), " %%%lus\n-%%%lus\n\n=%%%lus\n", p, p, p);
	printf(frmt, str[0], str[1], str[2]);
	
	return 0;

}

/*   Links
 *   + https://gcc.gnu.org/onlinedocs/gcc/Floating-Types.html
 *
 *   Notes
 *   - On Gentoo libquadmath requires USE="fortran" (for gcc).
 *
 *   (C) Antti Antinoja, 2019
 */
