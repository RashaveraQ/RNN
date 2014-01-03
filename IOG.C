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
int     dmax();
void    getinput();
double  rate(short,short,short,short);

/************************
 *      define MACRO    *
 ************************/
#define U_INP  15
#define U_MID  20
#define U_OUT  10
#define PTRN   15
#define SIGMO(X) ( 1.0 / ( 1.0 + exp ( X ) ) )

#define LX    20
#define LY    20
#define	CC     1

/********************************
 *      grobal arguments        *
 ********************************/
int     u_inp,
	u_mid,
	u_out,
	X0, Y0;
double  wji[U_MID][U_INP],
	wkj[U_OUT][U_MID],
	out_i[U_INP],
	out_j[U_MID],
	out_k[U_OUT],
	X[U_INP];
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
	for ( int i = 0; i < 350 / ( LY * 5 ); i++ ) {
	X0 = 0;
	for ( int j = 0; j < 640 / ( LX * 3 ); j++ ) {
		setcolor(3); rectangle(X0, Y0, X0 + LX * 3, Y0 + LY * 5);
		getinput();
		gotoxy((j+1) * LX*3 / 8,(i+1) * LY*5 / 16);
			printf("%d",dmax());
			while ( 1 ) {
		M_getstat(&ms);
			if ( ms.r )
					break;
			}
			while ( ms.r )
		M_getstat(&ms);
				X0 += LX * 3;
		}
		Y0 += LY * 5;
	}
}

int dmax()
{
	int   i, x1, y1, u_max = 0;
	double max = 0.0;

	for ( i = 0; i < 15; i++ ) {
	x1 = X0 + LX * ( i % 3 );
	y1 = Y0 + LY * ( i / 3 );
	out_i[i] = rate( x1, y1, x1 + LX, y1 + LY);
	max = ( max < out_i[i] ) ? out_i[i] : max;
    }
    if ( max )
        for ( i = 0; i < 15; i++ )
	    out_i[i] /= max;
    calc();
    for ( i = 0; i < u_out; i++) {
	if ( max < out_k[i] )
	    max = out_k[i], u_max = i;
    }
    return u_max;
}

/************************
 *      calculate       *
 ************************/
void calc()
{
    int     i, j, k, p;
    double  sum;

 	for ( j = 0; j < u_mid; j++ ) {
	    sum = 0.0;
	    for ( i = 0; i < u_inp; i++ )
	        sum += wji[j][i] * out_i[i];
	    out_j[j] = SIGMO( -sum );
    	}
	for ( k = 0; k < u_out; k++ ) {
	    sum = 0.0;
	    for ( j = 0; j < u_mid; j++ )
		sum += wkj[k][j] * out_j[j];
	    out_k[k] = SIGMO( -sum );
    	}
}
/************************/
/*	load weight	*/
/************************/
void load_weight()
{
	FILE	*fp;
	int     i, j, k, d;

	if (( fp = fopen("number.wdt","r")) == NULL ) {
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
	while(-1) {
	M_getstat(&ms);
	i -= ( i >= 0 )? 1 : 0;
	if (ms.l && ms.x == 0 && ms.y > 350 )
		exit(0);
	if (ms.l && i < 0) {
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

