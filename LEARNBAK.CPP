#include<stdio.h>
#include<dos.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

void load_init();
void load_w();
void save_w();
void input(int);
void dai();
void calc_x();
void calc_y();
void calc_w();
void calc_dw();
void calc_p();
char *get_tango(char*);

#define	PI	3.141592654
#define	UMax	10
#define	TsMax	200
#define	TpMax	200
#define	PMax	10

double	x[UMax];
double	y[UMax][TsMax+1];
double	bak_y[UMax];
double	w[UMax][UMax];	//w[i][j] is weight of unit from i to j.
double	dw[UMax][UMax];
double	p[UMax][TsMax];
double	X[UMax][TsMax];
double	Y[UMax][TsMax];
int	D[UMax][TpMax];

char	*w_name = "untitled.wdt";
int	Unit = 5;
double	Eta = 0.001;
double	Tau = 3.0;
int	Ts = 16, ts;

void main()
{
	load_init();
	load_w();

	int	t = 0, c;
	ts = Ts - random(Ts)/2;
	while ( (c = getchar()) != EOF )
	{
		ungetc( c, stdin );
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
		t++;
	}
	save_w();
}

void input(int t)
{
	char	stream[256], *duf;

	gets( stream );
	puts( stream );

	int	i;
	for ( i = 0; i < Unit; i++ )
		get_tango( stream );
	for ( i = 0; i < Unit; i++ )
		y[i][t] = atof( get_tango( stream ) );
	for ( i = 0; i < Unit; i++ )
	{
		duf = get_tango( stream );
		switch ( duf[0] )
		{
			case '*':
				D[i][t] = 0;
				break;
			default:
				D[i][t] = 1.0;
				Y[i][t] = atof( duf );
		}
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

void calc_p()
{
	double	sum, dum;

	for ( int i = 0; i < Unit; i++)
			p[i][ts-1] = 0.0;
	for ( int t = ts-1; t > 0; t-- )
	{
		for ( int i = 0; i < Unit; i++)
		{
			dum = y[i][t] * ( 1.0 - y[i][t] );
			sum = 0.0;
			for ( int j = 0; j < Unit; j++)
				sum += p[j][t] * w[i][j];
			p[i][t-1] = (1.0-1.0/Tau)*p[i][t]+sum*dum;
			if ( D[i][t-1] )
				p[i][t-1] += ( y[i][t] - Y[i][t]) * dum;
		}
	}
}

void calc_dw()
{
	int	sum;

	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
		{
			dw[i][j] = 0.0;
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
    double	dum;

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

