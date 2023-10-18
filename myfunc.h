#ifndef MYFUNC_H_INCLUDED
#define MYFUNC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "ctype.h"
#include "math.h"
#include "zlib.h"
#include "assert.h"
#include "errno.h"
#include "pthread.h"
#include "dirent.h"
#include "setjmp.h"

#ifdef _USE_AVX
#include "immintrin.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <stdint.h>

#ifndef __APPLE__
#include <cpuid.h>
#include <x86intrin.h>
#endif

#ifndef _WIN32
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "net/if.h"
#include "arpa/inet.h"
#include "netdb.h"
#else
#include "windows.h"
#endif // _WIN32

#ifdef _use_bz2
#include "bzlib.h"
#endif  // _use_bz2

#ifdef _WIN32
#define TIMEB _timeb
#define FILE_SEP '\'
#else
#define TIMEB timeb
#define FILE_SEP '/'
#endif // _WIN32

#define true 1
#define false 0
#define null 0
#ifndef NULL
#define NULL 0
#endif // NULL
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif // INT_MAX
#ifndef INT_MIN
#define INT_MIN -2147483648
#endif // INT_MIN
#ifndef DOUBLE_MAX
#define DOUBLE_MAX 0x1.fffffffffffffP+1023  //1.7976931348623157e+308
#endif // DOUBLE_MAX
#ifndef DOUBLE_MIN
#define DOUBLE_MIN 0x0.0000000000001P-1022  //4.9e-324
#endif // DOUBLE_MIN
#ifndef NaN
#define NaN 0.0/0.0
#endif // NaN
#ifndef POSITIVE_INFINITY
#define POSITIVE_INFINITY 1.0/0.0
#endif // POSITIVE_INFINITY
#ifndef NEGATIVE_INFINITY
#define NEGATIVE_INFINITY -1.0/0.0
#endif // NEGATIVE_INFINITY
#define maxline 1000000000
#define socketBuffSize 104857600    //1024*1024*100
#define bam_max_score 16383
#define bam_max_score_pair 32766
#define gz_stream_buff_size 1048576 //1024*1024

#define M_GAMMA 0.57721566490153286060651209
#define SQRT2 1.4142135623730950488016887242097

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define log_gamma(x) (((x)+0.5)*log((x)+5.5)-((x)+5.5)+log(2.5066282746310005*((1.000000000190015)+(76.18009172947146/((x)+1.0))-(86.50532032941677/((x)+2.0))+(24.01409824083091/((x)+3.0))-(1.231739572450155/((x)+4.0))+(0.1208650973866179e-2/((x)+5.0))-(0.5395239384953e-5/((x)+6.0)))/(x)))

#define debug_info(buff) ({sprintf(buff, "[%s:%s:%d]", __FILE__, __func__, __LINE__);buff;})
#define my_new(num,size) ({void *res=calloc(num, size);if(!res){fprintf(stderr, "[%s:%s:%d]:calloc memory error!\n",__FILE__,__func__,__LINE__);exit(0);}res;})
#define my_new2(size) ({void *res=malloc(size);if(!res){fprintf(stderr, "[%s:%s:%d]:malloc memory error!\n",__FILE__,__func__,__LINE__);exit(0);}res;})
#define my_renew(ptr,size) ({void *res=realloc(ptr,size);if(!res){fprintf(stderr, "[%s:%s:%d]:realloc memory error!\n",__FILE__,__func__,__LINE__);exit(0);}res;})

#define set_matrix2(matrix,dim2,i,j,value) ({matrix[(i)*(dim2)+(j)]=(value);})
#define get_matrix2(matrix,dim2,i,j) (matrix[(i)*(dim2)+(j)])
#define set_matrix3(matrix,dim2,dim3,i,j,k,value) ({matrix[(i)*(dim2)*(dim3)+(j)*(dim3)+(k)]=(value);})
#define get_matrix3(matrix,dim2,dim3,i,j,k) (matrix[(i)*(dim2)*(dim3)+(j)*(dim3)+(k)])

char loginfo[1000001];
char line[maxline+1];
int l_line;
int l_cache_length;
int *arr_i_increase;
int64_t *arr_l_increase;
double *arr_d_increase;

//===========================================================

typedef struct my_pair_t{
    int64_t *left;
    int64_t *right;
}Pair;

typedef struct bam_header_t{
    char *header;
    int l_header;
    int l_chr;
    char **chr_names;
    int *l_chr_names;
    int *chr_lens;
}BamHeader;

typedef struct{
    gzFile fp;
    FILE *out;
#ifdef _use_bz2
    BZFILE *bz2File;
#endif // _use_bz2
    int bzerror;
    char *buf;
    int begin;
    int end;
    int is_write;
    int is_eof;
}GzStream;

