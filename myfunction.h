#ifndef MYFUNCTION_H_INCLUDED
#define MYFUNCTION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "ctype.h"
#include "math.h"
#include "assert.h"

#include <stdint.h>
#include "immintrin.h"

#ifdef _linux
#include "zlib.h"
#include <cpuid.h>
#include <x86intrin.h>
#include "sys/socket.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include "arpa/inet.h"
#include "netdb.h"
#endif

#ifdef _use_bz2
#include "bzlib.h"
#endif  // _use_bz2

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
#define maxline 100000000
#define socketBuffSize 104857600    //1024*1024*100

#define M_GAMMA 0.57721566490153286060651209

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#ifdef _linux
#define debug_info(buff) ({sprintf(buff, "[%s:%s:%d]", __FILE__, __func__, __LINE__);buff;})
#define my_new(num,size) ({void *res=calloc(num, size);if(!res){fprintf(stderr, "[%s:%s:%d]:calloc memory error!\n",__FILE__,__func__,__LINE__);exit(0);}res;})
#define my_new2(size) ({void *res=malloc(size);if(!res){fprintf(stderr, "[%s:%s:%d]:malloc memory error!\n",__FILE__,__func__,__LINE__);exit(0);}res;})
#define my_renew(ptr,size) ({void *res=realloc(ptr,size);if(!res){fprintf(stderr, "[%s:%s:%d]:realloc memory error!\n",__FILE__,__func__,__LINE__);exit(0);}res;})
#else
#define debug_info(buff) sprintf(buff, "[%s:%s:%d]", __FILE__, __func__, __LINE__)
#define my_new(num,size) calloc(num, size)
#define my_new2(size) malloc(size)
#define my_renew(ptr,size) realloc(ptr,size)
#endif

#define bam_max_score 16383
#define bam_max_score_pair 32766
#define gz_stream_buff_size 1048576 //1024*1024

char line[maxline+1];
char loginfo[maxline+1];
int l_line;
int avx_bit;

//================

typedef struct{
    char *s;
    int64_t l;
}String;

typedef struct bam_header_t{
    char *header;
    int l_header;
    int l_chr;
    char **chr_names;
    int *l_chr_names;
    int *chr_lens;
}BamHeader;

typedef struct bam_buff_t{
    int64_t name;
    int flag;
    int chr;
    int mtid;
    int pos;
    int coordinate;
    int score;
    int ref_start;
    int ref_end;
    int l_bam;
    void *bam;
}BamBuff;

#ifdef _linux
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
#endif // _linux

//==================== get hardware information
char *get_uuid();
char *get_processor_id();
char *get_serial_number();

