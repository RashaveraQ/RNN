#include <dos.h>
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
void	getinput();
void    DrawPara();

/************************
 *      define MACRO    *
 ************************/
#define U_INP  14*14
#define U_MID  27
#define U_OUT  6
#define PTRN   37
#define SIGMO(X) ( 1.0 / ( 1.0 + exp ( X ) ) )

/********************************
 *      grobal arguments        *
 ********************************/
char	c[]="0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ????????????????????????????????????????????????????????????";
int		u_inp = U_INP,
		u_mid = U_MID,
		u_out = U_OUT;
double  wji[U_MID][U_INP], wkj[U_OUT][U_MID],
		out_i[U_INP], out_j[U_MID], out_k[U_OUT],
		X[U_INP];
mstat	ms;

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
	for ( int i = 0; i < 14; i++ )
		printf("・・・・・・・・・・・・・・\n");
	gotoxy(1,17);
	printf("   0     1     2     3      4     5");
	puts("\x1b[>5h");

	if (M_checkenv() == 0)
	{
		puts("cannot use mouse\n");
		return;
	}
	M_dispcursor();
	load_weight();
	getinput();
	M_erasecursor();
	puts("\x1b[>5l");
}

void DrawPara()
{
	calc();
	M_erasecursor(); cleardevice(); M_dispcursor();
	setcolor(1); setfillstyle(SOLID_FILL,2);
	int	ans = 0;
	for ( int i = 0; i < u_out; i++)
	{
		bar3d(50*i, 400-out_k[i]*300, 50*i+40, 400,10,1);
		if ( out_k[i] > 0.5 )
			ans += pow(2,i);
	}
	setcolor(7); gotoxy(1,15); printf("It is '%c'",c[ans]);
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
/*	load weight	*/
/************************/
void load_weight()
{
	FILE	*fp;
	int     i, j, k, d;

	if (( fp = fopen("letter.wdt", "r")) == NULL )
	{
		fprintf(stderr, "Can't Open weight.dat!\n");
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
	int i;
	short x,y;

	while(-1)
	{
		M_getstat(&ms);
		if (ms.l)
		{
			x = (short)(ms.x / 16), y = (short)(ms.y / 16);
			if ( x < 14 && y < 14 )
			{
				out_i[x + 14 * y] = 1.0 - out_i[x + 14 * y];
				if ( out_i[x + 14 * y] > 0.1 )
				{
					gotoxy( 2*x+1, y+1);
					printf("■");
				}
				else
				{
					gotoxy( 2*x+1, y+1);
					printf("・");
				}
				while(ms.l)
					M_getstat(&ms);
			}
			DrawPara();
		}
		if (ms.r) return;
	}
}