typedef struct my_random_t {
    int64_t seed;
    int haveNextNextGaussian;
    double nextNextGaussian;
}MyRand;

typedef struct string_builder_t{
    int64_t tableSize;
    int64_t size;
    char *str;
}SBuilder;

typedef struct arrayList_int_t{
    int64_t tableSize;
    int64_t size;
    int *elementData;
}AList_i;

typedef struct arrayList_long_t{
    int64_t tableSize;
    int64_t size;
    int64_t *elementData;
}AList_l;

typedef struct arrayList_double_t{
    int64_t tableSize;
    int64_t size;
    double *elementData;
}AList_d;

typedef struct linked_list_node_long_t{
    int64_t value;
    int64_t prev;
    int64_t next;
}LListNode_l;

typedef struct linked_list_long_t{
    int size;
    LListNode_l *first;
    LListNode_l *last;
}LList_l;

typedef struct entry_long_double_t{
    int32_t hash;
    int64_t key;
    double value;
    void *next;
    LListNode_l *node;
}Entry_ld;

typedef struct hash_long_double_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_ld **table;
    LList_l *pairs;
}Hash_ld;

typedef struct entry_double_long_t{
    int32_t hash;
    double key;
    int64_t value;
    void *next;
    LListNode_l *node;
}Entry_dl;

typedef struct hash_double_long_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_dl **table;
    LList_l *pairs;
}Hash_dl;

typedef struct entry_double_int_t{
    int32_t hash;
    double key;
    int value;
    void *next;
    LListNode_l *node;
}Entry_di;

typedef struct hash_double_int_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_di **table;
    LList_l *pairs;
}Hash_di;

typedef struct entry_double_double_t{
    int32_t hash;
    double key;
    double value;
    void *next;
    LListNode_l *node;
}Entry_dd;

typedef struct hash_double_double_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_dd **table;
    LList_l *pairs;
}Hash_dd;

typedef struct entry_int_double_t{
    int32_t hash;
    int key;
    double value;
    void *next;
    LListNode_l *node;
}Entry_id;

typedef struct hash_int_double_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_id **table;
    LList_l *pairs;
}Hash_id;

typedef struct entry_int_long_t{
    int32_t hash;
    int key;
    int64_t value;
    void *next;
    LListNode_l *node;
}Entry_il;

typedef struct hash_int_long_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_il **table;
    LList_l *pairs;
}Hash_il;

typedef struct entry_long_int_t{
    int32_t hash;
    int64_t key;
    int value;
    void *next;
    LListNode_l *node;
}Entry_li;

typedef struct hash_long_int_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_li **table;
    LList_l *pairs;
}Hash_li;

typedef struct entry_long_long_t{
    int32_t hash;
    int64_t key;
    int64_t value;
    void *next;
    LListNode_l *node;
}Entry_ll;

typedef struct hash_long_long_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_ll **table;
    LList_l *pairs;
}Hash_ll;

typedef struct entry_int_int_t{
    int32_t hash;
    int key;
    int value;
    void *next;
    LListNode_l *node;
}Entry_ii;

typedef struct hash_int_int_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_ii **table;
    LList_l *pairs;
}Hash_ii;

typedef struct entry_str_double_t{
    int32_t hash;
    char *key;
    double value;
    void *next;
    LListNode_l *node;
}Entry_sd;

typedef struct hash_str_double_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_sd **table;
    LList_l *pairs;
}Hash_sd;

typedef struct entry_str_int_t{
    int32_t hash;
    char *key;
    int value;
    void *next;
    LListNode_l *node;
}Entry_si;

typedef struct hash_str_int_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_si **table;
    LList_l *pairs;
}Hash_si;

typedef struct entry_str_long_t{
    int32_t hash;
    char *key;
    int64_t value;
    void *next;
    LListNode_l *node;
}Entry_sl;

typedef struct hash_str_long_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_sl **table;
    LList_l *pairs;
}Hash_sl;

typedef struct entry_void_double_t{
    int32_t hash;
    void *key;
    double value;
    void *next;
    LListNode_l *node;
}Entry_vd;

typedef struct hash_void_double_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_vd **table;
    LList_l *pairs;
}Hash_vd;

typedef struct entry_void_int_t{
    int32_t hash;
    void *key;
    int value;
    void *next;
    LListNode_l *node;
}Entry_vi;

typedef struct hash_void_int_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_vi **table;
    LList_l *pairs;
}Hash_vi;

typedef struct entry_void_long_t{
    int32_t hash;
    void *key;
    int64_t value;
    void *next;
    LListNode_l *node;
}Entry_vl;

typedef struct hash_void_long_t{
    int32_t tableSize;
    int32_t size;
    int32_t threshold;
    Entry_vl **table;
    LList_l *pairs;
}Hash_vl;

