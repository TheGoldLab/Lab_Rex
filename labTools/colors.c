/* COLORS.C
**
**	A collection of c-routines for using colors
**		via dotsX
**
**	Revision history:
**		6/28/06 created by jig
*/

/* INCLUDED files */
#include <stdio.h>
#include "colors.h"
#include "toys.h"

/* PRIVATE DATA STRUCTURES */
typedef struct _COinit_struct *_COinit;

struct _COinit_struct {
	int	r;
	int	g;
	int	b;
	int	lum;
};

/* PRIVATE VARIABLES */

/* 
** COLOR REGISTRY 
*/
#define MAXC 	255
#define MAXL	1000

static struct _COinit_struct gl_colors[] = {
	{ 		0, 		0,			0,		MAXL	},		/* 0 	*/
	{ 	MAXC, 	MAXC,		MAXC,		MAXL	},
	{ 	MAXC, 		0,			0,		MAXL	},
	{ 		0, 	MAXC,			0,		MAXL	},
	{ 		0, 		0,		MAXC,		MAXL	},
	{ 	MAXC, 	MAXC,			0,		MAXL	},		/*	5	*/
	{ 	MAXC, 		0,		MAXC,		MAXL	},
	{ 		0, 	MAXC,		MAXC,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},		/*	10	*/
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},		/*	15	*/
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},		/*	20	*/
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},		/*	25	*/
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},
	{ 		0, 		0,			0,		MAXL	},		/*	30	*/
	{ 		0, 		0,			0,		MAXL	}};

/* PUBLIC ROUTINES */

/* PUBLIC ROUTINE: co_init
**
**	Initializes _PLlist holding the color 
**		look-up table.
**
**	Requires gl_colors global, defined above.
*/
_PLlist co_init(void)
{
	register int i;
	int num_colors = sizeof(gl_colors)/sizeof(struct _COinit_struct);
	_PLlist colors = pl_list_init("Colors", 0, num_colors);

	/* add four properties: r,g,b,lum */
#define CO_NP 4
	colors->properties_length = CO_NP;
	colors->properties 		  = SAFE_ZALLOC(_PLproperty, CO_NP);

	colors->properties[0] = pl_prop_init("r",0,num_colors,1.0);
	colors->properties[1] = pl_prop_init("g",0,num_colors,1.0);
	colors->properties[2] = pl_prop_init("b",0,num_colors,1.0);
	colors->properties[3] = pl_prop_init("luminance",0,num_colors,1.0);

	/* fill-in the default values */
	for(i=0;i<num_colors;i++)
		co_set(colors, i, 
			gl_colors[i].r,
			gl_colors[i].g,
			gl_colors[i].b,
			gl_colors[i].lum);
		

	return(colors);
}

/* PUBLIC ROUTINE: co_set
**
**	Set r,g,b and lum for a given color index
**
*/
void co_set(_PLlist colors, int index, 
	int r, int g, int b, int lum)
{
	CO_SET_R	( colors, index, r	);
	CO_SET_G	( colors, index, g	);
	CO_SET_B	( colors, index, b	);
	CO_SET_L	( colors, index, lum	);
}

/* PUBLIC ROUTINE: co_parse
**
**	Creates (appends) a string corresponding
**		to the [r,b,b] triplet for the given
**		color index.
** 
**	Arguments:
**		buf		... already allocated string
**		colors 	... the _PLlist of color defs
**		index		... index into the _PLlist values
*/
void co_parse(char *buf, _PLlist colors, int index)
{
	double mult = ((double) (CO_GET_L(colors,index)))/1000.0;

	sprintf(buf, "%s[%d,%d,%d],", buf,
		(int) (CO_GET_R(colors,index)*mult),
		(int) (CO_GET_G(colors,index)*mult),
		(int) (CO_GET_B(colors,index)*mult));
}

