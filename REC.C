#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "setcolor.c"

void load_p();
void load_w();
void save_w();
void graph();
void setXY();
void dai();
void calc_x();
void calc_y();
void calc_w();
void calc_dw();
void calc_p();
void help();
void line(int,int,int,int);

#define	UMax	10
#define	TsMax	50
#define	TpMax	50
#define	PMax	10
#define	Width	800.0
#define Height	600.0

double	x[UMax][TsMax+1];
double	y[UMax][TsMax+1];
double	bak_y[UMax];
double	w[UMax][UMax];	/* w[i][j] is weight of unit from i to j.  */
double	dw[UMax][UMax];
double	p[UMax][TsMax];
double	X[UMax][TsMax];
double	Y[UMax][TsMax];
double	XX[PMax][UMax][TpMax];
double	YY[PMax][UMax][TpMax];
int	DD[PMax][UMax][TpMax];
int	D[UMax][TpMax];
int	Tp[PMax];

char	*w_name = "untitled.wdt";
char	*p_name = "untitled.pdt";
int	Unit = 5;
int	Pattern = 0;
double	Eta = 0.001;
double	Tau = 3.0;
long	Loop = 1000;
int	Ts = 16, ts, gstep;

Display		*d;
Window		rw, win;
GC		gc;
XEvent		ev;
XSetWindowAttributes	attr;
unsigned long	col[16];

void main(argc, argv)
int	argc;
char	**argv;
{
	char	*dum;
	int	i, flag = 0;
	long	n;

        d = XOpenDisplay( NULL );
	setcolor( d, &col[0] );
        rw = XDefaultRootWindow( d );
        win = XCreateSimpleWindow( d, rw, 50, 50,
			(unsigned int)Width,
			(unsigned int)Height, 2,
                                WhitePixel( d, 0 ), BlackPixel( d, 0 ) );

	attr.override_redirect = True;
	XChangeWindowAttributes( d, win, CWOverrideRedirect, &attr );

        XMapWindow( d, win );
	XFlush( d );
	printf( "Hit Return key!\n" );
	getchar();
        gc = XCreateGC( d, rw, 0, 0 );
        XSetLineAttributes( d, gc, 1, LineSolid, CapButt, JoinMiter );
/*
	XSelectInput( d, win, ExposureMask );
	while( 1 )
	{
		XNextEvent ( d, &ev );
		if ( ev.type == Expose ) break;
	}
	printf( "Start!!\n" );
*/
	for ( i = 1; i < argc; i++ )
	{
		if ( argv[i][0] == '-' || argv[i][0] == '/' )
		{
			switch( argv[i][1] )
			{
				case 'u':
				case 'U':
					Unit = atoi( argv[i] + 2 );
					break;
				case 'e':
				case 'E':
					Eta = atof( argv[i] + 2 );
					break;
				case 't':
				case 'T':
					Tau = atof( argv[i] + 2 );
					break;
				case 's':
				case 'S':
					Ts = atoi( argv[i] + 2 );
					break;
				case 'l':
				case 'L':
					Loop = atol( argv[i] + 2 );
					break;
				case 'g':
				case 'G':
					flag = 1;
					gstep = atof( argv[i] + 2 );
					break;
				default:
					fprintf( stderr,"Bad Option!\n");
				case 'h':
					help();
					exit(0);
			}
		}
		else
		{
			dum = argv[i];
			while( *(dum++) != '.' );
			switch ( *dum )
			{
				case 'w':
				case 'W':
					w_name = argv[i];
					break;
				case 'p':
				case 'P':
					p_name = argv[i];
					break;
				default:
					fprintf( stderr, "Bad filename! (%s)\n", argv[i] );
					exit(0);
			}
		}
	}

	load_p();
	load_w();
	srand(time(NULL));
	for ( n = 0; n < Loop; n++)
	{
		ts = Ts - Ts * (double)rand() / (double)RAND_MAX;
		setXY();
		calc_y();
		calc_p();
		calc_dw();
		calc_w();
		if ( flag )
			graph();
		dai();
	}
	save_w();
	XFlush( d );
	XFreeGC( d, gc );
	XDestroyWindow( d, win );
	XFlush( d );
	XCloseDisplay( d );
}

