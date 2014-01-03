#include<owl\window.h>
#include <stdio.h>
#include<stdlib.h>

int OwlMain(int _argc, char* _argv[])
{
	char	dum[10];
	int		i;
	for ( int j = 0; j < 10; j++ )
	{
		i = rand() % 100;
		itoa(i,dum,10);
		MessageBox(NULL,dum,"rand(2)",MB_OK);
	}
}

