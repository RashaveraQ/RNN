#include<dos.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include<graphics.h>

void load_init();
void save_init();
void draw();
void ginit();
char *get_tango(char*);

char	stream[126];
int		px = 0;

void main()
{
	ginit();
	load_init();

	int c;
	//double data;

	while ( (c = getchar()) != EOF )
	{
		ungetc( c, stdin );
		draw();
		px++;
		if ( px > 639 )
		{
			cleardevice();
			px = 0;
		}
	}
	save_init();
	//closegraph();
}

void draw()
{
	int		i;
	char	*p = new char[100];

	gets( stream );

	for( i = 1; i < _argc; i++ )
	{
		sscanf( stream, "%s", p );
		strcpy( stream, stream+strlen( p ) );
		if ( stream[0] == ' ' )
			strcpy( stream, stream+1 );
		if ( strstr( "1234567" , _argv[i] ) != NULL )
			putpixel( px, 200-200.0*atof( p ), atoi( _argv[i] ));
	}
}
char *get_tango(char* s)
{
	char *p = new char[100];
	int i = 0;

	do {
		p[i] = s[i];
	} while ( s[++i] != ' ' );

	p[i] = '\0';
	strcpy( s, s+i+1 );
	return p;
}

void load_init()
{
	FILE	*fp;

	// Read "grph.ini" file
	if ( (fp = fopen( "grph.ini", "rt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't open (grph.ini) Initialize file.\n" );
		return;
	}
	fscanf( fp, "X_point:%d\n", &px );
	fclose( fp );
}
void save_init()
{
	FILE	*fp;

	// Save "grph.ini" file
	if ( (fp = fopen( "grph.ini", "wt" )) == NULL )
	{
		fprintf( stderr, "Error: Can't create (grph.ini) file.\n" );
		return;
	}
	fprintf( fp, "X_point:%d\n", px );
	fclose( fp );
}

void ginit()
{
	/* 自動検出を要求 */
	int gdriver = DETECT, gmode, errorcode;

	/* グラフィックスシステムの初期化 */
	initgraph(&gdriver, &gmode, "c:\\bc4\bgi");

	/* 初期化の結果を取得 */
	errorcode = graphresult();

	if (errorcode != grOk)  /* エラーが発生したか？ */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1);             /* エラーとして終了 */
	}
}
