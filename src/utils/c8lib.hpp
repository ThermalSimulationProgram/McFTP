#ifndef C8LIB_C8LIB_H
#define C8LIB_C8LIB_H

#include <complex>
#include <cstring>

double            c8_abs(std::complex <double> x);
std::complex <double> c8_acos(std::complex <double> c1);
std::complex <double> c8_acosh(std::complex <double> c1);
std::complex <double> c8_add(std::complex <double> c1, std::complex <double> c2);
double            c8_arg(std::complex <double> x);
std::complex <double> c8_asin(std::complex <double> c1);
std::complex <double> c8_asinh(std::complex <double> c1);
std::complex <double> c8_atan(std::complex <double> c1);
std::complex <double> c8_atanh(std::complex <double> c1);
std::complex <double> c8_conj(std::complex <double> c1);
void              c8_copy(std::complex <double> c1, std::complex <double> c2);
std::complex <double> c8_cos(std::complex <double> c1);
std::complex <double> c8_cosh(std::complex <double> c1);
std::complex <double> c8_cube_root(std::complex <double> x);
std::complex <double> c8_div(std::complex <double> c1, std::complex <double> c2);
std::complex <double> c8_div_r8(std::complex <double> c1, double r);
std::complex <double> c8_exp(std::complex <double> c1);
std::complex <double> c8_i( );
double            c8_imag(std::complex <double> c);
std::complex <double> c8_inv(std::complex <double> c1);
bool              c8_le_l1(std::complex <double> x, std::complex <double> y);
bool              c8_le_l2(std::complex <double> x, std::complex <double> y);
bool              c8_le_li(std::complex <double> x, std::complex <double> y);
std::complex <double> c8_log(std::complex <double> c1);
double            c8_mag(std::complex <double> x);
std::complex <double> c8_mul(std::complex <double> c1, std::complex <double> c2);
std::complex <double> c8_neg(std::complex <double> c1);
std::complex <double> c8_nint(std::complex <double> c1);
double            c8_norm_l1(std::complex <double> x);
double            c8_norm_l2(std::complex <double> x);
double            c8_norm_li(std::complex <double> x);
std::complex <double> c8_normal_01(int&seed);
std::complex <double> c8_one( );
void              c8_print(std::complex <double> a, std::string title);
double            c8_real(std::complex <double> c);
std::complex <double> c8_sin(std::complex <double> c1);
std::complex <double> c8_sinh(std::complex <double> c1);
std::complex <double> c8_sqrt(std::complex <double> x);
std::complex <double> c8_sub(std::complex <double> c1, std::complex <double> c2);
void              c8_swap(std::complex <double>&x, std::complex <double>&y);
std::complex <double> c8_tan(std::complex <double> c1);
std::complex <double> c8_tanh(std::complex <double> c1);
void              c8_to_cartesian(std::complex <double> c, double&x, double&y);
void              c8_to_polar(std::complex <double> c, double&r, double&theta);
std::complex <double> c8_uniform_01(int&seed);
std::complex <double> c8_zero( );

void              c8mat_add(int m, int n, std::complex <double> alpha, std::complex <double> a[],
                            std::complex <double> beta, std::complex <double> b[], std::complex <double> c[]);
void              c8mat_add_r8(int m, int n, double alpha, std::complex <double> a[],
                               double beta, std::complex <double> b[], std::complex <double> c[]);
void              c8mat_copy(int m, int n, std::complex <double> a1[],
                             std::complex <double> a2[]);
std::complex <double> *c8mat_copy_new(int m, int n, std::complex <double> a1[]);
void                c8mat_fss(int n, std::complex <double> a[], int nb, std::complex <double> x[]);
std::complex <double> *c8mat_fss_new(int n, std::complex <double> a[], int nb,
                                     std::complex <double> b[]);
std::complex <double> *c8mat_identity_new(int n);
std::complex <double> *c8mat_indicator_new(int m, int n);
void              c8mat_minvm(int n1, int n2, std::complex <double> a[],
                              std::complex <double> b[], std::complex <double> e[]);
std::complex <double> *c8mat_minvm_new(int n1, int n2, std::complex <double> a[],
                                       std::complex <double> b[]);
void              c8mat_mm(int n1, int n2, int n3, std::complex <double> a[],
                           std::complex <double> b[], std::complex <double> c[]);
std::complex <double> *c8mat_mm_new(int n1, int n2, int n3, std::complex <double> a[],
                                    std::complex <double> b[]);
void              c8mat_nint(int m, int n, std::complex <double> a[]);
double            c8mat_norm_fro(int m, int n, std::complex <double> a[]);
double            c8mat_norm_l1(int m, int n, std::complex <double> a[]);
double            c8mat_norm_li(int m, int n, std::complex <double> a[]);
void              c8mat_print(int m, int n, std::complex <double> a[], std::string title);
void              c8mat_print_some(int m, int n, std::complex <double> a[], int ilo,
                                   int jlo, int ihi, int jhi, std::string title);
void              c8mat_scale(int m, int n, std::complex <double> alpha, std::complex <double> a[]);
void              c8mat_scale_r8(int m, int n, double alpha, std::complex <double> a[]);
void              c8mat_uniform_01(int m, int n, int&seed,
                                   std::complex <double> c[]);
std::complex <double> *c8mat_uniform_01_new(int m, int n, int&seed);
std::complex <double> *c8mat_zero_new(int m, int n);

void              c8vec_copy(int n, std::complex <double> a1[],
                             std::complex <double> a2[]);
std::complex <double> *c8vec_copy_new(int n, std::complex <double> a1[]);
std::complex <double> *c8vec_indicator_new(int n);
void              c8vec_nint(int n, std::complex <double> a[]);
double            c8vec_norm_l1(int n, std::complex <double> a[]);
double            c8vec_norm_l2(int n, std::complex <double> a[]);
double            c8vec_norm_li(int n, std::complex <double> a[]);
void              c8vec_print(int n, std::complex <double> a[], std::string title);
void              c8vec_print_part(int n, std::complex <double> a[], int max_print,
                                   std::string title);
void              c8vec_print_some(int n, std::complex <double> a[], int i_lo,
                                   int i_hi, std::string title);
void              c8vec_sort_a_l1(int n, std::complex <double> x[]);
void              c8vec_sort_a_l2(int n, std::complex <double> x[]);
void              c8vec_sort_a_li(int n, std::complex <double> x[]);
std::complex <double> *c8vec_spiral_new(int n, int m, std::complex <double> c1,
                                        std::complex <double> c2);
std::complex <double> *c8vec_uniform_01_new(int n, int&seed);
std::complex <double> *c8vec_unity_new(int n);

std::complex <double> cartesian_to_c8(double x, double y);

std::complex <double> polar_to_c8(double r, double theta);

// double            r8_abs ( double x );
// double            r8_atan ( double y, double x );
std::complex <double> r8_csqrt(double x);

// double            r8_floor ( double x );
// double            r8_max ( double x, double y );
// double            r8_sign ( double x );
// double            r8_uniform_01 ( int &seed );
void              r8poly2_root(double a, double b, double c,
                               std::complex <double>&r1, std::complex <double>&r2);
void              r8poly3_root(double a, double b, double c, double d,
                               std::complex <double>&r1, std::complex <double>&r2,
                               std::complex <double>&r3);
void              r8poly4_root(double a, double b, double c, double d, double e,
                               std::complex <double>&r1, std::complex <double>&r2, std::complex <double>&r3,
                               std::complex <double>&r4);

// void              sort_heap_external ( int n, int &indx, int &i, int &j, int isgn );
// void              timestamp ( );


#endif
