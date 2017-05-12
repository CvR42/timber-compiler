
////////////////
// longlong.h //
////////////////

// stub classes to support 64-bit integers

#ifndef LONGLONG_H
#define LONGLONG_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

class signed_longlong {
 public:
  signed   long hi;
  unsigned long lo;
 public:
  signed_longlong()
    {
      hi = lo = 0;
    };
  signed_longlong(const signed long HI, const unsigned long LO)
    {
      hi = HI;
      lo = LO;
    }
  signed_longlong(const long x)
    {
      hi = -(x<0);
      lo = x;
    };
  //  operator unsigned long ()
  //    {
  //      return lo;
  //    };

  friend signed_longlong operator + (const signed_longlong A,
				     const signed_longlong B)
  {
    int carry;
    signed_longlong result;

    carry = (A.lo > 0xffffffffUL - B.lo);

    result.lo = A.lo +  B.lo;
    result.hi = A.hi + (B.hi+carry);

    return result;
  }

  friend signed_longlong operator - (const signed_longlong A,
				     const signed_longlong B)
  {
    int borrow;
    signed_longlong result;

    borrow = (B.lo > A.lo);

    result.lo = A.lo -  B.lo;
    result.hi = A.hi - (B.hi+borrow);

    return result;
  }

  friend signed_longlong operator & (const signed_longlong A,
				     const signed_longlong B)
  {
    signed_longlong result;

    result.lo = A.lo & A.lo;
    result.hi = B.hi & A.hi;

    return result;
  }

  friend signed_longlong operator | (const signed_longlong A,
				     const signed_longlong B)
  {
    signed_longlong result;

    result.lo = A.lo | A.lo;
    result.hi = B.hi | A.hi;

    return result;
  }

  friend signed_longlong operator ^ (const signed_longlong A,
				     const signed_longlong B)
  {
    signed_longlong result;

    result.lo = A.lo ^ A.lo;
    result.hi = B.hi ^ A.hi;

    return result;
  }

  friend bool operator == (const signed_longlong A,
			   const signed_longlong B)
  {
    return (A.lo==B.lo) && (A.hi==B.hi);
  }
  
  friend bool operator != (const signed_longlong A,
			   const signed_longlong B)
  {
    return (A.lo!=B.lo) || (A.hi!=B.hi);
  }

  friend signed_longlong operator << (const signed_longlong A, int shift)
  {
    signed_longlong result = A;
    while(shift--)
      {
	int carry = ((signed long)result.lo<0);
	result.hi+=(result.hi+carry);
	result.lo+=result.lo;
      }
    return result;
  }

  friend signed_longlong operator >> (const signed_longlong A, int shift)
  {
    signed_longlong result = A;
    while(shift--)
      {
	int carry = (result.hi&1);
	result.hi>>=1;
	result.lo>>=1;
	if (carry)
	  result.lo |= 0x80000000UL;
      }
    return result;
  }

  operator double()
    {
      if (hi>=0)
	return hi*4294967296.0+lo;
      else
	return ((unsigned long)hi)*4294967296.0+lo-18446744073709551616.0;
    }
  /*
  operator float()
    {
      if (hi>=0)
	return hi*4294967296.0+lo;
      else
	return ((unsigned long)hi)*4294967296.0+lo-18446744073709551616.0;
    }
  */
};

class unsigned_longlong {
 public:
  unsigned_longlong(const int x);
};

#endif
