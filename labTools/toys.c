/*
** TOYS.C
**
** utilities for paradigms
*/

/* 3/16/02: JD added sort and median 
** 6/ 4/13: TD created toy_exp2() from toy_exp to sample a duration directly from a truncated exp distribution.
** 6/20/13: TD created toy_rt_to_x and toy_rt_to_y
*/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include "toys.h"
#include "safe_alloc.h"

/* PUBLIC ROUTINE: toy_atan
**
*/
int toy_atan(int x, int y)
{
	double val;

	if(!x && !y) return(0);

	val = 180.0/PI*atan2((double)y, (double)x);

	if(val < 0) val+= 360.;

	return((int) (val + 0.5));
}

/* PUBLIC ROUTINE: toy_mag
**
*/
int toy_mag(int x, int y)
{
	return((int) (sqrt(pow((double) x, 2.) + pow((double) y, 2.))));
}

/* PUBLIC ROUTINE: toy_exp
**
*/
long toy_exp(long min, long max, long mean)
{
	long ret;
	double tmp = ((double) rand()) / ((double) RAND_MAX);

	ret = min + (long) ((double) mean * -1 * log(tmp));

	if(ret > max)
		ret = max;

	return(ret);
}

/* PUBLIC ROUTINE: toy_exp2 (created by TD on 2013-06-04)
**
**   Unlike toy_exp, toy_exp2() directly samples a value from a truncated exp distribution,
**   so that the sample distribution does not have a "spike" at the upper end.
**   as before, mean refers to the mean of the untruncated exp distribution (so it's not the sample mean)
*/
long toy_exp2(long min, long max, long mean)
{
   long   ret;
   double tmp;
	double interv = (double) (max - min);
   double lambda = 1/(double)mean;
	double r = ((double) rand()) / ((double) RAND_MAX);

   tmp = r * ( 1-exp(-lambda*interv));
   ret = (long)( -log(1-tmp)/lambda );
   ret = ret + min; 

	return(ret);
}

/* PUBLIC ROUTINE: toy_unique
**
**	finds the unique members of an array
*/
int toy_unique(int num_in, int *array_in, int *array_out)
{
	register int i, j, keep, num_out=0;

	for(i=num_in;i>0;i--, array_in++) {
		for(keep=1,j=0;j<num_out;j++)
			keep *= (*array_in != array_out[j]);
		if(keep)
			array_out[num_out++] = *array_in;
	}
	return(num_out);
}

/* PUBLIC ROUTINE: toy_sort
**
** sorts a float array
** (just a straight insertion sort, quite slow, not for large arrays!!!)
**
** vec is a pointer to the float array
** vec_len is the number of elements in the array
*/
void toy_sort(float* vec,int vec_len){
	int i,j;
	float a;

	if (vec_len>1){
		for (j=1;j<vec_len;j++){
			a=vec[j];
			i=j-1;
		
			while(i>=0 && vec[i]>a){
				vec[i+1]=vec[i];
				i--;
			};

			vec[i+1]=a;
		};
	};
};

/* PUBLIC ROUTINE: toy_median
**
** returns the median of a float array
**
** vec is a pointer to the float array
** vec_len is the number of elements in the array
** (returns 0 if vec_len < 1)
*/
float toy_median(float* vec,int vec_len){
	float* fp;
	int i;
	float ret_val;

	if (vec_len<1) 
		return 0;

	if (vec_len==1)
		return *vec;

	if (vec_len==2)
		return (vec[0]+vec[1])/2;

	fp=SAFE_ZALLOC(float,vec_len); /* allocate a copy of the vector */
	
	for (i=0;i<vec_len;i++)
		fp[i]=vec[i]; /* make a copy of the vector (We do not want to sort the original vector.) */	

	toy_sort(fp,vec_len); /* sort vector */ 

	if ((vec_len/2)*2!=vec_len) /* odd length? */
		ret_val=fp[(vec_len-1)/2];
	else /* even length */
		ret_val=(fp[vec_len/2-1]+fp[vec_len/2])/2;

	SAFE_FREE(fp); /* release memory */
	return ret_val;
};

