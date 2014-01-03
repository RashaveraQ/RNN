#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <graphics.h>

/********************************
 *      function prototype      *
 ********************************/
void    load_points(void);
void    load_weight(void);
void    weight_init(void);
void    calc(void);
void    help(void);
void	menu(void);
void	option(void);

/************************
 *      define MACRO    *
 ************************/
#define U_INP  20
#define U_MID  30
#define U_OUT  20
#define PTRN   20
#define SIGMO(X) ( 1.0 / ( 1.0 + exp ( X ) ) )

/********************************
 *      grobal arguments        *
 ********************************/
char	*p_name = "untitled.pdt",
		*w_name = "untitled.wdt";
int     u_inp, u_mid, u_out,
		ptrn,
		Color = 1,
		seed = 1;
double	wji[U_MID][U_INP], wkj[U_OUT][U_MID],
		bak[U_OUT][U_MID],
		out_i[U_INP], out_j[U_MID], out_k[U_OUT],
		X[PTRN][U_INP],	Y[PTRN][U_OUT];
/****************
 *      main    *
 ****************/
void main()
{
	if ( _argc == 1 )
		menu();
	else
		option();
	int gdriver = DETECT, gmode, errorcode;
	initgraph(&gdriver, &gmode, "c:\\bc4\\bgi");
	errorcode = graphresult();
	if (errorcode != grOk)  /* エラーが発生したか？ */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1); /* エラーとして終了 */
	}
	for ( ; ; )
	{
		load_points();
		for ( int p = 0; p < ptrn; p++ )
			circle( 640*X[p][0], 400*(1.0-Y[p][0]), 3 );
		load_weight();
		calc();
		if ( kbhit() )
			switch ( getch() )
			{
				case 'c':
					cleardevice();
					break;
				case 'q':
					goto next;
			}
	}
	next:
	closegraph();
}

/************************
 *      calculate       *
 ************************/
void calc()
{
	int x = 0;
	Color += Color + ( Color > 15 ) ? 14 : 0;
	for ( double l = 0; l < 1; l += 1.0/640.0 )
	{
		for ( int i = 0; i < u_inp; i++ )
			out_i[i] = 1;
		out_i[0] = l;
		for ( int j = 0; j < u_mid; j++ )
		{
			double sum = 0.0;
			for ( int i = 0; i < u_inp; i++ )
				sum += wji[j][i] * out_i[i];
			out_j[j] = SIGMO( -sum );
		}
		for ( int k = 0; k < u_out; k++ ) {
			double sum = 0.0;
			for ( int j = 0; j < u_mid; j++ )
				sum += wkj[k][j] * out_j[j];
			out_k[k] = SIGMO( -sum );
		}
		putpixel( x++, 400.0*(1.0-out_k[0]), Color);
	}
}
/************************/
/*	load weight	*/
/************************/
void load_weight()
{
	FILE	*fp;
	int		d;

	if (( fp = fopen( w_name, "r")) == NULL ) {
		weight_init();
		return;
	}
	fscanf(fp,"Unit:(%d,%d,%d)\n",&u_inp,&u_mid,&u_out);
	for ( int i = 0; i < u_inp; i++ )
		for ( int j = 0; j < u_mid; j++ )
			fscanf(fp, "w[%d][%d]=%lf\n",&d,&d,&wji[j][i]);
	for ( int j = 0; j < u_mid; j++ )
		for ( int k = 0; k < u_out; k++ )
			fscanf(fp, "w[%d][%d]=%lf\n",&d,&d,&wkj[k][j]);
	fclose(fp);
}

/************************
 *      load points     *
 ************************/
void load_points()
{
	FILE    *fp;
	char	*dum;

	if ( ( fp = fopen(p_name, "r") ) == NULL ) {
		printf("File Open Error!\n%s\n",p_name);
		exit(-1);
	}

	fscanf(fp,"Unit:(%d,%d,%d)\n",&u_inp,&u_mid,&u_out);
	fscanf(fp,"Patern:%d\n", &ptrn);
	/*
	int p = 0, xory = 0, i = 0;
	do {
		do {
			fgets( dum, 128, fp );
		} while ( *dum == '/' );
		switch ( xory )
		{
			case 0:	// x
				X[p][i] = atof( dum );
				i++;
				if ( i == u_inp )
					i = 0, xory = 1;
				break;
			case 1:	// y
				Y[p][i] = atof( dum );
				i++;
				if ( i == u_out )
				{
					i = xory = 0, p++;
					if ( p == ptrn ) goto next;
				}
		}
	} while ( 1 );
	next:
	if ( xory == 0 )
		X[
	*/
	for ( int p = 0; p < ptrn; p++ )
	{
		fscanf( fp, "X(" );
		for ( int i = 0; i < u_inp; i++ )
			fscanf( fp, " %lf", &X[p][i] );
		fscanf( fp, "), Y(" );
		for ( int k = 0; k < u_out; k++ )
			fscanf( fp, " %lf", &Y[p][k] );
		fscanf( fp, ")\n" );
	}
	fclose(fp);
}
/************************/
/*	weight init	*/
/************************/
void weight_init()
{
	int     i, j, k;

	srand(seed);
	for ( i = 0; i < u_inp; i++ )
		for ( j = 0; j < u_mid; j++ )
			wji[j][i] = 2.0*(double)rand()/RAND_MAX-1.0;

	for ( j = 0; j < u_mid; j++ )
		for ( k = 0; k < u_out; k++ )
			wkj[k][j] = 2.0*(double)rand()/RAND_MAX-1.0;
}

void option()
{
	char	*dum;

	for ( int i = 1; i < _argc; i++ )
	{
		if ( _argv[i][0] == '-' || _argv[i][0] == '/' )
		{
			switch( _argv[i][1] )
			{
				case 's':
				case 'S':
					seed = atoi( _argv[i] + 2 );
					break;
				default:
					fprintf( stderr,"Bad Option!\n");
				case '?':
					help();
					exit(0);
			}
		}
		else
		{
			dum = _argv[i];
			while( *(dum++) != '.' );
			switch( *dum )
			{
				case 'w':
				case 'W':
					w_name = _argv[i];
					break;
				case 'p':
				case 'P':
					p_name = _argv[i];
					break;
				default:
					fprintf( stderr, "Bad filename! (%s)\n", _argv[i] );
					exit(0);
			}
		}
	}
}

/****************
 *      help    *
 ****************/
void help()
{
	printf("パターンを学習し結合荷重のデータをファイルに落とします。\n\n");
	printf("\t結合荷重のデータファイル[untitled.wdt]\n");
	printf("\tパターンのデータファイル[untitled.pdt]\n");
	printf("オプション\n");
	printf("\t/e\t学習係数η[0.50]\n");
	printf("\t/s\tｓｅｅｄ値[1]\n");
	printf("\t/l\t学習回数[100]\n");
	printf("\t/g\tGraphics\n");
	printf("\t/?\tこのメッセージの出力\n");
}
void menu()
{
	printf("階層型ニューラルネットワークの学習プログラム\n");
	char	c;
	do {
		printf("パターンのデータファイル[untitled.pdt]? ");
		scanf("%s", p_name );
		printf("%s\n", *p_name);
		printf("結合荷重のデータファイル[untitled.wdt]? ");
		scanf("%s", w_name);
		printf("%s\n", *w_name);
		printf("seed値[1]? " );
		scanf("%d", &seed );
		printf("seed=%d\n", seed );
		printf("よろしいですか（ｙ／ｎ）？");
		c = getche();
	} while ( c != 'y' && c != 'Y' );
}

