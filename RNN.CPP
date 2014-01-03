#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream.h>
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
float	Tau = 3.0f;
float	x[UMax];
float	nx[UMax];
float	X[UMax];
float  by[UMax];
float  y[UMax];
float	w[UMax][UMax];	//w[i][j] is weight of unit from i to j.
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
	int		i, a;
	char	p[128];

	gets( stream );

	for ( i = 0; i < Unit; i++ )
		X[i] = 0.0;

	for ( i = 0; i < Unit; i++ )
	{
		a = sscanf( stream, "%s", p );
		if ( a == 0 || a == EOF )
			break;
		if ( p[0] == ';' )
		{
			strcpy( stream, stream+1 );
			while ( stream[0] == ' ' )
				strcpy( stream, stream+1 );
			break;
		}
		X[i] = atof( p );
		strcpy( stream, stream+strlen( p ) );
		while ( stream[0] == ' ' )
			strcpy( stream, stream+1 );
	}
}

char *get_tango(char* s)
{
	char *p;
	int i = 0;

	p = new char[100];
	
	if ( strlen( s ) == 0 )
	{
		p[0] = '\0';
		return p;
	}

	do {
		p[i] = s[i];
	} while ( s[++i] != ' ' );

	p[i] = '\0';
	strcpy( s, s+i+1 );
	return p;
}

void calc()
{
	float	sum;

	for ( int i = 0; i < Unit; i++ )
	{
		sum = 0.0f;
		for ( int j = 0; j < Unit; j++ )
			sum += w[j][i] * by[j];
		nx[i] = (1.0f-1.0f/Tau)*x[i]+sum+(1.0f/Tau)*X[i];
		y[i] = 1.0f/(1.0f+(float)exp((double)-nx[i]));
	}
}

void output()
{
	int i;
	for ( i = 0; i < Unit; i++ )
		cout << X[i] << " ";
	for ( i = 0; i < Unit; i++ )
		cout << y[i] << " ";
	cout << stream << endl;
}

void load_init()
{
	FILE	*fp;
	float	dum;

	// Read "rnn.ini" file
	if ( (fp = fopen( "rnn.ini", "rt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't open (rnn.ini) Initialize file.\n" );
		exit(0);
	}
	fscanf( fp, "Newron:%d\n", &Unit );
	fscanf( fp, "Tau:%e\n", &Tau );
	fscanf( fp, "Eta:%e\n", &dum );
	fscanf( fp, "Weight Name:%s", w_name );
	fclose( fp );
}

void load_w()
{
	FILE	*fp;
	int a, b;
	float	dum;

	if (( fp = fopen( w_name, "r" )) == NULL )
	{
		fprintf( stderr, "File open error(weight): %s\n", w_name);
		return;
	}
	fscanf( fp, "Unit:%d\n", &a);
	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
		{
			if ( fscanf(fp, "w[%d][%d]=%e\n", &a, &b, &dum ) == 3)
				w[a][b] = dum;
		}
	fclose(fp);
}

void load_inside()
{
	FILE	*fp;
	float	dum1,dum2;

	// Read "inside.dat" file
	if ( (fp = fopen( "inside.dat", "rt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't open (inside.dat) file.\n" );
		return;
	}
	for ( int i = 0; i < Unit; i++ )
	{
		fscanf( fp, "x:%e y:%e\n", &dum1, &dum2 );
		x[i] = dum1;
		by[i] = dum2;
	}
	fclose( fp );
}

void save_inside()
{
	FILE	*fp;

	// Save "inside.dat" file
	if ( (fp = fopen( "inside.dat", "wt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't create (inside.dat) file.\n" );
		return;
	}
	for ( int i = 0; i < Unit; i++ )
		fprintf( fp, "x:%e y:%e\n", nx[i], y[i] );
	fclose( fp );
}

