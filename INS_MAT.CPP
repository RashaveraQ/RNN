#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <STRING.h>
#include <iostream.h>

void main(void)
{
	char	*dum1, *dum2;
    int	count = 0;
	FILE	*fp;

	if ( _argc != 2 )
	{
		fprintf( stderr, "Bad Option!\naddstr filename\n" );
		exit(0);
	}
	if ( (fp = fopen( _argv[1], "rt" )) == NULL )
	{
		fprintf( stderr, "File Open Error!\n");
		exit(0);
	}
	while ( !feof( fp ) )
	{
		fscanf( fp, "%s\n", dum2 );
		cout << "X[0][0][" << count++ << "]=" << dum2 << endl;
	}
}
