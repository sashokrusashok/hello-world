#include "../include/mycomplex.h"
complex_z division(complex_z a, complex_z b) // (a1*a2+b1*b2)/(a2*a2+b2*b2)+(a1*b2-b1*a2)/(a2*a2+b2*b2)
{
	complex_z z;
	z.x = (a.x * a.y + b.x * b.y)/(a.y*a.y+b.y*b.y);
	z.y = (a.y * b.x - a.x * b.y)/(a.y*a.y+b.y*b.y);
	return z;
}
