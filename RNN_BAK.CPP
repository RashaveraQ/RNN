#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

void load_init();
void load_w();
void load_inside();
void input();
void calc();
void save_inside();
void output();
char *get_tango(char*);

#define	UMax	10

int		Unit = 5;
double	Tau = 3.0;
double	x[UMax];
double	nx[UMax];
double	X[UMax];
double  by[UMax];
double  y[UMax];
double	w[UMax][UMax];	//w[i][j] is weight of unit from i to j.
char	*w_name = "untitled.wdt";
char	stream[126];

void main()
{
	int	c;

	load_init();
	load_w();
	while ( (c = getchar()) != EOF )
	{
		ungetc( c, stdin );
		load_inside();
		input();
		calc();
		save_inside();
		output();
	}
}

void input()
{
	int i = 0, p = 0;

	for ( i = 0; i < Unit; i++ )
		X[i] = 0.0;
	gets( stream );



	i = 0, p = 0;

	for(;;)
	{
		switch ( *(stream + p) )
		{
			case ',':
				*( stream + p ) = '\n';
				X[i] = atof( stream );
				i++;
				strcpy( stream, stream + p+1 );
				p = -1;
				break;
			case ';':
				*( stream + p ) = '\n';
				if ( p )
					X[i] = atof( stream ),
					i++;
				strcpy( stream, stream + p+1 );
			case '\n':
				return;
		}
		p++;
	}
}
char *get_tango(char* s)
{
	char *p;
	int i = 0;

	do {
		p[i] = s[i];
	} while ( s[++i] != ' ' );

	p[i] = '\0';
	strcpy( s, s+i+1 );
	return p;
}

void calc()
{
	double	sum;

	for ( int i = 0; i < Unit; i++ )
	{
		sum = 0.0;
		for ( int j = 0; j < Unit; j++ )
			sum += w[j][i] * by[j];
		nx[i] = (1.0-1.0/Tau)*x[i]+sum+(1.0/Tau)*X[i];
		y[i] = 1.0/(1.0+exp(-nx[i]));
	}
}

void output()
{
	int i;
	for ( i = 0; i < Unit; i++ )
		printf( "%lf ", X[i] );
	for ( i = 0; i < Unit; i++ )
		printf( "%lf ", y[i] );
	printf( ";%s\n", stream );
}

void load_init()
{
	FILE	*fp;
    double	dum;

	// Read "rnn.ini" file
	if ( (fp = fopen( "rnn.ini", "rt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't open (rnn.ini) Initialize file.\n" );
		exit(0);
	}
	fscanf( fp, "Newron:%d\n", &Unit );
	fscanf( fp, "Tau:%lf\n", &Tau );
	fscanf( fp, "Eta:%lf\n", &dum );
	fscanf( fp, "Weight Name:%s\n", w_name );
	fclose( fp );
}

void load_w()
{
	FILE	*fp;
	int a, b;
	double	dum;

	if (( fp = fopen( w_name, "r" )) == NULL )
	{
		fprintf( stderr, "File open error(weight): %s\n", w_name);
		return;
	}
	fscanf( fp, "Unit:%d\n", NULL);
	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
		{
			if ( fscanf(fp, "w[%d][%d]=%lf\n", &a, &b, &dum ) == 3)
				w[a][b] = dum;
		}
	fclose(fp);
}

void load_inside()
{
	FILE	*fp;
	double	dum1,dum2;

	// Read "inside.dat" file
	if ( (fp = fopen( "inside.dat", "rt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't open (inside.dat) file.\n" );
		return;
	}
	for ( int i = 0; i < Unit; i++ )
	{
		fscanf( fp, "x:%lf y:%lf\n", &dum1, &dum2 );
		x[i] = dum1;
		by[i] = dum2;
	}
	fclose( fp );
}

void save_inside()
{
	FILE	*fp;
	double	dum1,dum2;

	// Save "inside.dat" file
	if ( (fp = fopen( "inside.dat", "wt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't create (inside.dat) file.\n" );
		return;
	}
	for ( int i = 0; i < Unit; i++ )
		fprintf( fp, "x:%lf y:%lf\n", nx[i], y[i] );
	fclose( fp );
}

