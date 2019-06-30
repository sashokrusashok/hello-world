#ifndef mycomplex_h
#define mycomplex_h
#include <stdio.h>
#include <stdlib.h>
typedef struct{
double x,y;
} complex_z;
complex_z sub(complex_z a, complex_z b);
complex_z add(complex_z a, complex_z b);
complex_z division(complex_z a, complex_z b);
complex_z mul(complex_z a, complex_z b);
#endif