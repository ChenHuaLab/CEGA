#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "math.h"
#include "time.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _linux
#include "pthread.h"
#include "dirent.h"
#endif

//=============== global

static char *util_str_deep_copy(char *str);
static int32_t hash_int(int key);
static int32_t hash_long(int64_t key);
static int32_t hash_double(double d);
int32_t hash_str(char *key);

//=============== pair

typedef struct my_pair_t{
    int64_t *left;
    int64_t *right;
}Pair;

//=============== linkedList_long

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

LList_l *new_llist_l();
void llist_l_add(LList_l *list, int64_t value);
void llist_l_add_first(LList_l *list, int64_t value);
LListNode_l *llist_l_get(LList_l *list, int i);
void llist_l_remove(LList_l *list, LListNode_l *n);
void llist_l_remove2(LList_l *list, int64_t value);
void llist_l_clear(LList_l *list);
void free_llist_l(LList_l *list);

//=============== hash_long_double

typedef  struct entry_long_double_t{
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

typedef  struct entry_double_long_t{
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

typedef  struct entry_double_int_t{
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

typedef  struct entry_double_double_t{
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

typedef  struct entry_int_double_t{
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

typedef  struct entry_int_long_t{
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

typedef  struct entry_long_int_t{
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

typedef  struct entry_long_long_t{
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

typedef  struct entry_int_int_t{
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

//=============== arrayList_str

typedef struct arrayList_str_t{
    int64_t tableSize;
    int64_t size;
    char **elementData;
}AList_str;

AList_str *new_alist_str(int64_t initialCapacity);
void alist_str_add(AList_str *list, char *str);
void alist_str_add_with_no_copy(AList_str *list, char *str);
static void alist_str_ensure(AList_str *list, int64_t minCapacity);
void free_alist_str(AList_str *list);

//=============== arrayList_mem

typedef struct arrayList_mem_t{
    int64_t tableSize;
    int64_t size;
    void **elementData;
    int64_t *elementLength;
}AList_mem;

AList_mem *new_alist_mem(int64_t initialCapacity);
void alist_mem_add(AList_mem *list, void *data, int64_t size);
static void alist_mem_ensure(AList_mem *list, int64_t minCapacity);
void free_alist_mem(AList_mem *list);

//=============== arrayList_int

typedef struct arrayList_int_t{
    int64_t tableSize;
    int64_t size;
    int *elementData;
}AList_i;

AList_i *new_alist_i(int64_t initialCapacity);
AList_i *alist_i_deep_copy(AList_i *list);
void alist_i_add(AList_i *list, int value);
static void alist_i_ensure(AList_i *list, int64_t minCapacity);
void free_alist_i(AList_i *list);

//=============== arrayList_long

typedef struct arrayList_long_t{
    int64_t tableSize;
    int64_t size;
    int64_t *elementData;
}AList_l;

AList_l *new_alist_l(int64_t initialCapacity);
AList_l *alist_l_deep_copy(AList_l *list);
void alist_l_add(AList_l *list, int64_t value);
static void alist_l_ensure(AList_l *list, int64_t minCapacity);
void free_alist_l(AList_l *list);

//=============== arrayList_doubleAList_l *alist_l_deep_copy(AList_l *list)

typedef struct arrayList_double_t{
    int64_t tableSize;
    int64_t size;
    double *elementData;
}AList_d;

AList_d *new_alist_d(int64_t initialCapacity);
AList_d *alist_d_deep_copy(AList_d *list);
void alist_d_add(AList_d *list, double value);
static void alist_d_ensure(AList_d *list, int64_t minCapacity);
void free_alist_d(AList_d *list);

//=============== string_builder

typedef struct string_builder_t{
    int64_t tableSize;
    int64_t size;
    char *str;
}SBuilder;

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

#ifdef _linux

//===============   queue

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

Queue *new_queue(int size);
void queue_put(Queue *q, void *data);
void queue_set_finished(Queue *q);
void *queue_get(Queue *q);
void free_queue(Queue *q);

//===============

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

OrderQueue *new_order_queue(int size);
void order_queue_put(OrderQueue *q, void *data, int index);
void order_queue_set_finished(OrderQueue *q);
void *order_queue_get(OrderQueue *q);
void free_order_queue(OrderQueue *q);

//=============== array_deque_long

typedef struct array_deque_long_t{
    int64_t *elements;
    int64_t l_elements;
    int64_t head;
    int64_t tail;
}ADeque_l;

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

typedef struct priority_queue_t{
    int capacity;
    void **queue;
    int size;
    int (*compare)(void *, void *);
}PriorityQueue;

PriorityQueue *new_priority_queue(int capacity, int (*compare)(void *, void *));
int priority_queue_add(PriorityQueue *q, void *value);
void *priority_queue_poll(PriorityQueue *q);
void free_priority_queue(PriorityQueue *q, void (*myfree)(void *));

//=============== thread pool

int64_t starts_thread[20000];
int64_t ends_thread[20000];

void split_span_for_threads(int thread_num, int64_t start, int64_t end);
void go_run_thread(void *(*func)(void *arg), void *arg);
void wg_wait_thread();

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

ThreadPool *new_thread_pool(int pool_thread_num);
static void *thread_routine(void *args);
void thread_pool_add_worker(ThreadPool *p, void *(*func)(void *arg), void *arg);
void thread_pool_invoke_all(ThreadPool *p);
void free_thread_pool(ThreadPool *p);

//=============== file
AList_l *get_file_list(char *path);
int get_file_type(char *path);
Pair *split_file_size(char *file, int num);

//=============== global variables
//-- (producer-consumer) for read and write file
Queue *queue_for_read_file;
Queue *queue_for_write_file;

#else

//=============== File operation

void get_all_files_with_prefix(char *path, char *prefix, AList_l *files);
void get_all_files_with_suffix(char *path, char *suffix, AList_l *files);

#endif

//=============== java random

typedef struct my_random_t {
    int64_t seed;
    int haveNextNextGaussian;
    double nextNextGaussian;
}MyRand;

MyRand* new_my_rand1();
MyRand* new_my_rand2(int64_t seed);
static int my_rand_next(MyRand* mr, int bits);
int my_rand_next_int(MyRand* mr, int bound);
double my_rand_next_double(MyRand* mr);
double my_rand_next_gaussian(MyRand* mr);
//===============

#ifdef __cplusplus
}
#endif
#endif // UTIL_H_INCLUDED
















