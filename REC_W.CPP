#include <owl\owlpch.h>
#include <owl\applicat.h>
#include<stdio.h>
#include<dos.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>

/************************
 *      define MACRO    *
 ************************/
#define	PI	3.141592654
#define	UMax	10
#define	TsMax	3000
#define	TpMax	3000
#define	PMax	10
#define LEFT_MAX	50
#define RIGHT_MAX	980
#define TOP_MAX		0
#define BOTTOM_MAX	405

//#define	TRUE	0
//#define	FALSE	1

/********************************
 *      function prototype      *
 ********************************/
int		load_init(char *,char *);
int		load_p(char *);
int		load_w(char *);
int		save_w(char *);
void    weight_init(void);
void	graph(HDC);
void	setXY(int);
void	dai();
void	calc_x();
void	calc_y();
void	calc_w();
void	calc_dw();
void	calc_p();
void	Sread();

/********************************
 *      grobal arguments        *
 ********************************/
double	x[UMax][TsMax+1];
double	y[UMax][TsMax+1];
double	bak_y[UMax];
double	w[UMax][UMax];	//w[i][j] is weight of unit from i to j.
double	dw[UMax][UMax];
double	p[UMax][TsMax];
double	X[UMax][TsMax];
double	Y[UMax][TsMax];
double	XX[PMax][UMax][TpMax];
double	YY[PMax][UMax][TpMax];
int		DD[PMax][UMax][TpMax];
int		D[UMax][TpMax];
int		Tp[PMax];

int		Unit = 5;
int		Pattern = 0;
double	Eta = 0.001;
double	Tau = 3.0;
long	Loop = 1000;
int		Ts = 16, ts, gstep = 5, flag;
int lp, lt, t0;
RECT	rect;
HBRUSH	brush;
//int		t0, lp, lt;

CRITICAL_SECTION gCriticalSection;

int OwlMain(int _argc, char* _argv[])
{
	InitializeCriticalSection(&gCriticalSection);
	EnterCriticalSection(&gCriticalSection);
	Sread();
	LeaveCriticalSection(&gCriticalSection);
	randomize();

	return 0;
}

void Sread()
{
	char    p_name[100],
			w_name[100];

	HDC		hdc;
	hdc = GetDC(0);
	brush = CreateSolidBrush(RGB(255,255,255));

	if ( load_init(p_name,w_name) == FALSE )
		return ;
	if ( load_p(p_name) == FALSE )
		return ;
	if ( load_w(w_name) == FALSE )
		return ;
	srand(time(NULL));
	rect.top = TOP_MAX;
	rect.bottom = BOTTOM_MAX;
	rect.left = LEFT_MAX;

	for ( ; ; )
	{
		ts = Ts - (rand() % Ts) /2;
		setXY(Pattern);
		calc_y();
		calc_p();
		calc_dw();
		calc_w();
		graph(hdc);
		dai();
		/*if ( save_w(w_name) == FALSE )
		{
			MessageBox(NULL, "Sread() save_w() error!!!", "Error!!!", MB_OK);
			return ;
		}
		*/
	}
}

