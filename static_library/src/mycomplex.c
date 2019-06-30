#include "../include/mycomplex.h"
int main()
{
	double x1,y1,x2,y2;
	int n;
	complex_z ad,s,m,d;
	while(1)
	{
		printf("1 - addition\n");
		printf("2 - subtraction\n");
		printf("3 - multiplication\n");
		printf("4 - division\n");
		printf("Exit - any key \n");
		printf("Enter coordinates x and y of value a and b:");
		scanf("%lf %lf %lf %lf", &x1,&y1,&x2,&y2);
		complex_z a = {x1,y1}; // the first complex number a with coordinates x and y
		complex_z b = {x2,y2}; // the second complex number b with coordinates x and y
		printf("Enter menu of item: ");
		scanf("%d",&n);
		switch(n)
		{
			case 1:
				ad = add(a,b);
				printf ("addition       = (%5.2lf, %5.2lf)\n", ad.x, ad.y ); 
				break;
			case 2:
				s = sub(a,b);
				printf ("subtraction    = (%5.2lf, %5.2lf)\n", s.x, s.y ); 
				break;
			case 3:
				m = mul(a,b);
				printf ("multiplication = (%5.2lf, %5.2lf)\n", m.x, m.y ); 
				break;
			case 4:
				d = division(a,b);
				printf ("division       = (%5.2lf, %5.2lf)\n", d.x, d.y );
				break;
			default: 
				return 0;
				break;
		}
	}
	return 0;
}