void graph()
{
	static int	t0;
	int		t, i;

	if ( t0 >= (int)Width )
	{
		t0 = 0;
	}
	for ( t = 0; t < ts; t++ )
	{
		XSetForeground( d, gc, col[0] );
		XFillRectangle( d, win, gc,	
			(int)gstep*(t+1)+t0,
			(int)0,
			(int)gstep*(t+2)+t0,
			(int)Height);
		XSetForeground( d, gc, col[14] );
		line(	(int)gstep*(t+1)+t0,
			(int)80.0-X[0][t]*80.0,
			(int)gstep*(t+2)+t0,
			(int)80.0-X[0][t+1]*80.0);
		for ( i = 0; i < Unit; i++ )
		{
			if ( D[i][t] )
			{
				XSetForeground( d, gc, col[9] );
				line(	(int)gstep*t+t0,
					(int)Height-Y[i][t]*(Height-160.0),
					(int)gstep*(t+1)+t0,
					(int)Height-Y[i][t+1]*(Height-160.0));
				XSetForeground( d, gc, col[10] );
				line(	(int)gstep*t+t0,
					(int)Height-y[i][t]*(Height-160.0),
					(int)gstep*(t+1)+t0,
					(int)Height-y[i][t+1]*(Height-160.0));
			}
		}
		XFlush( d );
	}
	t0 += ts*gstep;
}

void setXY()
{
	static int lp, lt;
	int	i, t;

	for ( t = 0; t <= ts; t++ )
	{
		lt++;
		if (lt == Tp[lp])
		{
			lp = Pattern * (double)rand() / (double)RAND_MAX;
			lt = 0;
		}
		for ( i = 0; i < Unit; i++ )
		{
			X[i][t] = XX[lp][i][lt];
			Y[i][t] = YY[lp][i][lt];
			D[i][t] = DD[lp][i][lt];
		}
	}
	lt--;
}

void calc_y()
{
	int		t, i, j;
	double	sum;

	for( t = 0; t < ts; t++ )
	{
		for ( i = 0; i < Unit; i++ )
		{
			sum = 0.0;
			for ( j = 0; j < Unit; j++ )
				sum += w[j][i] * y[j][t];
			x[i][t+1] = (1.0-1.0/Tau)*x[i][t]+sum+(1.0/Tau)*X[i][t];
			y[i][t+1] = 1.0/(1.0+exp(-x[i][t+1]));
		}
	}
}

void calc_p()
{
	double	sum, dum;
	int	i, j, t;

	for ( i = 0; i < Unit; i++)
			p[i][ts-1] = 0.0;
	for ( t = ts-1; t > 0; t-- )
	{
		for ( i = 0; i < Unit; i++)
		{
			dum = y[i][t] * ( 1.0 - y[i][t] );
			sum = 0.0;
			for ( j = 0; j < Unit; j++)
				sum += p[j][t] * w[i][j];
			p[i][t-1] = (1.0-1.0/Tau)*p[i][t]+sum*dum;
			if ( D[i][t-1] )
				p[i][t-1] += ( y[i][t] - Y[i][t]) * dum;
		}
	}
}

void calc_dw()
{
	int	i, j, t, sum;

	for ( i = 0; i < Unit; i++ )
		for ( j = 0; j < Unit; j++ )
		{
			dw[i][j] = 0.0;
			for ( t = 0; t < ts; t++ )
				dw[i][j] += p[j][t]*y[i][t];
		}
}

void calc_w()
{
	int	i, j;
	for ( i = 0; i < Unit; i++ )
		for ( j = 0; j < Unit; j++ )
			w[i][j] -= Eta * dw[i][j];
}