void setXY(int Pattern)
{
	for ( int t = 0; t <= ts; t++ )
	{
		lt++;
		if (lt >= Tp[lp])
		{
			lp = rand() % (Pattern+1) ;
			lt = 0;
		}
		for ( int i = 0; i < Unit; i++ )
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
	int		t, i;
	double	sum;

	for( t = 0; t < ts; t++ )
	{
		for ( i = 0; i < Unit; i++ )
		{
			sum = 0.0;
			for ( int j = 0; j < Unit; j++ )
				sum += w[j][i] * y[j][t];
			x[i][t+1] = (1.0-1.0/Tau)*x[i][t]+sum+(1.0/Tau)*X[i][t];
			y[i][t+1] = 1.0/(1.0+exp(-x[i][t+1]));
		}
	}
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

void graph(HDC hdc)
{
	int t, i;

	for ( t = 0; t < ts; t++ )
	{
		if ( rect.right >= RIGHT_MAX)
		{
			rect.left = LEFT_MAX;
			rect.right = gstep;
		}
		rect.left += gstep;
		rect.right = rect.left + gstep;

		FillRect(hdc,&rect,brush);
		MoveToEx(hdc,rect.left,40-X[0][t]*40,NULL) ;
		LineTo(hdc,rect.right,40-X[0][t+1]*40);
		for ( i = 0; i < Unit; i++ )
		{
			if ( D[i][t] )
			{
				MoveToEx(hdc,rect.left,400-Y[i][t]*320,NULL);
				LineTo(hdc,rect.right,400-Y[i][t+1]*320);
				MoveToEx(hdc,rect.left,400-y[i][t]*320,NULL);
				LineTo(hdc,rect.right,400-y[i][t+1]*320);
			}
		}
	}
}

void dai()
{
	for ( int i = 0; i < Unit; i++ )
	{
		x[i][0] = x[i][ts];
		y[i][0] = y[i][ts];
	}
}

int load_p(char *p_name)
{
	FILE	*fp;
	char	c, dum[200];
	int		p, i, t;
	double	x;

	if (( fp = fopen( p_name, "r" )) == NULL )
	{
		MessageBox(NULL, "load_p() file open error.", "Error!!!", MB_OK) ;
		return FALSE ;
	}
	for ( p = 0; p < PMax; p++ )
		for ( i = 0; i < UMax; i++ )
			for ( t = 0; t < TpMax; t++ )
				DD[p][i][t] = XX[p][i][t] = YY[p][i][t] = 0.0;
	int flag = 1;
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

				Pattern = ( Pattern > p ) ? Pattern: p;
				break;
			case 'y':
			case 'Y':
				fscanf(fp, "[%d][%d][%d]=%lf\n", &p, &i, &t, &x);
				YY[p][i][t] = x;
				DD[p][i][t] = 1;
				Tp[p] = ( Tp[p] > t+1 )? Tp[p]: t+1;
				Pattern = ( Pattern > p ) ? Pattern: p;
				break;
			case 'e':
			case 'E':
				flag = 0;
			default:
				fgets(dum , 128, fp);
		}
	} while ( flag );
	if ( fclose( fp ) == EOF )
	{
		MessageBox(NULL, "load_p() fclose error.", "Error!!!", MB_OK) ;
		return FALSE ;
	}
	return TRUE;
}

int load_w(char *w_name)
{
	FILE	*fp;
	int a, b;
	double	dum;

	if (( fp = fopen( w_name, "r" )) == NULL )
	{
		weight_init();
		return TRUE ;
	}
	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
		{
			if ( fscanf(fp, "w[%d][%d]=%lf\n", &a, &b, &dum ) == 3)
				w[a][b] = dum;
		}
	if ( fclose(fp) == EOF )
	{
		MessageBox(NULL, "load_w() fclose error.", "Error!!!", MB_OK) ;
		return FALSE ;
	}

	return TRUE ;
}

int save_w(char *w_name)
{
	FILE	*fp;

	if (( fp = fopen( w_name, "w" )) == NULL )
	{
		MessageBox(NULL,"save_w() file open error.","Error!!!", MB_OK);
		return FALSE;
	}
	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
			fprintf(fp, "w[%d][%d]=%lf\n", i, j, w[i][j] );
	if ( fclose(fp) == EOF )
	{
		MessageBox(NULL, "save_w() fclose error.", "Error!!!", MB_OK) ;
		return FALSE ;
	}

	return TRUE ;
}
/************************
 *      load init       *
 ************************/
int load_init(char *p_name, char *w_name)
{
	FILE    *fp;

	while ( ( fp = fopen("rec_init.txt", "rt") ) == NULL ) {
		switch ( MessageBox(NULL,"rec_init.txt","File Open Error!", MB_RETRYCANCEL))
		{
			case IDCANCEL:
				return FALSE;
		}
	}
	fscanf(fp,"%s\n",p_name);
	fscanf(fp,"%s\n",w_name);
	fscanf(fp,"Unit:%d\n",&Unit);
	fscanf(fp,"Eta:%lf\n",&Eta);
	fscanf(fp,"Tau:%lf\n",&Tau);
	fscanf(fp,"Ts:%d\n",&Ts);
	fscanf(fp,"Graph:%d\n",&flag);
	fscanf(fp,"Gstep:%d\n",&gstep);
	if ( fclose(fp) == EOF )
	{
		MessageBox(NULL, "load_init() fclose error.", "Error!!!", MB_OK) ;
		return FALSE ;
	}
	return TRUE;
}
/************************/
/*	weight init	*/
/************************/
void weight_init()
{
	srand(time(NULL));
	for ( int i = 0; i < Unit; i++ )
		for ( int j = 0; j < Unit; j++ )
				w[i][j] = 2.0*(double)rand()/RAND_MAX-1.0;
}
/*
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
	printf(" /? ヘルプの表示\n");
	printf("p_name:\n");
	printf(" パターンデータのファイル名[%s]\n", p_name);
	printf("w_name:\n");
	printf(" 結合荷重のファイル名[%s]\n", w_name);
}
*/
