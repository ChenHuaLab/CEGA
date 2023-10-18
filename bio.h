#ifndef BIO_H_INCLUDED
#define BIO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include <stdint.h>
#include "math.h"

#include "myfunc.h"

#define DBL_MAX 1.79769313486231470e+308

//ONLY for comparing char and int variables, and returns "1" if equal.
#define EQUAL(x,y) ((x)==(y) ? 1 : 0 )
#define Gd(x,y) ((x)>(y) ? 1.0 : 0.0)
#define GEd(x,y) ((x)>=(y) ? 1.0 : 0.0)
#define LEd(x,y) ((x)<=(y) ? 1.0 : 0.0)
#define GEi(x,y) ((x)>=(y) ? 1 : 0)

int l_log_fac;
double *log_fac;
double *sum_reciprocal;

//=========================================
void initial_bio();
double log_per(int n, int k);
double log_com(int n, int k);
double log_rfactor(int n, int k);
double rfactor(int n, int k);
double log_ffactor(int n, int k);
double ffactor(int n, int k);
double bico(int n, int k);
double factln(int n);
//=========================================
double part_1(int i, int m, int n);
double part_2(int i, int k, int n);
double part_3(int k1, int k2, int m1, int m2);
double part_4(int k, int i, int n, int m);
double polyaEdist(int n,int m,int b,int a);
double hyperGeodist(int n1,int n2,int k1,int k2);
//=========================================
double cal_integral(double start, double end, double (*growth_model)(double *, double), double *paras);
double Gnm(int n,int m,double T,double N,double g);
double g_t(int m, int n, double N, double T_integral, double g);
void nAFS(int n, double T, double N, double *Sb, double g, double mu, double *gt);
double *E_S_n_array(int n, double N, double mu, double T, double *gt);
double E_S_n(int i, int n, double N, double mu, double T, double *gt);
void E_T_not_gt(double *ET, int m, int n, double N, double mu, double T, double gt);
double E_S_a(int i, int n, double N, double mu);
void afs_m_growth(double *arr, int n, int m, double T, double T1, double N1, double N2, double mu, double g, double *gt1, double (*growth_model)(double *, double), double *paras);
void afs_growth(double *arr, int n, double T, double T1, double N1, double N2, double mu, double g, double (*growth_model)(double *, double), double *paras);
void afs_growth_one(char *para);
//=========================================
void s2_exp_jafs(double *arr, int row, int col, double N0, double N1, double N2, int n1, int n2, int T, double mu, double (*growth_model)(double *, double), double *paras);
void s2_exp_jafs_one1(int index);
void s2_exp_jafs_one2(int index);
void s2_exp_jafs_v2(double *arr, int row, int col, double N1, double N2, int n1, int n2, int T1, int T2, double mu, int l_esa, double *esa, double (*growth_model)(double *, double), double *paras);
void initial_cega(int n1, int n2);
void s2_exp_s1_s2_s12_D(double N0, double N1, double N2, int n1, int n2, int T, double mu, double (*growth_model)(double *, double), double *paras, double *res_s1, double *res_s2, double *res_s12, double *res_D);
void Asy_TwoIS_new(double *arr, int row, int col, double N1, double N2, int n1, int n2, int T1, int T2, double g1, double g2, double mu, int l_esa, double *esa, double (*growth_model)(double *, double), double *paras);
//=========================================
void Asy_AFS_Gazave_CG(int n,int m,double *N,double *T,double r,double *S,double mu);
double Asy_ET_Gazave_CG(int n,int m,double *N,double *T,double g,double *ET);
void ET_2_EW_CG(int n,double *ET,double *EW);
void AFS_fromEW(int n,double *S,double *EW,double mu);
double descendDist(int n,int j,int k);
void Asy_AFS_2Epoch_CG(int n,double T1,double N1,double N2,double *S,double mu);
double Asy_ET_2Epoch_CG(int n,double T1,double N1,double N2,double *ET);
double Asy_gnm_bound4Sum(int n,double T,double N,double g,int *LBn,int *UBn);
double Asy_gnm_2SEG_bound4Sum(int n,double T,double N,double g,double Tg,double Na,int *LBn,int *UBn);
void AFSreduct(int n,double *Sn,int m,double *Sm);
double hypgeom(int j,int n,int i,int m);
double Gnm_haploid(int n,int m,double T,double N,double g);
double Asy_gnm_prob(int n,int m,double T,double N,double g);
double Asy_gnm_2SEG_prob(int n,int m,double T,double N,double g,double Tg,double Na);
double trigamma(double x);
double Asy_ET_ExpGr_CG(int n,double T,double N,double g,double *ET,double *ST);
double Asy_ET_2SExpGr_CG(int n,double T,double N,double Na,double g,double *ET,double *ST);
void Asy_AFS_IG(int n,double T,double N,double g,double *S,double mu);
void Asy_ET_IG(int n,double T,double N,double g,double *ET,double *EW);
void Asy_AFS_IG_2SEGrowth(int n,double T,double Tg,double N,double Na,double g,double *S,double mu);
void Asy_ET_IG_2SEGrowth(int n,double T,double Tg,double N,double Na,double g,double *ET,double *EW);
double AFSreductII(int n,double *Sn,int m,int j, double *arr_hyper, int col);
double AFS3Dreduct12II(int n1,int n2,int n3,int i3,int m1,int m2,int j1,int j2,double *S3, double *S3_2, double *S3_3, int col1, int col2, int col3, double *arr_tmp1, double *arr_tmp2, double *arr_tmp3);
double MigP_2way(int nn1,int nn2,int ii1,int ii2,int m1,int m2,int k1,int k2,double beta12,double beta21);
//====================================
void Asy_TwoIS_1c1TSEG(int n1,int n2,double T1,double T2,double Tg,double N1,double N2,double Na2,double g,double *S0,double *S,double mu);
void Asy_TwoIS_onlySa_2SEG(int n1,int n2,double T1,double T2,double N1,double N2,double Tg,double g,double N2a,double *S0,double *S,double mu);
void Asy_TwoIS_onlySa_2SEG_one(char *para);
//====================================
int S3_threeMig12_fastII(int n1,int n2,int n3,double N1,double N2,double N3,double N4,double N5,double N6,double T1,double T2,double T3,double T4,double T5,double T0,double Tmig,double beta12,double beta21,double *S0,double *S2bm,double *S3,double mu);
void S2_twoIS_v2_fast(int n1,int n2,int col_S0,double T1,double T2,double N1,double N2,double *S0,double *S2,double mu);
void calcualte_part_1(char *para);
void calcualte_part_2(char *para);
void S2_twoIS_v2_fast_one(char *para);
void S3_2to3IS_v2_fastII(int n1,int n2, int n3, int n4, double N1,double N2,double N3,double T2,double T3,double *S2,double *S3,double mu);
void calculate_smj_one(char *para);
void S3_2to3IS_v2_fastII_one(char *para);
void Mig_3_2and1(int n1,int n2,int n3,double N1,double N2,double N3,double Tmig,double beta12,double beta21,double *S3bm,double *S3,double mu);
void Mig_3_2and1_one(char *para);
//====================================
void sweep_o_p2(int n1,int n2,double Td,double Ts,double N1,double N2,double N0,double r,double s,double g,double *Sn2,double mu);
void S2_p1Selection(int n1,int n2,double N0,double N1,double N2,double Ts,double r,double s,double *Sp2,double *Sn2,double mu);
void sweep_o_p1_s_S2(int n1,int n2,double Ts,double N1,double r,double s,double g,double *S0,double *Ss,double mu);
void sweep_o_p1_s_S2_one(int index);
double Ps_tk(int k,int n,double s,double r,double N);
void sweep_o_p1_S2(int n1,int n2,double Ts,double N1,double r,double s,double g,double *Ss,double *Sn,double mu);
void sweep_o_p1_S2_one(int index);
double Ps_value(int n,double s,double r,int N,double *PSValue);
double Ps(int S,int E,int L,double s,double r,int N);
double EpF(double alpha,double rho,double gamma,int n,int l);
double pF(double alpha,double gamma,int f);
double ppF(int n, int i);
//====================================
void Asy_ThreeIS_1c2TSEG(int n1,int n2,int n3,double Td23,double Td123,double T1a,double N1,double N2,double N3,double Na23,double N1a,double g2,double g3,double *S3,double mu);
void Asy_ThreeIS_1c2TSEG_one(char *para);
void Asy_S3_2to3IS(int n1,int n2, int n3,double N1,double N2,double N3,double T2,double T3,double g2,double g3,double *S2,double *S3,double mu);
void Asy_S3_2to3IS_one(char *para);
//====================================
#ifdef __cplusplus
}
#endif
#endif // BIO_H_INCLUDED
