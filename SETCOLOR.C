setcolor( d, c )
Display		*d;
unsigned long	c[];
{
	static char colorname[16][15] = {
		"black",	"slate blue",	"coral",	"blue violet",
		"lime green",	"spring green",	"khaki",	"light gray",
		"white",	"blue",		"red",		"magenta",
		"green",	"cyan",		"yellow",	"orange"
	};
	Colormap	colmap;
	XColor		c0,c1;
	int		i;

	colmap = XDefaultColormap( d, 0 );

	for ( i = 0; i < 16 ; i++ )
	{
		XAllocNamedColor ( d, colmap, colorname[i], &c1, &c0 );
		c[i] = c1.pixel;
	}
}

