
///////////////
// complex.h //
///////////////

// stub class to support complex numbers

#ifdef __GNUC__
#if __GNUC__<3
#include <complex.h>
#else
#include <complex>
#endif
#else
#ifndef COMPLEX_H
#define COMPLEX_H

template < class T>
class complex {
 private:
  T re;
  T im;
 public:
  complex<T>(T r, T i) { re=r; im=i; };
  complex<T>(T r) { re=r ; im=0; };
  complex<T>() { re=im=0; };

  friend complex<T> conj(const complex<T> a)
  {
    complex<T> r;
    r.re =  a.re;
    r.im = -a.im;
    return r;
  }
  friend T real(const complex<T> a)
  {
    return a.re;
  }
  friend T imag(complex<T> a)
  {
    return a.im;
  }

  friend complex<T> operator +(complex<T> a, complex<T> b)
    {
      complex<T> r;
      r.re = a.re + b.re;
      r.im = a.im + b.im;
      return r;
    }
  friend bool operator ==(complex<T> a, complex<T> b)
    {
      return (a.re==b.re && a.im==b.im);
    }
  friend complex<T> operator -(const complex<T> a, const complex<T> b)
    {
      complex<T> r;
      r.re = a.re - b.re;
      r.im = a.im - b.im;
      return r;
    }
  friend complex<T> operator *(const complex<T> a, const complex<T> b)
    {
      complex<T> r;
      r.re = a.re * b.re - a.im*b.im;
      r.im = a.re * b.im + b.re*a.im;
      return r;
    }
  friend complex<T> operator /(complex<T> a,complex<T> b)
    {
      complex<T> r;
      r.re = (a.re*b.re+a.im*b.im)/(b.re*b.re+b.im*b.im);
      r.im = (a.im*b.re-a.re*b.im)/(b.re*b.re+b.im*b.im);
      return r;
    }
};

#endif
#endif