//================ avx
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
char *substring(char *str, int start, int end);
char *substring2(char *str, int64_t start, int64_t end);
char *str_2_lower_no_copy(char *str);
char *str_2_upper_no_copy(char *str);
char* str_2_lower_no_copy_2(char *str, int len);
char* str_2_upper_no_copy_2(char *str, int len);
char *str_2_lower_with_copy(char *str);
char *str_2_upper_with_copy(char *str);
char* str_2_lower_with_copy_2(char *str, int len);
char* str_2_upper_with_copy_2(char *str, int len);
int str_tab_index(char *str, char delimiter, int *tabs, int is_newline_break);
char **split(char *str, char c, int *arrayNum);
char **split_with_len(char *str, int len, char c, int *arrayNum);
char *str_copy(char *str);
char *str_copy_with_len(char *str, int len);
char *str_copy_and_ptr_end(char *buff, char *str);
int str_starts(char *str, char *sub);
int str_ends(char *str, char *sub);
int str_indexOf(char *str, char *sub);
int str_indexOf_from(char *str, char *sub, int from);
char *str_replace(char *orig, char *rep, char *with);
void str_replace_char_with_no_copy(char *str, char pre, char now);
char *str_reverse(char *str);
//================ file operation
int64_t get_file_size(char *path);
void *read_file_content(char *path, int64_t *resSize);
void write_file_content(char *path, void *buff, int64_t size);
void write_string_with_multi_lines(FILE *f, int strLen, char *str, int oneLineLen);
#ifdef _linux
GzStream *gz_stream_open(char *file, char *mode);
int gz_read_util(GzStream *gz, char delimiter, char *buff, int max_len, int *res_len);
int gz_read(GzStream *gz, char *str, int len);
void gz_write(GzStream *gz, char *str, int len);
void gz_write_char(GzStream *gz, char c);
void gz_stream_destory(GzStream *gz);
#endif // _linux
//================ sam bam
int to_bam_header(int l_chrNames, char **chrNames, int *chrLengths, char *ID, char *SM, char *LB, char *buff);
#ifdef _linux
void skip_bam_header(GzStream *bam, char *buff);
BamHeader *read_bam_header(GzStream *bam, char *buff);
int read_bam_to_sam(GzStream *bam, int l_chrNames, char **chrNames, char *buff, char *sam);
void free_bamHeader(BamHeader *header);
#endif // _linux
int bam_to_sam(int l_chrNames, char **chrNames, int datasize, char *buff, char *sam);
int sam_to_bam(int l_chrNames, char **chrNames, char *sam, int *tabs, char *buff);
BamBuff *sam_to_bam_buff(int l_chrNames, char **chrNames, char *sam, int *tabs, char *buff);
uint32_t bam_calend(int pos, int n_cigar, int32_t *cigar);
int bam_reg2bin(uint32_t beg, uint32_t end);
char *get_md5(char *str, int str_len, int is_string_or_file);
//==================== copy
int *copy_int_array(int size, int *array);
int64_t *copy_long_array(int size, int64_t *array);
double *copy_double_array(int size, double *array);
//================ sort
void mysort_int(int *a, int left, int length);
void mysort_int_inner(int *a, int left, int right, int leftmost);
void mysort_long(int64_t *a, int left, int length);
void mysort_long_inner(int64_t *a, int left, int rigth, int leftmost);
void mysort_longlong(int64_t  *a, int left, int length);
void mysort_longlong_inner(int64_t  *a, int left, int right, int leftmost);
void mysort_short(short *a, int left, int length);
void mysort_short_doSort(short *a, int left, int right, short *work, int workBase, int workLen);
void mysort_short_inner(short *a, int left, int right, int leftmost);
void mysort_char(char *a, int left, int length);
void mysort_char_inner(char *a, int left, int right, int leftmost);
void mysort_float(float *a, int left, int length);
void mysort_float_inner(float *a, int left, int right, int leftmost);
void mysort_double(double *a, int left, int length);
void mysort_double_inner(double *a, int left, int right, int leftmost);
void mysort_void(void **arr, int left, int length, int (*mycompare)(void *a, void *b));
void mysort_void_inner(void **arr, int left, int right, int leftmost, int (*mycompare)(void *a, void *b));
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
//================ math
double dotProduct(double *vs1, double *vs2, int len);
double norm_L2(double *vs, int len);
void normalizeArray(double *vs, int len);
void avg_unbiased_var(int len, double *arr, double *res_avg, double *res_var);
void avg_biased_var(int len, double *arr, double *res_avg, double *res_var);
double norm_pdf(double x, double mu, double sigma);
double norm_cdf(double x, double mu, double sigma);
double log_gamma(double x);
double gamm2(double a,double x,double e1,double e0);
double chi2_pdf(double x, int k);
double chi2_cdf(double x, int k);
//================ socket
#ifdef _linux
int is_valid_ip(const char *ip);
char *get_local_ip();
int socket_listen(int port);
int socket_accept(int listen_id);
int socket_connect(char *ip, int port);
void set_socket_buff_size(int sock_id);
void socket_send_data(int socket_id, char *buff, int64_t size);
void socket_receive_data(int socket_id, char *buff, int64_t *resSize);
#endif
//================

#ifdef __cplusplus
}
#endif
#endif // MYFUNCTION_H_INCLUDED
