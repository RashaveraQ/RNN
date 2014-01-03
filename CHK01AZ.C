#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <graphics.h>
#include <mouse.h>

/********************************
 *      function prototype      *
 ********************************/
void    load_weight();
void    calc();
char	dmax();
void    getinput();
double  rate(short,short,short,short);

/************************
 *      define MACRO    *
 ************************/
#define U_INP  14*14
#define U_MID  27
#define U_OUT  6
#define PTRN   37
#define SIGMO(X) ( 1.0 / ( 1.0 + exp ( X ) ) )

#define LX    8
#define LY    10
#define	CC     1

/********************************
 *      grobal arguments        *
 ********************************/
char	c[]="0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ????????????????????????????????????????????????????????????";

int	u_inp = U_INP,
	u_mid = U_MID,
	u_out = U_OUT,
	X0, Y0;
double	far	wji[U_MID][U_INP];
double	far	wkj[U_OUT][U_MID];
double	far	out_i[U_INP];
double	far	out_j[U_MID];
double	far	out_k[U_OUT];
double	far	X[U_INP];
struct mstat ms;

/****************
 *      main    *
 ****************/
void main()
{
	int gdriver = DETECT, gmode, errorcode;
	initgraph(&gdriver, &gmode, "a:\\bc\\bgi");
	errorcode = graphresult();
	if (errorcode != grOk)  /* エラーが発生したか？ */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1); /* エラーとして終了 */
	}
	if (M_checkenv() == 0)
	{
		puts("cannot use mouse");
		return;
	}
	load_weight();
	for ( int i = 0; i < 350 / ( LY * 14 ); i++ )
	{
		X0 = 0;
		for ( int j = 0; j < 640 / ( LX * 14 ); j++ )
		{
			setcolor(3); rectangle(X0, Y0, X0 + LX * 14, Y0 + LY * 14);
			getinput();
			gotoxy((j+1) * LX*14 / 8,(i+1) * LY*14 / 16);
			printf("%c",dmax());
			while ( 1 )
			{
				M_getstat(&ms);
				if ( ms.r )
					break;
			}
			while ( ms.r )
				M_getstat(&ms);
			X0 += LX * 14;
		}
		Y0 += LY * 14;
	}
}

char dmax()
{
	int   i, x1, y1;

	for ( i = 0; i < 14*14; i++ )
	{
		x1 = X0 + LX * ( i % 14 );
		y1 = Y0 + LY * ( i / 14 );
		out_i[i] = (rate( x1, y1, x1 + LX, y1 + LY) > 0.1) ? 1.0 : 0.0;
		//max = ( max < out_i[i] ) ? out_i[i] : max;
	}
	/*
	if ( max )
		for ( i = 0; i < 14*14; i++ )
			out_i[i] /= max;
    */
	/*
	for ( int y = 0; y < 14; y++ )
		for ( int x = 0; x < 14; x++ )
		{
			gotoxy(5*x,y);
			printf("%lf",out_i[x+y*14]);
		}
	*/
	calc();
	int	ans = 0;
	for ( i = 0; i < u_out; i++)
		if ( out_k[i] > 0.5 )
			ans += pow(2,i);
	return c[ans];
}

/************************
 *      calculate       *
 ************************/
void calc()
{
	int     i, j, k, p;
	double  sum;

	for ( j = 0; j < u_mid; j++ )
	{
		sum = 0.0;
		for ( i = 0; i < u_inp; i++ )
			sum += wji[j][i] * out_i[i];
		out_j[j] = SIGMO( -sum );
	}
	for ( k = 0; k < u_out; k++ )
	{
		sum = 0.0;
		for ( j = 0; j < u_mid; j++ )
			sum += wkj[k][j] * out_j[j];
		out_k[k] = SIGMO( -sum );
	}
}
/************************/
/*	load weight			*/
/************************/
void load_weight()
{
	FILE	*fp;
	int     i, j, k, d;

	if (( fp = fopen("letter.wdt","r")) == NULL )
	{
		fprintf(stderr, "Can't Open number.wdt!\n");
		exit(-1);
	}
	fscanf(fp,"Unit:(%d,%d,%d)\n",&u_inp,&u_mid,&u_out);
	for ( i = 0; i < u_inp; i++ )
		for ( j = 0; j < u_mid; j++ )
			fscanf(fp, "w[%d][%d]=%lf\n",&d,&d,&wji[j][i]);
	for ( j = 0; j < u_mid; j++ )
		for ( k = 0; k < u_out; k++ )
			fscanf(fp, "w[%d][%d]=%lf\n",&d,&d,&wkj[k][j]);
	fclose(fp);
}

void getinput()
{
	int         i = -1;

	setcolor(CC);
	M_dispcursor();
	while(-1)
	{
		M_getstat(&ms);
		i -= ( i >= 0 )? 1 : 0;
		if (ms.l && ms.x == 0 && ms.y > 350 )
			exit(0);
		if (ms.l && i < 0)
		{
			i = 500;
			M_erasecursor();
			setfillstyle(1,CC);fillellipse(ms.x, ms.y, 4, 4);
			M_dispcursor();
		}
		if (ms.r)
			break;
	}
	M_erasecursor();
}

double rate(short x1, short y1, short x2, short y2)
{
	int         x, y, a = 0, b;
	double      answer;

	b = LX * LY;
	for ( y = y1; y <= y2; y++ )
		for ( x = x1; x <= x2; x++ )
			a += ( getpixel( x, y) == CC ) ? 1: 0;
	answer = (double)a / (double)b;

	return answer;
}