/* PUBLIC ROUTINE: toy_seriesi
**
** returns a series of "num" integer values
**		starting at "start" incremented by "step"
*/
int *toy_seriesi(int num, int start, int step)
{
	register int i,val;
	int *vals;

	if(num < 1)
		return(NULL);

	vals = SAFE_ZALLOC(int, num);		

	for(i=0,val=start;i<num;i++,val+=step)
		vals[i] = val;

	return(vals);
}

/* PUBLIC ROUTINE: toy_seriesf
**
** returns a series of "num" float values
**		starting at "start" incremented by "step"
*/
float *toy_seriesf(float num, float start, float step)
{
	register int i;
	float *vals, val;

	if(num < 1)
		return(NULL);

	vals = SAFE_ZALLOC(float, num);		

	for(i=0,val=start;i<num;i++,val+=step)
		vals[i] = val;

	return(vals);
}

/* PUBLIC ROUTINE: toy_boundi
**
*/
int toy_boundi(int num, int min, int max)
{
	if(num < min)
		num = min;
	else if(num > max)
		num = max;

	return(num);
}

/* PUBLIC ROUTINE: toy_boundf
**
*/
float toy_boundf(float num, float min, float max)
{
	if(num < min)
		num = min;
	else if(num > max)
		num = max;

	return(num);
}

/* PUBLIC ROUTINE: toy_bessi0
**
** from Numeric Recipes in C, 1988
** pp. 190-191
**
*/
double toy_bessi0(double x)
{
   double ax, ans;
   double y;

   if ((ax=fabs(x)) < 3.75) {
      y = x/3.75;
      y*=y;
      ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
         +y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
   } else {
      y=3.75/ax;
      ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
         +y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
         +y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
         +y*0.392377e-2))))))));
   }
   return(ans);
}

/* PUBLIC ROUTINE: toy_vonMises
**
*/
double toy_vonMises(double x, double mu, double k)
{
   return( exp(k*cos((x-mu)*PI/180.))/(2.*PI*toy_bessi0(k)) );
}

/* PUBLIC ROUTINE: toy_vonMisesA
**
** 'A' for Array
**    allocates memory for return array
*/
double *toy_vonMisesA(double *x, int num_x, double mu, double k)
{
   double *prob, bes;
   register int i;

   prob = SAFE_ZALLOC(double, num_x);
   bes  = 2.*PI*toy_bessi0(k);

   for (i=0;i<num_x;i++)
      prob[i] = exp(k*cos((x[i]-mu)*PI/180))/bes;

   return(prob);
}

/* PUBLIC ROUTINE: toy_vonMisesRand
**
** Generate a random number from a von Mises distribution
** 1979 Best-Fisher Algorithm
**
*/
double toy_randCircularNormal(double mu, double sigma)
{
   double X,Y,sample;
   
   do {
      X = RANDV;
      Y = RANDV;
   } while(pow(X,2.0)+pow(Y,2.0)>=1);

   if(RANDV<0.5)
      X = -X;

   sample = X*pow((-2*log(pow(X,2.0)+pow(Y,2.0))/(pow(X,2.0)+pow(Y,2.0))),0.5);
   sample = fmod(sample * sigma+ mu, 360.0);
}

double toy_double(void)
{
	return(101.3345);
}
float toy_float(void)
{
	return(101.3345);
}
long toy_long(void)
{
	return(1013345);
}

/* PUBLIC ROUTINE: toy_rt_to_x and toy_rt_to_y
**  The public macro TOY_RT_TO_X uses cast to int type, producing a bias and residual as much as 0.9999.
**  For example, TOY_RT_TO_X returns -99 for 180 degree but +100 for 0 degree 
**  with the same amplitude 100.
**  toy_rt_to_x round off an input after taking abs() to avoid this bias. Also,
**  the floor is taken after 0.5 is added.
*/
int toy_rt_to_x(int x, int r, int t)
{
  double d; 
  int    i; 
 
  d = (double)(x) + ((double)(r)*cos(DEG2RAD*(double)(t)));
  i = (int) ( floor(fabs(d) + 0.5) * TOY_SIGN(d) );
  return(i);
}
    
int toy_rt_to_y(int x, int r, int t)
{
  double d; 
  int    i; 
 
  d = (double)(x) + ((double)(r)*sin(DEG2RAD*(double)(t)));
  i = (int) ( floor(fabs(d) + 0.5) * TOY_SIGN(d) );
  return(i);
}
