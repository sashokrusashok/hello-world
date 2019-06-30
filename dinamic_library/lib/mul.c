#include "../include/mycomplex.h"
complex_z mul(complex_z a, complex_z b) // a1*b1-a2*b2 + i(a1b2+a2b1)
{
	complex_z z;
	z.x = a.x * b.x - a.y * b.y;
	z.y = a.x * b.y + a.y * b.x;
	return z;	
}
