#ifndef MATRIX_EXPONENTIAL_H
#define MATRIX_EXPONENTIAL_H

#include <complex>
//
//  Complex functions.
//
std::complex <double> *c8mat_expm1(int n, std::complex <double> a[]);

//
//  Real functions.
//
double *r8mat_expm1(int n, double a[]);
double *r8mat_expm2(int n, double a[]);
double *r8mat_expm3(int n, double a[]);


#endif
