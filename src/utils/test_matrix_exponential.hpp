#ifndef TEST_MATRIX_EXPONENTIAL_HPP
#define TEST_MATRIX_EXPONENTIAL_HPP

#include <complex>
//
//  Complex examples.
//
std::complex <double> *c8mat_exp_a(int test, int n);
std::complex <double> *c8mat_exp_expa(int test, int n);
int c8mat_exp_n(int test);
void c8mat_exp_story(int test);
int c8mat_exp_test_num( );

//
//  Real examples.
//
double *r8mat_exp_a(int test, int n);
double *r8mat_exp_expa(int test, int n);
int r8mat_exp_n(int test);
void r8mat_exp_story(int test);
int r8mat_exp_test_num( );

#endif
