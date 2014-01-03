#include<stdio.h>
#include<dos.h>
#include<conio.h>
#include<stdlib.h>
#include<math.h>
#include<graphics.h>

void load_p();
void load_w();
void graph();
void setXY();
void calc_y();

#define	PI		3.141592654
#define	UMax	10
#define	TpMax	40
#define	PMax	5

double	x[UMax], y[UMax], by[UMax], w[UMax][UMax],
		X[UMax], Y[UMax],
		XX[PMax][UMax][TpMax], YY[PMax][UMax][TpMax],
		Xmin = 0.0, Xmax = 1.0, Ymin = 0.0, Ymax = 1.0;

int		Unit, Pattern, Tp[TpMax], Tau = 50.0, Xunit = 0, Yunit = 1;
char	*w_name = "untitled.wdt",
		*p_name = "untitled.pdt";

void main()
{
	char *dum;

	for ( int i = 1; i < _argc; i++ )
	{
		dum = _argv[i];
		while( *(dum++) != '.' );
			switch ( *dum )
			{
				case 'w':
				case 'W':
					w_name = _argv[i];
					break;
				case 'p':
				case 'P':
					p_name = _argv[i];
			}
	}
	load_p();
	load_w();
	int gdriver = DETECT, gmode, errorcode;
	initgraph(&gdriver, &gmode, "a:\\bc\\bgi");
	/* 初期化の結果を取得 */
	errorcode = graphresult();

	if (errorcode != grOk)  /* エラーが発生したか？ */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1);             /* エラーとして終了 */
	}
	double gx, gy;
	int t, tt, p;
	char	c;
	while ( (c = getch()) != 'q' )
	{
		if ( c == 'c' )
		{
			cleardevice();
		}
		else
		{
			p = atoi( (const char * )c );
			for ( t = 0; t < Tp[p]; t++ )
			{
				for ( tt = 0; tt < Tau; tt++ )
				{
					for ( i = 0; i < Unit; i++ )
						X[i] = XX[p][i][t];
					calc_y();
					gx = (double)XX[p][0][t]*320.0+320.0;
					for ( i = 0; i < Unit; i++ )
					{
						//Ymax = ( Ymax < y[i] ) ? Ymax : y[i];
						//Ymin = ( Ymin > y[i] ) ? Ymin : y[i];
						gy = 400.0-400.0*(y[i]-Ymin)/( Ymax - Ymin );
						putpixel(gx, gy, (t % 7)+1);
					}
				}
			}
			circle( gx, gy, 10);
		}
	}
	closegraph();
}

void calc_y()
{
	double	sum;

	for ( int i = 0; i < Unit; i++ )
		by[i] = y[i];
	for ( i = 0; i < Unit; i++ )
	{
		sum = 0.0;
		for ( int j = 0; j < Unit; j++ )
			sum += w[j][i] * by[j];
		x[i] = (1.0 - 1.0/Tau)*x[i]+sum+(1.0/Tau)*X[i];
		y[i] = 1.0/(1.0+exp(-x[i]));
	}
}

void load_p()
{
	FILE	*fp;

	//	Pattern data
	if (( fp = fopen( p_name, "r" )) == NULL )
	{
		fprintf(stderr, "File open error(pattern): %s\n", p_name);
		exit(0);
	}
	fscanf( fp, "Pattern:%d\n", &Pattern );
	for ( int p = 0; p < Pattern; p++ )
	{
		fscanf( fp, "Tp:%d\n", &Tp[p] );
		for ( int t = 0; t < Tp[p]; t++ )
			for ( int i = 0; i < Unit; i++ )
				XX[p][i][t] = YY[p][i][t] = 0.0;
		int	a, b;
		fscanf( fp, "UnitXY(%d,%d)\n", &a , &b);
		for ( t = 0; t < Tp[p]; t++ )
		{
			for ( int i = 0; i < a+b; i++ )
			{
				if ( i < a )
				{
					fscanf( fp, " %lf", &XX[p][i][t] );
				}
				else
				{
					fscanf( fp, " %lf", &YY[p][i][t] );
				}
			}
			fscanf( fp, "\n" );
		}
	}
	fclose(fp);
}

void load_w()
{
	FILE	*fp;
	int	a, b;
	double	dum;

	if (( fp = fopen( w_name, "r" )) == NULL )
	{
		fprintf(stderr,"File open error(weight): %s\n",w_name);
		return;
	}
	fscanf(fp, "Unit:%d\n", &Unit);
	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
		{
			fscanf(fp, "w[%d][%d]=%lf\n", &a, &b, &dum );
			w[a][b] = dum;
		}
	fclose(fp);
}

