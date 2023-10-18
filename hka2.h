#ifndef HKA2_H_INCLUDED
#define HKA2_H_INCLUDED

#include "myfunc.h"
#include "bio.h"

#include "optimize.h"

//==============================
typedef struct{
    char *chr;
    int start;
    int end;
    int length;
}Win1;

typedef struct{
    int t_win;
    int l_win;
    Win1 **wins;
}Win2;

typedef struct{
    int l_chr;
    char **chrs;
    Win2 **data;
}Win3;

typedef struct pop_data_one_chr_t{
    int t_data;
    int l_data;
    int *data;
    int *pos;
}Pop1;

typedef struct pop_data_one_t{
    int n_sample;
    int l_chr;
    char **chrs;
    Pop1 **pops;
}Pop2;

typedef struct s2_t{
    int dim1;
    int dim2;
    double *arr;
    double s1;
    double s2;
    double s12;
    double s12_1;
    double s12_2;
    double D;
    double D1;
    double D2;
    double estimate_s1;
    double estimate_s2;
    double estimate_s12;
    double estimate_D;
}S2;



//==============================
char *pop1_file;
char *pos1_file;
char *pop2_file;
char *pos2_file;
char *afs_file;
int thread_num;
int optimize_type;
int win_size;
int step_size;
int is_calculate_LRT;
char *win_size_file;
char *win_size_file_for_global;
char *out_file;
//--
ThreadPool *pool;
pthread_mutex_t *p_locker;
//----------
Win3 *all_wins;
Win3 *all_wins_global;
S2 *global_s2;
//--
AList_l *all_s2s_wins;
AList_l *all_s2s_wins_global;
AList_l *all_s2s;
AList_l *all_s2s_global;

//-- likelihood global paras
double func_n1;
double func_n2;
double global_L;
double global_N0;
double global_N1;
double global_N2;
double global_T;
//--
double *res_1_mu;
double *res_1_like;
double *res_2_mu;
double *res_2_lambda1;
double *res_2_lambda2;
double *res_2_like;
double *res_2_like_mu_lambda1;
double *res_2_like_mu_lambda2;
double *res_2_LLR1;
double *res_2_LLR2;



//==============================
void load_parameters(int argc, char **argv);
void print_usage();
//----------
Win3 *read_win_size_file(char *file);
AList_l *read_data();
Pop1 *get_pop1_by_chr(char *chr, Pop2 *data);
S2 *get_S2(int *flag1, int *flag2, int start, int end, int dim1, int dim2);
void complete_S2(S2 *s2);
Pop2 *read_hap_file(char *hap_file, char *pos_file);
Pop2 *read_tped_file(char *tped_file);
Pop2 *read_vcf_file(char *vcf_file);
S2 *read_afs_file();
//----------
void grid_paras(int len, double *x, double *low, double *up, int max, double (*func)(double*,void *), void *data);
void grid_paras_perm(int len, int index, AList_d **all_paras, double *stack, AList_l *list);
//----------
void estimate_exp_jafs(S2 *exp_s2, double *arr, int row, int col, double N0, double N1, double N2, int n1, int n2, double T, double mu, double lambda2, double L, double (*growth_model)(double *, double), double *paras);
double log_likelihood_afs_func(double *x, void *data);
double log_likelihood_func(double *x, void *data);
double log_likelihood_mu_func(double *x, void *data);
double log_likelihood_mu_lambda_func(double *x, void *data);
double log_likelihood_mu_lambda1_func(double *x, void *data);
double log_likelihood_mu_lambda2_func(double *x, void *data);
void optimize_mu_for_win(int index);
void optimize_mu_lambda_for_win(int index);
void optimize_mu_lambda1_for_win(int index);
void optimize_mu_lambda2_for_win(int index);
double get_log_poisson_probability(int k, double lambda);
//----------
void get_nlambda_and_P(AList_d *L, AList_d *nlambda, AList_d *p);
AList_d *bcTransform(double par, AList_d *L);
AList_d *box_cox(AList_d *L, double *res_par);
double func_for_optimize_box_cox(double *x, void *data);
//----------
void print_S2_1(S2 *s2);
void print_S2(S2 *s2);
void write_pop2(Pop2 *p2, char *file);
//==============================

#endif // HKA2_H_INCLUDED