typedef struct queue_t{
    int size;
    int head;
    int tail;
    int count;
    void **array;
    int is_finished;
    pthread_mutex_t locker;
    pthread_cond_t cond_put;
    pthread_cond_t cond_get;
}Queue;

typedef struct order_queue_t{
    int size;
    int head;
    int tail;
    int count;
    int index;
    void **array;
    int is_finished;
    pthread_mutex_t locker;
    pthread_cond_t cond_put;
    pthread_cond_t cond_get;
}OrderQueue;

typedef struct array_deque_long_t{
    int64_t *elements;
    int64_t l_elements;
    int64_t head;
    int64_t tail;
}ADeque_l;

typedef struct priority_queue_t{
    int capacity;
    void **queue;
    int size;
    int (*compare)(void *, void *);
}PriorityQueue;

typedef struct thread_worker_t{
    void *(*process)(void *arg);
    void *arg;
    void *next;
}ThreadWorker;

typedef struct thread_pool_t{
    int thread_num;
    int *is_wait;
    //--
    int fininshed_num;
    int is_finished;
    //--
    pthread_mutex_t locker;
    pthread_cond_t cond;
    pthread_cond_t cond2;
    //--
    ThreadWorker *head;
    //--
    pthread_t *threads;
}ThreadPool;

typedef struct{
    int n_data;
    int n_par;
    int evaluations;
    int iterations;
    double *value; //dim=n_data
    double *points; //dim=n_par
    double **jacobian;  //dim=n_data*n_par
    double *residuals;  //dim=n_data
    double cost;
}LeastSquaresEvaluation;

typedef struct{
    int n_data;
    int n_par;
    double **weightedJacobian;  //n_data*n_par
    int *permutation;    //n_par
    int rank;
    double *diagR;  //n_par
    double *jacNorm;    //n_par
    double *beta;   //n_par
}LeastSquaresInternalData;

//===========================================================

