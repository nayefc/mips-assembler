#ifndef __HASH_FUNCTION_H
#define __HASH_FUNCTION_H

/*
-------------------------------------------------------------------
lookup.c, by Bob Jenkins, 1996
hash() and mix() are the only externally useful functions in here.
Routines to test the hash are included if SELF_TEST is defined.
--------------------------------------------------------------------
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
typedef  unsigned       char ub1;

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)

static ub4 hash(register ub1 *k, register ub4 length, register ub4 level);

/*
--------------------------------------------------------------------
mix -- mix 4 32-bit values reversibly.
Changing any input bit will usually change at least 32 output bits,
  whether the hash is run forward or in reverse.
Changing any input bit will change each of the 32 output bits in d
  about half the time when inputs a,b,c,d are uniformly distributed.
mix() takes 32 machine cycles.  No 16 or 24 cycle mixer works; this
  was confirmed by brute-force search.  This hash is the best of
  about 500,000 32-cycle hashes examined.
--------------------------------------------------------------------
*/

#define mix(a,b,c,d) \
{ \
  a += d; d += a; a ^= (a>>7);  \
  b += a; a += b; b ^= (b<<13);  \
  c += b; b += c; c ^= (c>>17); \
  d += c; c += d; d ^= (d<<9); \
  a += d; d += a; a ^= (a>>3);  \
  b += a; a += b; b ^= (b<<7);  \
  c += b; b += c; c ^= (c>>15); \
  d += c; c += d; d ^= (d<<11); \
}

/*
--------------------------------------------------------------------
hash() -- hash a variable-length key into a 32-bit value
  k     : the key (the unaligned variable-length array of bytes)
  len   : the length of the key, counting by bytes
  level : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  There are no funnels smaller than 32 bits.
About 34+5len instructions.

There is no need to divide by a prime (division is sooo slow!).  If
you need less than 32 bits, use a bitmask.  For example, if you need
only 10 bits, do
  h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (ub1 **)k, do it like this:
  for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

(c) Bob Jenkins, 1996.  74512.261@compuserve.com.  You may use this
code any way you wish, private, educational, or commercial, as long
as this whole comment accompanies it.

See http://ourworld.compuserve.com/homepages/bob_jenkins/evahash.htm
Use for hash table lookup, or anything where one collision in 2^32 is
acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
*/

static inline ub4 hash(register ub1 *k, register ub4 length, register ub4 level)
{
   register ub4 a,b,c,d,len;

   /* Set up the internal state */
   len = length;
   a = b = c = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
   d = level;               /* the previous hash value */

   /*---------------------------------------- handle most of the key */
   while (len >= 16)
   {
      a += (k[0] +((ub4)k[1]<<8) +((ub4)k[2]<<16) +((ub4)k[3]<<24));
      b += (k[4] +((ub4)k[5]<<8) +((ub4)k[6]<<16) +((ub4)k[7]<<24));
      c += (k[8] +((ub4)k[9]<<8) +((ub4)k[10]<<16)+((ub4)k[11]<<24));
      d += (k[12]+((ub4)k[13]<<8)+((ub4)k[14]<<16)+((ub4)k[15]<<24));
      mix(a,b,c,d);
      k += 16; len -= 16;
   }

   /*------------------------------------- handle the last 15 bytes */
   d += length;
   switch(len)              /* all the case statements fall through */
   {
   case 15: d+=((ub4)k[14]<<24);
   case 14: d+=((ub4)k[13]<<16);
   case 13: d+=((ub4)k[12]<<8);
      /* the first byte of d is reserved for the length */
   case 12: c+=((ub4)k[11]<<24);
   case 11: c+=((ub4)k[10]<<16);
   case 10: c+=((ub4)k[9]<<8);
   case 9 : c+=k[8];
   case 8 : b+=((ub4)k[7]<<24);
   case 7 : b+=((ub4)k[6]<<16);
   case 6 : b+=((ub4)k[5]<<8);
   case 5 : b+=k[4];
   case 4 : a+=((ub4)k[3]<<24);
   case 3 : a+=((ub4)k[2]<<16);
   case 2 : a+=((ub4)k[1]<<8);
   case 1 : a+=k[0];
     /* case 0: nothing left to add */
   }
   mix(a,b,c,d);
   /*-------------------------------------------- report the result */
   return d;
}
#endif