void line( x1, y1, x2, y2)
int	x1, y1, x2, y2;
{
	XDrawLine( d, win, gc, x1, y1, x2, y2 );
}

void dai()
{
	int	i;
	for ( i = 0; i < Unit; i++ )
	{
		x[i][0] = x[i][ts];
		y[i][0] = y[i][ts];
	}
}

void load_p()
{
	FILE	*fp;
	char	c, dum[128];
	int	p, i, t;
	double	x;
	int	flag = 1;

	/*	Pattern data	*/
	if (( fp = fopen( p_name, "r" )) == NULL )
	{
		fprintf(stderr,"File open error(pattern): %s\n", p_name);
		exit(0);
	}
	for ( p = 0; p < PMax; p++ )
		for ( i = 0; i < UMax; i++ )
			for ( t = 0; t < TpMax; t++ )
				DD[p][i][t] = XX[p][i][t] = YY[p][i][t] = 0.0;
	do
	{
		fscanf(fp, "%c" , &c);
		switch( c )
		{
			case 'x':
			case 'X':
				fscanf(fp, "[%d][%d][%d]=%lf\n", &p, &i, &t, &x);
				XX[p][i][t] = x;
				Tp[p] = ( Tp[p] > t+1 )? Tp[p]: t+1;
				Pattern = ( Pattern > p+1 ) ? Pattern: p+1;
				break;
			case 'y':
			case 'Y':
				fscanf(fp, "[%d][%d][%d]=%lf\n", &p, &i, &t, &x);
				YY[p][i][t] = x;
				DD[p][i][t] = 1;
				Tp[p] = ( Tp[p] > t+1 )? Tp[p]: t+1;
				Pattern = ( Pattern > p+1 ) ? Pattern: p+1;
				break;
			case 'e':
			case 'E':
				flag = 0;
				break;
			default:
				fgets(dum , 128, fp);
		}
	} while ( flag );
	fclose( fp );
}

void load_w()
{
	FILE	*fp;
	int	a, b, i, j;
	double	dum;

	if (( fp = fopen( w_name, "r" )) == NULL )
	{
		fprintf( stderr, "New File (weight): %s\n", w_name);
		return;
	}
	fscanf( fp, "Unit:%d\n", &a);
	for ( i = 0; i < Unit; i++ )
		for ( j = 0; j < Unit; j++ )
		{
			if ( fscanf(fp, "w[%d][%d]=%lf\n", &a, &b, &dum ) == 3)
				w[a][b] = dum;
		}
	fclose(fp);
}

void save_w()
{
	FILE	*fp;
	int	i, j;

	if (( fp = fopen( w_name, "w" )) == NULL )
	{
		fprintf( stderr,"File open error: %s\n", w_name);
		exit(0);
	}
	fprintf( fp, "Unit:%d\n", Unit);
	for ( i = 0; i < Unit; i++ )
		for ( j = 0; j < Unit; j++ )
			fprintf(fp, "w[%d][%d]=%lf\n", i, j, w[i][j] );
	fclose(fp);
}

void help()
{
	printf("ＲＮＮの学習を行なう。\n");
	printf("REC [option...] [p_name] [w_name]\n");
	printf("option:\n");
	printf(" /u ユニット数[%d]\n",Unit);
	printf(" /e	学習係数η[%lf]\n",Eta);
	printf(" /t 一次遅れ係数τ[%lf]\n",Tau);
	printf(" /s サンプリングタイム[%d]\n",Ts);
	printf(" /l 学習回数[%ld]\n",Loop);
	printf(" /g 波形表示[No]と単位幅[%d]\n",gstep);
	printf(" /h ヘルプの表示\n");
	printf("p_name:\n");
	printf(" パターンデータのファイル名[%s]\n", p_name);
	printf("w_name:\n");
	printf(" 結合荷重のファイル名[%s]\n", w_name);
}