//==================== get hardware information
char *get_uuid();
char *get_processor_id();
char *get_serial_number();
char *path_2_absolute_path(char *path);
//==================== avx
void *calloc_align_ptr(int64_t size, int8_t align_bytes);
void free_align_ptr(void *ptr);
//================ string
char *getTime();
void mylog(char *str);
void myerror(char *str);
double format_double(double d, int nBits);
int64_t double_2_int64_by_union(double d);
int is_integer(char *str);
int is_double(char *str);
int is_digit(char *str);
int chmop(char *str);
int chmop_with_len(char *str, int len);
void freeArray(int len, void **array);
char *str_copy_range_of(char *str, int l_str, int from, int to, int *resLen);
char *substring(char *str, int64_t start, int64_t end);
char *str_2_lower_no_copy(char *str);
char *str_2_upper_no_copy(char *str);
char* str_2_lower_no_copy_2(char *str, int len);
char* str_2_upper_no_copy_2(char *str, int len);
char *str_2_lower_with_copy(char *str);
char *str_2_upper_with_copy(char *str);
char* str_2_lower_with_copy_2(char *str, int len);
char* str_2_upper_with_copy_2(char *str, int len);
int str_tab_index(char *str, char delimiter, int *tabs);
char **split(char *str, char delimiter, int *resNum, int *tabs);
char **split_with_no_copy(char *str, char delimiter, int *resNum, int *tabs);
char *str_copy(char *str);
char *str_copy_with_len(char *str, int len);
char *str_copy_and_ptr_end(char *buff, char *str);
int str_starts(char *str, char *sub);
int str_ends(char *str, char *sub);
int str_indexOf(char *str, char *sub);
int str_indexOf_from(char *str, char *sub, int from);
int mem_indexOf(void *ptr, int64_t l_ptr, void *sub, int64_t l_sub);
int mem_indexOf_from(void *ptr, int64_t l_ptr, void *sub, int64_t l_sub, int64_t from);
char *str_replace(char *orig, char *rep, char *with);
void str_replace_char_with_no_copy(char *str, char pre, char now);
char *str_reverse(char *str);
//================ file operation
AList_l *get_file_list(char *path);
int get_file_type(char *path);
int64_t get_file_size(char *path);
void *read_file_content(char *path, int64_t *resSize);
void write_file_content(char *path, void *buff, int64_t size);
void write_string_with_multi_lines(FILE *f, int strLen, char *str, int oneLineLen);
GzStream *gz_stream_open(char *file, char *mode);
int gz_read_util(GzStream *gz, char delimiter, char *buff, int max_len, int *res_len);
int gz_read(GzStream *gz, char *str, int len);
void gz_write(GzStream *gz, char *str, int len);
void gz_write_char(GzStream *gz, char c);
void gz_stream_destory(GzStream *gz);
char *get_md5(char *str, int str_len, int is_file);
//================ bio
char *get_fasta_name(char *str, int len);
char get_anti_base(char c);
char *anti_rev_seq(char *seq, int len);
//================ sam bam
int is_big_endian();
uint16_t swap_endian_2(uint16_t v);
void *swap_endian_2p(void *x);
uint32_t swap_endian_4(uint32_t v);
void *swap_endian_4p(void *x);
uint64_t swap_endian_8(uint64_t v);
void *swap_endian_8p(void *x);
int to_bam_header(int l_chrNames, char **chrNames, int *chrLengths, char *ID, char *SM, char *LB, char *buff);
void skip_bam_header(GzStream *bam, char *buff);
BamHeader *read_bam_header(GzStream *bam, char *buff);
void free_bamHeader(BamHeader *header);
int read_bam_to_sam(GzStream *bam, int l_chrNames, char **chrNames, char *buff, char *sam);
int bam_to_sam(int l_chrNames, char **chrNames, int datasize, char *buff, char *sam);
int sam_to_bam(int l_chrNames, char **chrNames, char *sam, int *tabs, char *buff);
uint32_t bam_calend(int pos, int n_cigar, int32_t *cigar);
int bam_reg2bin(uint32_t beg, uint32_t end);
char *get_md5(char *str, int str_len, int is_string_or_file);
//==================== java1.6 sort
void java_sort_int(int *arr, int left, int length);
static void java_sort_int_inner(int *x, int off, int len);
static void java_sort_int_swap(int *x, int a, int b);
static int java_sort_int_med3(int *x, int a, int b, int c);
static void java_sort_int_vecswap(int *x, int a, int b, int n);
void java_sort_long(int64_t *arr, int left, int length);
static void java_sort_long_inner(int64_t *x, int off, int len);
static void java_sort_long_swap(int64_t *x, int a, int b);
static int java_sort_long_med3(int64_t *x, int a, int b, int c);
static void java_sort_long_vecswap(int64_t *x, int a, int b, int n);
void java_sort_double(double *arr, int left, int length);
static void java_sort_double_inner(double *x, int off, int len);
static void java_sort_double_swap(double *x, int a, int b);
static int java_sort_double_med3(double *x, int a, int b, int c);
static void java_sort_double_vecswap(double *x, int a, int b, int n);
void java_sort_void(void **arr, int left, int length, int(*mycompare)(void *a, void *b));
static void java_sort_void_merge_sort(void **src, void **dest, int low, int high, int off, int(*mycompare)(void *a, void *b));
static void java_sort_void_swap(void **x, int a, int b);
//=============== java random
MyRand* new_my_rand1();
MyRand* new_my_rand2(int64_t seed);
static int my_rand_next(MyRand* mr, int bits);
int my_rand_next_int(MyRand* mr, int bound);
double my_rand_next_double(MyRand* mr);
double my_rand_next_gaussian(MyRand* mr);
//================ math
int *new_i_matrix2(int64_t row, int64_t col);
void set_i_matrix2(int *matrix, int64_t row, int64_t col, int64_t i, int64_t j, int value);
int get_i_matrix2(int *matrix, int64_t row, int64_t col, int64_t i, int64_t j);
int64_t *new_l_matrix2(int64_t row, int64_t col);
void set_l_matrix2(int64_t *matrix, int64_t row, int64_t col, int64_t i, int64_t j, int64_t value);
int64_t get_l_matrix2(int64_t *matrix, int64_t row, int64_t col, int64_t i, int64_t j);
float *new_f_matrix2(int64_t row, int64_t col);
void set_f_matrix2(float *matrix, int64_t row, int64_t col, int64_t i, int64_t j, float value);
float get_f_matrix2(float *matrix, int64_t row, int64_t col, int64_t i, int64_t j);
double *new_d_matrix2(int64_t row, int64_t col);
void set_d_matrix2(double *matrix, int64_t row, int64_t col, int64_t i, int64_t j, double value);
double get_d_matrix2(double *matrix, int64_t row, int64_t col, int64_t i, int64_t j);
int *new_i_matrix3(int64_t dim1, int64_t dim2, int64_t dim3);
void set_i_matrix3(int *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k, int value);
int get_i_matrix3(int *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k);
int64_t *new_l_matrix3(int64_t dim1, int64_t dim2, int64_t dim3);
void set_l_matrix3(int64_t *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k, int64_t value);
int64_t get_l_matrix3(int64_t *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k);
float *new_f_matrix3(int64_t dim1, int64_t dim2, int64_t dim3);
void set_f_matrix3(float *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k, float value);
float get_f_matrix3(float *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k);
double *new_d_matrix3(int64_t dim1, int64_t dim2, int64_t dim3);
void set_d_matrix3(double *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k, double value);
double get_d_matrix3(double *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k);
double dotProduct(double *vs1, double *vs2, int len);
double norm_L2(double *vs, int len);
void normalizeArray(double *vs, int len);
void avg_unbiased_var(int len, double *arr, double *res_avg, double *res_var);
void avg_biased_var(int len, double *arr, double *res_avg, double *res_var);
double norm_pdf(double x, double mu, double sigma);
double norm_cdf(double x, double mu, double sigma);
double gamm2(double a,double x,double e1,double e0);
double chi2_pdf(double x, int k);
double chi2_cdf(double x, int k);
static void matrix_multi_one(int index);
void matrix_multi(double *arr1, double *arr2, double *res1, int row1, int col1, int row2, int col2);
//=========================================================== optimize mcmc
double my_mcmc_min(int max_reject_num, int sample_type, int l_vec, double *vec, void *data, double *lowbound, double *upbound, double (*func)(double *, void *));
void my_mcmc_min_one(int index);
void my_mcmc_min_sample_gaussian(MyRand  *rand, int l_vec, double *vec, double *lowbound, double *upbound, double *res);
void my_mcmc_min_sample_uniform(MyRand  *rand, int l_vec, double *vec, double *lowbound, double *upbound, double *res);
//=========================================================== optimize powell (not support multi-thread)
void powell(double p[], int n, double ftol, int *iter, double *fret, double (*func)(double []));
void linmin(double p[], double xi[], int n, double *fret, double (*func)(double []));
double f1dim(double x);
void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb, double *fc, double (*func)(double));
double brent(double ax, double bx, double cx, double (*f)(double), double tol, double *xmin);
//=========================================================== optimize least squares
//------------------------------ fit gaussian_distribution
LeastSquaresEvaluation *gaussian_fit(int size, double *xs, double *ys);
//-- n_par=3 param[0]=scale param[1]=mu param[2]=sigma
double gaussian_func(double x, int n_par, double *param);
//-- n_par=3 param[0]=scale param[1]=mu param[2]=sigma
double *gaussian_gradient(double x, int n_par, double *param, double(*func)(double,int,double*));
double *gaussian_guess_start_points(int size, double *xs, double *ys);
double gaussian_interpolateXAtY(int size, double *xs, double *ys, int startIdx, int idxStep, double y, int *is_out_of_range);
//------------------------------ least squares
LeastSquaresEvaluation *leastSquares_optimize(int size, double *xs, double *ys, int n_par, double *start_points, double(*func)(double,int,double*), double*(*gradient)(double,int,double*,double(*)(double,int,double*)));
LeastSquaresEvaluation *leastSquares_evaluate(int size, double *xs, double *ys, int n_par, double *points, double(*func)(double,int,double*), double*(*gradient)(double,int,double*,double(*)(double,int,double*)));
void free_LeastSquaresEvaluation(LeastSquaresEvaluation *et);
double *leastSquares_default_gradient(double x, int n_par, double *param, double(*func)(double,int,double*));
LeastSquaresInternalData *leastSquares_qrDecomposition(int n_data, int n_par, double **jacobian, int solvedCols, double qrRankingThreshold);
void free_LeastSquaresInternalData(LeastSquaresInternalData *ld);
int leastSquares_compare_xs(char *str1, char *str2);
void leastSquares_qTy(double* y, LeastSquaresInternalData *internalData);
double leastSquares_determineLMParameter(double* qy, double delta, double* diag, LeastSquaresInternalData *internalData, int solvedCols, double* work1, double* work2, double* work3, double* lmDir, double lmPar);
void leastSquares_determineLMDirection(double* qy, double* diag, double* lmDiag, LeastSquaresInternalData *internalData, int solvedCols, double* work, double* lmDir);
//=========================================================== optimize Differential Evolution (DE)
double optimize_DE_min(int method_type, int npar, double *x, void *data, double (*func)(int, double*, void*), double *lowbound, double *upbound);
//================ socket
#ifndef _WIN32
int is_valid_ip(const char *ip);
int socket_listen(int port);
int socket_accept(int listen_id);
int socket_connect(char *ip, int port);
void set_socket_buff_size(int sock_id);
void socket_send_data(int socket_id, char *buff, int64_t size);
void socket_receive_data(int socket_id, char *buff, int64_t *resSize);
#endif // _WIN32
//===========================================================
static char *util_str_deep_copy(char *str);
static int32_t hash_int(int key);
static int32_t hash_long(int64_t key);
static int32_t hash_double(double d);
int32_t hash_str(char *key);
//=============== string_builder
SBuilder *new_s_builder(int64_t capacity);
SBuilder *s_builder_deep_copy(SBuilder *sb);
void s_builder_add_char(SBuilder *sb, char c);
void s_builder_add_int(SBuilder *sb, int64_t v);
void s_builder_add_double(SBuilder *sb, double v);
void s_builder_add_str(SBuilder *sb, char *str);
void s_builder_add_str_with_len(SBuilder *sb, char *str, int len);
void s_builder_add_str_with_len2(SBuilder *sb, char *str, int len);
void s_builder_compress(SBuilder *sb);
void s_builder_ensure(SBuilder *sb, int64_t minCapacity);
void free_s_builder(SBuilder *sb);
//=============== arrayList_int
AList_i *new_alist_i(int64_t initialCapacity);
AList_i *alist_i_deep_copy(AList_i *list);
void alist_i_add(AList_i *list, int value);
static void alist_i_ensure(AList_i *list, int64_t minCapacity);
void free_alist_i(AList_i *list);
//=============== arrayList_long
AList_l *new_alist_l(int64_t initialCapacity);
AList_l *alist_l_deep_copy(AList_l *list);
void alist_l_add(AList_l *list, int64_t value);
static void alist_l_ensure(AList_l *list, int64_t minCapacity);
void free_alist_l(AList_l *list);
//=============== arrayList_double
AList_d *new_alist_d(int64_t initialCapacity);
AList_d *alist_d_deep_copy(AList_d *list);
void alist_d_add(AList_d *list, double value);
static void alist_d_ensure(AList_d *list, int64_t minCapacity);
void free_alist_d(AList_d *list);
//=============== linkedList_long
LList_l *new_llist_l();
void llist_l_add(LList_l *list, int64_t value);
void llist_l_add_first(LList_l *list, int64_t value);
LListNode_l *llist_l_get(LList_l *list, int i);
void llist_l_remove(LList_l *list, LListNode_l *n);
void llist_l_remove2(LList_l *list, int64_t value);
void llist_l_clear(LList_l *list);
void free_llist_l(LList_l *list);
//=============== hash_long_double
static void free_entry_ld(Entry_ld *e);
Hash_ld *new_hash_ld();
Hash_ld *new_hash_ld1(int32_t size);
Hash_ld *new_hash_ld2(int32_t size, double load_factor);
Entry_ld *hash_ld_get(Hash_ld *hp, int64_t key);
void hash_ld_put(Hash_ld *hp, int64_t key, double value);
static void hash_ld_resize(Hash_ld *hp, int32_t newCapacity);
void hash_ld_remove(Hash_ld *hp, int64_t key);
void hash_ld_clear(Hash_ld *hp);
void free_hash_ld(Hash_ld *hp);
//=============== hash_double_long
static void free_entry_dl(Entry_dl *e);
Hash_dl *new_hash_dl();
Hash_dl *new_hash_dl1(int32_t size);
Hash_dl *new_hash_dl2(int32_t size, double load_factor);
Entry_dl *hash_dl_get(Hash_dl *hp, double key);
void hash_dl_put(Hash_dl *hp, double key, int64_t value);
static void hash_dl_resize(Hash_dl *hp, int32_t newCapacity);
void hash_dl_remove(Hash_dl *hp, double key);
void hash_dl_clear(Hash_dl *hp);
void free_hash_dl(Hash_dl *hp);
//=============== hash_double_int
static void free_entry_di(Entry_di *e);
Hash_di *new_hash_di();
Hash_di *new_hash_di1(int32_t size);
Hash_di *new_hash_di2(int32_t size, double load_factor);
Entry_di *hash_di_get(Hash_di *hp, double key);
void hash_di_put(Hash_di *hp, double key, int value);
static void hash_di_resize(Hash_di *hp, int32_t newCapacity);
void hash_di_remove(Hash_di *hp, double key);
void hash_di_clear(Hash_di *hp);
void free_hash_di(Hash_di *hp);
//=============== hash_double_double
static void free_entry_dd(Entry_dd *e);
Hash_dd *new_hash_dd();
Hash_dd *new_hash_dd1(int32_t size);
Hash_dd *new_hash_dd2(int32_t size, double load_factor);
Entry_dd *hash_dd_get(Hash_dd *hp, double key);
void hash_dd_put(Hash_dd *hp, double key, double value);
static void hash_dd_resize(Hash_dd *hp, int32_t newCapacity);
void hash_dd_remove(Hash_dd *hp, double key);
void hash_dd_clear(Hash_dd *hp);
void free_hash_dd(Hash_dd *hp);
//=============== hash_int_double
static void free_entry_id(Entry_id *e);
Hash_id *new_hash_id();
Hash_id *new_hash_id1(int32_t size);
Hash_id *new_hash_id2(int32_t size, double load_factor);
Entry_id *hash_id_get(Hash_id *hp, int key);
void hash_id_put(Hash_id *hp, int key, double value);
static void hash_id_resize(Hash_id *hp, int32_t newCapacity);
void hash_id_remove(Hash_id *hp, int key);
void hash_id_clear(Hash_id *hp);
void free_hash_id(Hash_id *hp);
//=============== hash_int_long
static void free_entry_il(Entry_il *e);
Hash_il *new_hash_il();
Hash_il *new_hash_il1(int32_t size);
Hash_il *new_hash_il2(int32_t size, double load_factor);
Entry_il *hash_il_get(Hash_il *hp, int key);
void hash_il_put(Hash_il *hp, int key, int64_t value);
static void hash_il_resize(Hash_il *hp, int32_t newCapacity);
void hash_il_remove(Hash_il *hp, int key);
void hash_il_clear(Hash_il *hp);
void free_hash_il(Hash_il *hp);
//=============== hash_long_int
static void free_entry_li(Entry_li *e);
Hash_li *new_hash_li();
Hash_li *new_hash_li1(int32_t size);
Hash_li *new_hash_li2(int32_t size, double load_factor);
Entry_li *hash_li_get(Hash_li *hp, int64_t key);
void hash_li_put(Hash_li *hp, int64_t key, int value);
static void hash_li_resize(Hash_li *hp, int32_t newCapacity);
void hash_li_remove(Hash_li *hp, int64_t key);
void hash_li_clear(Hash_li *hp);
void free_hash_li(Hash_li *hp);
//=============== hash_long_long
static void free_entry_ll(Entry_ll *e);
Hash_ll *new_hash_ll();
Hash_ll *new_hash_ll1(int32_t size);
Hash_ll *new_hash_ll2(int32_t size, double load_factor);
Entry_ll *hash_ll_get(Hash_ll *hp, int64_t key);
void hash_ll_put(Hash_ll *hp, int64_t key, int64_t value);
static void hash_ll_resize(Hash_ll *hp, int32_t newCapacity);
void hash_ll_remove(Hash_ll *hp, int64_t key);
void hash_ll_clear(Hash_ll *hp);
void free_hash_ll(Hash_ll *hp);
//=============== hash_int_int
static void free_entry_ii(Entry_ii *e);
Hash_ii *new_hash_ii();
Hash_ii *new_hash_ii1(int32_t size);
Hash_ii *new_hash_ii2(int32_t size, double load_factor);
Entry_ii *hash_ii_get(Hash_ii *hp, int key);
void hash_ii_put(Hash_ii *hp, int key, int value);
static void hash_ii_resize(Hash_ii *hp, int32_t newCapacity);
void hash_ii_remove(Hash_ii *hp, int key);
void hash_ii_clear(Hash_ii *hp);
void free_hash_ii(Hash_ii *hp);
//=============== hash_str_double
static void free_entry_sd(Entry_sd *e);
Hash_sd *new_hash_sd();
Hash_sd *new_hash_sd1(int32_t size);
Hash_sd *new_hash_sd2(int32_t size, double load_factor);
Entry_sd *hash_sd_get(Hash_sd *hp, char *key);
void hash_sd_put(Hash_sd *hp, char *key, double value);
static void hash_sd_resize(Hash_sd *hp, int32_t newCapacity);
void hash_sd_remove(Hash_sd *hp, char *key);
void hash_sd_clear(Hash_sd *hp);
void free_hash_sd(Hash_sd *hp);
//=============== hash_str_int
static void free_entry_si(Entry_si *e);
Hash_si *new_hash_si();
Hash_si *new_hash_si1(int32_t size);
Hash_si *new_hash_si2(int32_t size, double load_factor);
Entry_si *hash_si_get(Hash_si *hp, char *key);
void hash_si_put(Hash_si *hp, char *key, int value);
static void hash_si_resize(Hash_si *hp, int32_t newCapacity);
void hash_si_remove(Hash_si *hp, char *key);
void hash_si_clear(Hash_si *hp);
void free_hash_si(Hash_si *hp);
//=============== hash_str_long
static void free_entry_sl(Entry_sl *e);
Hash_sl *new_hash_sl();
Hash_sl *new_hash_sl1(int32_t size);
Hash_sl *new_hash_sl2(int32_t size, double load_factor);
Entry_sl *hash_sl_get(Hash_sl *hp, char *key);
void hash_sl_put(Hash_sl *hp, char *key, int64_t value);
static void hash_sl_resize(Hash_sl *hp, int32_t newCapacity);
void hash_sl_remove(Hash_sl *hp, char *key);
void hash_sl_clear(Hash_sl *hp);
void free_hash_sl(Hash_sl *hp);
//=============== hash_void_double
static void free_entry_vd(Entry_vd *e, void(*free_key)(void *key));
Hash_vd *new_hash_vd();
Hash_vd *new_hash_vd1(int32_t size);
Hash_vd *new_hash_vd2(int32_t size, double load_factor);
Entry_vd *hash_vd_get(Hash_vd *hp, void *key, int hash, int(*compare)(void *a, void *b));
void hash_vd_put(Hash_vd *hp, void *key, int hash, double value, int(*compare)(void *a, void *b));
static void hash_vd_resize(Hash_vd *hp, int32_t newCapacity);
void hash_vd_remove(Hash_vd *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key));
void hash_vd_clear(Hash_vd *hp, void(*free_key)(void *key));
void free_hash_vd(Hash_vd *hp, void(*free_key)(void *key));
//=============== hash_void_int
static void free_entry_vi_2(Entry_vi *e, void(*free_key)(void *key));
Hash_vi *new_hash_vi();
Hash_vi *new_hash_vi1(int32_t size);
Hash_vi *new_hash_vi2(int32_t size, double load_factor);
Entry_vi *hash_vi_get(Hash_vi *hp, void *key, int hash, int(*compare)(void *a, void *b));
void hash_vi_put(Hash_vi *hp, void *key, int hash, int value, int(*compare)(void *a, void *b));
static void hash_vi_resize(Hash_vi *hp, int32_t newCapacity);
void hash_vi_remove(Hash_vi *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key));
void hash_vi_clear(Hash_vi *hp, void(*free_key)(void *key));
void free_hash_vi(Hash_vi *hp, void(*free_key)(void *key));
//=============== hash_void_long
static void free_entry_vl(Entry_vl *e, void(*free_key)(void *key));
Hash_vl *new_hash_vl();
Hash_vl *new_hash_vl1(int32_t size);
Hash_vl *new_hash_vl2(int32_t size, double load_factor);
Entry_vl *hash_vl_get(Hash_vl *hp, void *key, int hash, int(*compare)(void *a, void *b));
void hash_vl_put(Hash_vl *hp, void *key, int hash, int64_t value, int(*compare)(void *a, void *b));
static void hash_vl_resize(Hash_vl *hp, int32_t newCapacity);
void hash_vl_remove(Hash_vl *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key));
void hash_vl_clear(Hash_vl *hp, void(*free_key)(void *key));
void free_hash_vl(Hash_vl *hp, void(*free_key)(void *key));
//===============   queue
Queue *new_queue(int size);
void queue_put(Queue *q, void *data);
void queue_set_finished(Queue *q);
void *queue_get(Queue *q);
void free_queue(Queue *q);
//===============
OrderQueue *new_order_queue(int size);
void order_queue_put(OrderQueue *q, void *data, int index);
void order_queue_set_finished(OrderQueue *q);
void *order_queue_get(OrderQueue *q);
void free_order_queue(OrderQueue *q);
//=============== array_deque_long
ADeque_l *new_adeque_l(int64_t initialSize);
static void adeque_l_double_capacity(ADeque_l *q);
void adeque_l_add_first(ADeque_l *q, int64_t value);
void adeque_l_add_last(ADeque_l *q, int64_t value);
int64_t *adeque_l_poll_first(ADeque_l *q);
int64_t *adeque_l_poll_last(ADeque_l *q);
int64_t *adeque_l_peek_first(ADeque_l *q);
int64_t *adeque_l_peek_last(ADeque_l *q);
void adeque_l_add(ADeque_l *q, int64_t value);
int64_t *adeque_l_poll(ADeque_l *q);
int64_t *adeque_l_peek(ADeque_l *q);
void free_adeque_l(ADeque_l *q);
//=============== PriorityQueue
PriorityQueue *new_priority_queue(int capacity, int (*compare)(void *, void *));
int priority_queue_add(PriorityQueue *q, void *value);
void *priority_queue_poll(PriorityQueue *q);
void free_priority_queue(PriorityQueue *q, void (*myfree)(void *));
//=============== thread pool
void go_run_thread(void *(*func)(void *arg), void *arg);
void wg_wait_thread();
ThreadPool *new_thread_pool(int pool_thread_num);
static void *thread_routine(void *args);
void thread_pool_add_worker(ThreadPool *p, void *(*func)(void *arg), void *arg);
void thread_pool_invoke_all(ThreadPool *p);
void free_thread_pool(ThreadPool *p);
//===========================================================
//=============== global variables
//-- (producer-consumer) for read and write file
Queue *queue_for_read_file;
Queue *queue_for_write_file;
//--
int thread_num;
ThreadPool *thread_pool;
//===========================================================

#ifdef __cplusplus
}
#endif

#endif // MYFUNC_H_INCLUDED


