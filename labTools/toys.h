/* Toys.h
*
*/

/*
** TOY_SIGN, toy_rt_to_x, and toy_rt_to_y are added. TD, 20130620 
**
*/

#ifndef _TOYS_H_
#define _TOYS_H_

#include <string.h>
#include <math.h>
#include "safe_alloc.h"

/* PUBLIC CONSTANTS */
#define DEG2RAD	0.0174533
#define PI			3.141592654
#define MET 		1
#define ANYMET		0xff
#define CORRECT	1
#define WRONG		0
#define ONES 		65535
#define ENDC		NULL
#define ENDI		-1

/* PUBLIC MACROS */
#define TOY_RT_TO_XF(x,r,t) (float) ((float)(x) + ((float)(r)*cos(DEG2RAD*(float)(t))))
#define TOY_RT_TO_YF(y,r,t) (float) ((float)(y) + ((float)(r)*sin(DEG2RAD*(float)(t))))
#define TOY_RT_TO_X(x,r,t)	(int) ((double)(x) + ((double)(r)*cos(DEG2RAD*(double)(t))))
#define TOY_RT_TO_Y(y,r,t)	(int) ((double)(y) + ((double)(r)*sin(DEG2RAD*(double)(t))))
#define TOY_ADD_STRING(f1,f2) ((f1) ? (f2) ? (char *) strcat((char *) \
									   SAFE_REALLOC((f1), (strlen(f1) + strlen(f2) + 2)), (f2)) \
									: (f1) : NULL)
#define TOY_CARBONCOPY(s) 	((s) ? strcpy(SAFE_MALLOC(strlen(s)+1),s) : NULL )
#define TOY_BOUND(x,b,t)	(x < b ? b : (x > t : t))
#define TOY_MAX(x,y)			(x>=y?x:y)
#define TOY_MIN(x,y)			(x<=y?x:y)

#define TOY_PCT(c,i)			((c)+(i)?100.*(float)(c)/((float)(i)+(float)(c)):0.)

/* added by TD 20130620 */
#define TOY_SIGN(x)        ( (x>0)-(x<0) )

	/* random integer between 0 and r-1 */
#define TOY_RAND(ra)			((int) ((ra) * rand() / (RAND_MAX+1.0)))

	/* return comparison of r and random # between 0 and 999 */
#define TOY_RCMP(rc)			((rc) >= 0 && TOY_RAND(1000.0) < (int) (rc))

/* PUBLIC ROUTINE PROTOTYPES */

int 	 toy_atan		(int, int);
int 	 toy_mag			(int, int);
long 	 toy_exp			(long, long, long);
long 	 toy_exp2		(long, long, long);
int	 toy_unique		(int, int *, int *);
void   toy_sort 		(float *, int);
float  toy_median 	(float *, int);

int	*toy_seriesi	(int, int, int);
float *toy_seriesf	(float, float, float);

int	 toy_boundi		(int, int, int);
float	 toy_boundf		(float, float, float);

double  toy_bessi0   (double);
double  toy_vonMises (double, double, double);
double *toy_vonMisesA(double *, int, double, double);

double	toy_double(void);
float		toy_float(void);

int toy_rt_to_x(int, int, int);
int toy_rt_to_y(int, int, int);


#endif /* _TOYS_H_ */
