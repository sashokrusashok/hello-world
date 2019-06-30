#include <stdio.h>
int main()
{

	int a[5],*a1;
	char s[5]={1,2,3,4,5},*s1;
	s1 = s;
	a1 = (int*)s;
	printf("%d",*((char*)a1 + 1));
}