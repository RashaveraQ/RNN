#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

int main(void)
{
	FILE	*fp;

	if ( _argc != 2 )
	{
		fprintf( stderr, "error!\n%s filename\n", _argv[0] );
		exit(0);
	}
	if ( (fp = fopen( _argv[1], "rb" )) == NULL )
	{
		fprintf( stderr, "Error!\n");
		return 0;
	}

	signed char	a;
	unsigned	b;
	double		d;

	while ( !feof( fp ) )
	{
		fread( &b, 1, 1, fp );
		fread( &a, 1, 1, fp );
		d = (double) a / 128.0;
		d += (double) b / (256.0*128.0);
		cout << d << endl;
	}
	return 1;
}


