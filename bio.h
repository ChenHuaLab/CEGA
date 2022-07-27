#ifndef BIO_H_INCLUDED
#define BIO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#define DBL_MAX 1.79769313486231470e+308

int l_log_fac;
double *log_fac;

//=========================================
void initial_bio();
double log_gamm(double xx);
double log_per(int n, int k);
double log_com(int n, int k);
//=========================================
double part_1(int i, int m, int n);
double part_2(int i, int k, int n);
double part_3(int k1, int k2, int m1, int m2);
double part_4(int k, int i, int n, int m);
//=========================================
double cal_integral(double start, double end, double (*growth_model)(double *, double), double *paras);
double g_t(int m, int n, double N, double T_integral);
void E_T_not_gt(double *ET, int m, int n, double N, double mu, double T, double gt);
double E_S_n(int i, int n, double N, double mu, double T, double *gt);
double E_S_a(int i, int n, double N, double mu);
void afs_m_growth(double *arr, int n, int m, double T, double T1, double N1, double N2, double mu, double *gt1, double (*growth_model)(double *, double), double *paras);
void afs_growth(double *arr, int n, double T, double T1, double N1, double N2, double mu, double (*growth_model)(double *, double), double *paras);
//=========================================
void s2_exp_jafs(double *arr, int row, int col, double N0, double N1, double N2, int n1, int n2, int T, double mu, double (*growth_model)(double *, double), double *paras);
void s2_exp_jafs_v2(double *arr, int row, int col, double N1, double N2, int n1, int n2, int T1, int T2, double mu, int l_esa, double *esa, double (*growth_model)(double *, double), double *paras);
void s2_exp_s1_s2_s12_D(double N0, double N1, double N2, int n1, int n2, int T, double mu, double (*growth_model)(double *, double), double *paras, double *res_s1, double *res_s2, double *res_s12, double *res_D);
//=========================================
void s3_exp_2_to_3Is(double *arr, int dim1, int dim2, int dim3, int n1, int n2, int n3, double N1, double N2, double N3, double T1, double T2, double mu, double *s_0, double (*growth_model)(double *, double), double *paras);
void s3_exp_3Is(double *arr, int dim1, int dim2, int dim3, int n1, int n2 ,int n3, double N1, double N2, double N3,double N4,double N5,double N6, double TT1, double TT2, double TT3, double TT0, double mu, double (*growth_model)(double *, double), double *paras);
void print_array_dim3(double *arr, int dim1, int dim2, int dim3);
//=========================================
void Asy_AFS_Gazave_CG(int n,int m,double *N,double *T,double r,double *S,double mu);
double Asy_ET_Gazave_CG(int n,int m,double *N,double *T,double g,double *ET);
void ET_2_EW_CG(int n,double *ET,double *EW);
double AFS_fromEW(int n,double *S,double *EW,double mu);
double descendDist(int n,int j,int k);

//====================================

#ifdef __cplusplus
}
#endif
#endif // BIO_H_INCLUDED
