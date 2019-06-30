#include "../include/mycomplex.h"
complex_z sub(complex_z a, complex_z b) // a1-b1 - a2-b2
{
	complex_z z;
	z.x = a.x - b.x;
	z.y = a.y - b.y;
	return z;
}
