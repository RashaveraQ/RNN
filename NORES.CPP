#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream.h>

#define	PI	3.1415926535

void main(void)
{
	int		i, t1;
	double	a;

	for ( i = 0; i < 1093; i++ ) {
		a = sin(5*PI*i/1092);
		if ( a < 0 ) a = -a;
		cout << "Y[0][1][" << i << "]=" << a << endl;
	}
}
