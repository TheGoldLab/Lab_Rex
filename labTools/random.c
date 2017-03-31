/* Random number generating code from: Bratley, P., B.L. Fox,
 * and L.E. Schrage. A guide to simulation. Springer-Verlag:
 * New York, NY, 1983. p.209.  Two functions involved: mcunif()
 * seeds the random # generator, ivunif() generates numbers.
 */

#define MODULUS ((1L << 31) - 1L)
#define SIGN_BIT (1L << 31)

long mcunif(int seed)
{
  static long x=3;

  if(seed > 0) x=2*seed + 1;
  x *= 29903947L;
  x &= ~SIGN_BIT;

  return(x);
}

/* range = 0-n, inclusive */
int ivunif(int seed, int n)
{
  register int jtry;
  long mcunif(), x;
  long i, j, k, l, iji, ijr, kli, klr;

  x = mcunif(seed);
  n++;
  jtry = x/(MODULUS/n);
  if(jtry <= 0) return(0);
  for(;;jtry--) {
    i = MODULUS;
    j = x;
    k = n;
    l = jtry;
    for(;;) {
      iji = i/j;
      kli = k/l;
      if(iji < kli) return(jtry);
      if(iji > kli) break;
      ijr = i%j;
      if(ijr <= 0) return(jtry);
      klr = k%l;
      if(klr <= 0) break;
      if(ijr <= 0) return(jtry);
      klr = k%l;
      if(klr <= 0) break;
      i = l;
      k = j;
      j = klr;
      l = ijr;
    }
  }
}
