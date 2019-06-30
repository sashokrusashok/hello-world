#include "../include/mycomplex.h"
complex_z add(complex_z a, complex_z b) //(a1+a2i)+(b1+b2i)= a1+b1+(a2+b2)i
{
	complex_z z;
	z.x = a.x + b.x;
	z.y = a.y + b.y;
	return z;
}