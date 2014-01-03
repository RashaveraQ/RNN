#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <STRING.h>
#include <iostream.h>

void main(void)
{
	char	*dum1, *dum2;

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
		cout << dum2 << " 1 ; * * " << gets( dum1 ) << endl;
	}
}
