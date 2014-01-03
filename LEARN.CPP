#include<stdio.h>
#include<dos.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream.h>
#include<math.h>

void load_init();
void load_w();
void save_w();
void input(int);
void output(int);
void dai();
void calc_x();
void calc_y();
void calc_w();
void calc_dw();
void calc_p();

#define	PI	3.141592654f
#define	UMax	6
#define	TsMax	100
#define	TpMax	100
//#define	DEBUG

double	x[UMax];
double	y[UMax][TsMax+1];
double	bak_y[UMax];
double	w[UMax][UMax];	//w[i][j] is weight of unit from i to j.
double	dw[UMax][UMax];
double	p[UMax][TsMax];
double	X[UMax];
double	Y[UMax][TsMax];
char	D[UMax][TpMax];

int	Unit = 5;
double	Eta = 0.001;
double	Tau = 3.0;
int	Ts = 16, ts;
char	stream[256];
char	w_name[30] = "untitled.wdt";

void main()
{
	load_init();
	load_w();

	int	t = 0, c;
	ts = Ts - random(Ts)/2;
	while ( (c = getchar()) != EOF )
	{
		if ( ungetc( c, stdin ) == EOF )
		{
			cerr << "error:ungetc( c, stdin )\n" << endl;
			exit(0);
		}
		input(t);
		if ( ts == t )
		{

			calc_p();

			calc_dw();

			calc_w();
			ts = Ts - random(Ts)/2;
			t = 0;
			dai();
		}
		output(t);
		t++;
	}
	save_w();
}

void input(int t)
{
	int		i, a;
	char	*p = new char[100];

	gets( stream );

	for ( i = 0; i < Unit; i++ )
	{
		a = sscanf( stream, "%s", p );
		X[i] = atof( p );
		strcpy( stream, stream+strlen( p ) );
		if ( stream[0] == ' ' )
			strcpy( stream, stream+1 );
	}
	for ( i = 0; i < Unit; i++ )
	{
		a = sscanf( stream, "%s", p );
		y[i][t] = atof( p );
		strcpy( stream, stream+strlen( p ) );
		if ( stream[0] == ' ' )
			strcpy( stream, stream+1 );
	}
	for ( i = 0; i < Unit; i++ )
	{
		a = sscanf( stream, "%s", p );
		if ( a == EOF )
			p[0] = '*';
		switch ( p[0] )
		{
			case '*':
				D[i][t] = 'F';
				break;
			default:
				D[i][t] = 'T';
				Y[i][t] = atof( p );
		}
		strcpy( stream, stream+strlen( p ) );
		if ( stream[0] == ' ' )
			strcpy( stream, stream+1 );
	}
}

void output(int t)
{
	int i;
	for ( i = 0; i < Unit; i++ )
		cout << X[i] << " ";
	for ( i = 0; i < Unit; i++ )
		cout << y[i][t] << " ";
	for ( i = 0; i < Unit; i++ )
		cout << Y[i][t] << " ";
	cout << endl;
}

void calc_p()
{
	double	sum, dum;

	for ( int i = 0; i < Unit; i++)
			p[i][ts-1] = 0.0f;
	for ( int t = ts-1; t > 0; t-- )
	{
		for ( int i = 0; i < Unit; i++)
		{
			dum = y[i][t] * ( 1.0 - y[i][t] );
			sum = 0.0f;
			for ( int j = 0; j < Unit; j++)
				sum += p[j][t] * w[i][j];
			p[i][t-1] = (1.0f-1.0f/Tau)*p[i][t]+sum*dum;
			if ( D[i][t-1] == 'T' )
				p[i][t-1] += ( y[i][t] - Y[i][t]) * dum;
		}
	}
}

void calc_dw()
{
	//int	sum;

	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
		{
			dw[i][j] = 0.0f;
			for ( int t = 0; t < ts; t++ )
				dw[i][j] += p[j][t]*y[i][t];
		}
}

void calc_w()
{
	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
			w[i][j] -= Eta * dw[i][j];
}

void dai()
{
	for ( int i = 0; i < Unit; i++ )
		y[i][0] = y[i][ts];
}

void load_init()
{
	FILE	*fp;
	//double	dum;

	// Read "rnn.ini" file
	if ( (fp = fopen( "rnn.ini", "rt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't open (rnn.ini) Initialize file.\n" );
		exit(0);
	}
	fscanf( fp, "Newron:%d\n", &Unit );
	fscanf( fp, "Tau:%lf\n", &Tau );
	fscanf( fp, "Eta:%lf\n", &Eta );
	fscanf( fp, "Weight Name:%s\n", w_name );
	fscanf( fp, "Sampling Time:%d\n", &Ts );
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

void save_w()
{
	FILE	*fp;

	if (( fp = fopen( w_name, "w" )) == NULL )
	{
		fprintf( stderr,"File open error: %s\n", w_name);
		exit(0);
	}
	fprintf( fp, "Unit:%d\n", Unit);
	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
			fprintf(fp, "w[%d][%d]=%lf\n", i, j, w[i][j] );
	fclose(fp);
}

int matherr (struct exception *a)
{
	cerr << "Math Error!" << endl;
	exit(0);
}
