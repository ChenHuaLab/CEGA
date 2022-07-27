/**
 * util.c
 *
 *  Created on: 2016-8-11
 *      Author: chilianjiang
 */

#include "myutil.h"

#define indexFor(h,length) h&(length-1)

#define DEFAULT_INITIAL_CAPACITY  16
#define MAXIMUM_CAPACITY  1<<30
#define DEFAULT_LOAD_FACTOR 0.75

//=============== global

static char *util_str_deep_copy(char *str){
    int len=strlen(str)+1;
    char *res=(char *)calloc(len, sizeof(char));
    memcpy(res, str, len-1);
    return res;
}

static int32_t hash_int(int key){
    int32_t h = (int32_t)key;
	// This function ensures that hashCodes that differ only by
	// constant multiples at each bit position have a bounded
	// number of collisions (approximately 8 at default load factor).
	h ^= (h >> 20) ^ (h >> 12);
	return h ^ (h >> 7) ^ (h >> 4);
}

static int32_t hash_long(int64_t key){
    int32_t h=(int32_t)(key ^ (key >> 32));
    // This function ensures that hashCodes that differ only by
	// constant multiples at each bit position have a bounded
	// number of collisions (approximately 8 at default load factor).
	h ^= (h >> 20) ^ (h >> 12);
	return h ^ (h >> 7) ^ (h >> 4);
}

static int32_t hash_double(double d){
    int64_t *l=(int64_t *)(&d);
    return hash_long(*(l));
}

int32_t hash_str(char *key){
    int32_t i=0, h=0;
    while(key[i]) h=31*h+key[i++];
    return h;
}

//=============== linkedList_long

LList_l *new_llist_l(){
    return (LList_l *)calloc(1, sizeof(LList_l));
}

void llist_l_add(LList_l *list, int64_t value){
    LListNode_l *n=(LListNode_l *)calloc(1, sizeof(LListNode_l));
    n->value=value;
    if(!list->first){
        list->first=n;
        list->last=n;
    }else{
        list->last->next=(int64_t)n;
        n->prev=(int64_t)list->last;
        list->last=n;
    }
    list->size++;
}

void llist_l_add_first(LList_l *list, int64_t value){
    LListNode_l *n=(LListNode_l *)calloc(1, sizeof(LListNode_l));
    n->value=value;
    if(!list->first){
        list->first=n;
        list->last=n;
    }else{
        list->first->prev=(int64_t)n;
        n->next=(int64_t)list->first;
        list->first=n;
    }
    list->size++;
}

LListNode_l *llist_l_get(LList_l *list, int i){
    if(list->size<=i) return NULL;

    int j;
    if(i<(list->size>>1)){
        LListNode_l *x=list->first;
        for(j=0;j<i;j++) x=(LListNode_l *)x->next;
        return x;
    }else{
        LListNode_l *x=list->last;
        for(j=list->size-1;j>i;j--) x=(LListNode_l *)x->prev;
        return x;
    }
}

void llist_l_remove(LList_l *list, LListNode_l *n){
    if(n->next) ((LListNode_l *)n->next)->prev=n->prev;
    else list->last=(LListNode_l *)n->prev;
    //--
    if(n->prev) ((LListNode_l *)n->prev)->next=n->next;
    else list->first=(LListNode_l *)n->next;
    //--
    list->size--;
    free(n);
}

void llist_l_remove2(LList_l *list, int64_t value){
    LListNode_l *n=list->first;
    while(n){
        LListNode_l *c=n;
        n=(LListNode_l *)n->next;
        //--
        if(c->value==value) llist_l_remove(list, c);
    }
}

void llist_l_clear(LList_l *list){
    LListNode_l *n=list->first;
    while(n){
        LListNode_l *c=n;
        n=(LListNode_l *)n->next;
        free(c);
    }
    list->size=0;
    list->first=NULL;
    list->last=NULL;
}

void free_llist_l(LList_l *list){
    LListNode_l *n=list->first;
    while(n){
        LListNode_l *c=n;
        n=(LListNode_l *)n->next;
        free(c);
    }
    free(list);
}

//=============== hash_long_double

static void free_entry_ld(Entry_ld *e){
    if(!e) return;
    free_entry_ld((Entry_ld *)e->next);
    free(e);
}

Hash_ld *new_hash_ld(){
    return new_hash_ld2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_ld *new_hash_ld1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_ld2(size, DEFAULT_LOAD_FACTOR);
}

Hash_ld *new_hash_ld2(int32_t size, double load_factor){
    Hash_ld *res=malloc(sizeof(Hash_ld));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_ld *));
    res->pairs=new_llist_l();
    return res;
}

Entry_ld *hash_ld_get(Hash_ld *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    Entry_ld *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_ld *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_ld_put(Hash_ld *hp, int64_t key, double value){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ld *e=hp->table[i];

    for(;e!=NULL;e=(Entry_ld *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_ld));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_ld_resize(hp, hp->tableSize*2);
}

static void hash_ld_resize(Hash_ld *hp, int32_t newCapacity){
    Entry_ld **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_ld **newTable=(Entry_ld **)calloc(hp->tableSize, sizeof(Entry_ld *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_ld *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_ld *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_ld_remove(Hash_ld *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ld *e=hp->table[i];
    Entry_ld *pre=NULL;

    for(;e!=NULL;e=(Entry_ld *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_ld *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_ld_clear(Hash_ld *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_ld(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_ld(Hash_ld *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_ld(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_double_long

static void free_entry_dl(Entry_dl *e){
    if(!e) return;
    free_entry_dl((Entry_dl *)e->next);
    free(e);
}

Hash_dl *new_hash_dl(){
    return new_hash_dl2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_dl *new_hash_dl1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_dl2(size, DEFAULT_LOAD_FACTOR);
}

Hash_dl *new_hash_dl2(int32_t size, double load_factor){
    Hash_dl *res=malloc(sizeof(Hash_dl));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_dl *));
    res->pairs=new_llist_l();
    return res;
}

Entry_dl *hash_dl_get(Hash_dl *hp, double key){
    int32_t tmpHash=hash_double(key);
    Entry_dl *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_dl *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_dl_put(Hash_dl *hp, double key, int64_t value){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_dl *e=hp->table[i];

    for(;e!=NULL;e=(Entry_dl *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_dl));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_dl_resize(hp, hp->tableSize*2);
}

static void hash_dl_resize(Hash_dl *hp, int32_t newCapacity){
    Entry_dl **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_dl **newTable=(Entry_dl **)calloc(hp->tableSize, sizeof(Entry_dl *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_dl *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_dl *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_dl_remove(Hash_dl *hp, double key){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_dl *e=hp->table[i];
    Entry_dl *pre=NULL;

    for(;e!=NULL;e=(Entry_dl *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_dl *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_dl_clear(Hash_dl *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_dl(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_dl(Hash_dl *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_dl(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_double_int

static void free_entry_di(Entry_di *e){
    if(!e) return;
    free_entry_di((Entry_di *)e->next);
    free(e);
}

Hash_di *new_hash_di(){
    return new_hash_di2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_di *new_hash_di1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_di2(size, DEFAULT_LOAD_FACTOR);
}

Hash_di *new_hash_di2(int32_t size, double load_factor){
    Hash_di *res=malloc(sizeof(Hash_di));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_di *));
    res->pairs=new_llist_l();
    return res;
}

Entry_di *hash_di_get(Hash_di *hp, double key){
    int32_t tmpHash=hash_double(key);
    Entry_di *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_di *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_di_put(Hash_di *hp, double key, int value){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_di *e=hp->table[i];

    for(;e!=NULL;e=(Entry_di *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_di));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_di_resize(hp, hp->tableSize*2);
}

static void hash_di_resize(Hash_di *hp, int32_t newCapacity){
    Entry_di **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_di **newTable=(Entry_di **)calloc(hp->tableSize, sizeof(Entry_di *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_di *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_di *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_di_remove(Hash_di *hp, double key){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_di *e=hp->table[i];
    Entry_di *pre=NULL;

    for(;e!=NULL;e=(Entry_di *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_di *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_di_clear(Hash_di *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_di(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_di(Hash_di *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_di(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_double_double

static void free_entry_dd(Entry_dd *e){
    if(!e) return;
    free_entry_dd((Entry_dd *)e->next);
    free(e);
}

Hash_dd *new_hash_dd(){
    return new_hash_dd2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_dd *new_hash_dd1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_dd2(size, DEFAULT_LOAD_FACTOR);
}

Hash_dd *new_hash_dd2(int32_t size, double load_factor){
    Hash_dd *res=malloc(sizeof(Hash_dd));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_dd *));
    res->pairs=new_llist_l();
    return res;
}

Entry_dd *hash_dd_get(Hash_dd *hp, double key){
    int32_t tmpHash=hash_double(key);
    Entry_dd *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_dd *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_dd_put(Hash_dd *hp, double key, double value){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_dd *e=hp->table[i];

    for(;e!=NULL;e=(Entry_dd *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_dd));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_dd_resize(hp, hp->tableSize*2);
}

static void hash_dd_resize(Hash_dd *hp, int32_t newCapacity){
    Entry_dd **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_dd **newTable=(Entry_dd **)calloc(hp->tableSize, sizeof(Entry_dd *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_dd *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_dd *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_dd_remove(Hash_dd *hp, double key){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_dd *e=hp->table[i];
    Entry_dd *pre=NULL;

    for(;e!=NULL;e=(Entry_dd *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_dd *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_dd_clear(Hash_dd *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_dd(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_dd(Hash_dd *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_dd(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_int_double

static void free_entry_id(Entry_id *e){
    if(!e) return;
    free_entry_id((Entry_id *)e->next);
    free(e);
}

Hash_id *new_hash_id(){
    return new_hash_id2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_id *new_hash_id1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_id2(size, DEFAULT_LOAD_FACTOR);
}

Hash_id *new_hash_id2(int32_t size, double load_factor){
    Hash_id *res=malloc(sizeof(Hash_id));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_id *));
    res->pairs=new_llist_l();
    return res;
}

Entry_id *hash_id_get(Hash_id *hp, int key){
    int32_t tmpHash=hash_int(key);
    Entry_id *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_id *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_id_put(Hash_id *hp, int key, double value){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_id *e=hp->table[i];

    for(;e!=NULL;e=(Entry_id *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_id));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_id_resize(hp, hp->tableSize*2);
}

static void hash_id_resize(Hash_id *hp, int32_t newCapacity){
    Entry_id **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_id **newTable=(Entry_id **)calloc(hp->tableSize, sizeof(Entry_id *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_id *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_id *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_id_remove(Hash_id *hp, int key){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_id *e=hp->table[i];
    Entry_id *pre=NULL;

    for(;e!=NULL;e=(Entry_id *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_id *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_id_clear(Hash_id *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_id(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_id(Hash_id *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_id(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_int_long

static void free_entry_il(Entry_il *e){
    if(!e) return;
    free_entry_il((Entry_il *)e->next);
    free(e);
}

Hash_il *new_hash_il(){
    return new_hash_il2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_il *new_hash_il1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_il2(size, DEFAULT_LOAD_FACTOR);
}

Hash_il *new_hash_il2(int32_t size, double load_factor){
    Hash_il *res=malloc(sizeof(Hash_il));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_il *));
    res->pairs=new_llist_l();
    return res;
}

Entry_il *hash_il_get(Hash_il *hp, int key){
    int32_t tmpHash=hash_int(key);
    Entry_il *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_il *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_il_put(Hash_il *hp, int key, int64_t value){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_il *e=hp->table[i];

    for(;e!=NULL;e=(Entry_il *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_il));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_il_resize(hp, hp->tableSize*2);
}

static void hash_il_resize(Hash_il *hp, int32_t newCapacity){
    Entry_il **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_il **newTable=(Entry_il **)calloc(hp->tableSize, sizeof(Entry_il *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_il *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_il *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_il_remove(Hash_il *hp, int key){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_il *e=hp->table[i];
    Entry_il *pre=NULL;

    for(;e!=NULL;e=(Entry_il *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_il *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_il_clear(Hash_il *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_il(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_il(Hash_il *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_il(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_long_int

static void free_entry_li(Entry_li *e){
    if(!e) return;
    free_entry_li((Entry_li *)e->next);
    free(e);
}

Hash_li *new_hash_li(){
    return new_hash_li2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_li *new_hash_li1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_li2(size, DEFAULT_LOAD_FACTOR);
}

Hash_li *new_hash_li2(int32_t size, double load_factor){
    Hash_li *res=malloc(sizeof(Hash_li));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_li *));
    res->pairs=new_llist_l();
    return res;
}

Entry_li *hash_li_get(Hash_li *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    Entry_li *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_li *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_li_put(Hash_li *hp, int64_t key, int value){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_li *e=hp->table[i];

    for(;e!=NULL;e=(Entry_li *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_li));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_li_resize(hp, hp->tableSize*2);
}

static void hash_li_resize(Hash_li *hp, int32_t newCapacity){
    Entry_li **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_li **newTable=(Entry_li **)calloc(hp->tableSize, sizeof(Entry_li *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_li *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_li *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_li_remove(Hash_li *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_li *e=hp->table[i];
    Entry_li *pre=NULL;

    for(;e!=NULL;e=(Entry_li *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_li *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_li_clear(Hash_li *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_li(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_li(Hash_li *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_li(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_long_long

static void free_entry_ll(Entry_ll *e){
    if(!e) return;
    free_entry_ll((Entry_ll *)e->next);
    free(e);
}

Hash_ll *new_hash_ll(){
    return new_hash_ll2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_ll *new_hash_ll1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_ll2(size, DEFAULT_LOAD_FACTOR);
}

Hash_ll *new_hash_ll2(int32_t size, double load_factor){
    Hash_ll *res=malloc(sizeof(Hash_ll));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_ll *));
    res->pairs=new_llist_l();
    return res;
}

Entry_ll *hash_ll_get(Hash_ll *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    Entry_ll *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_ll *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_ll_put(Hash_ll *hp, int64_t key, int64_t value){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ll *e=hp->table[i];

    for(;e!=NULL;e=(Entry_ll *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_ll));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_ll_resize(hp, hp->tableSize*2);
}

static void hash_ll_resize(Hash_ll *hp, int32_t newCapacity){
    Entry_ll **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_ll **newTable=(Entry_ll **)calloc(hp->tableSize, sizeof(Entry_ll *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_ll *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_ll *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_ll_remove(Hash_ll *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ll *e=hp->table[i];
    Entry_ll *pre=NULL;

    for(;e!=NULL;e=(Entry_ll *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_ll *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_ll_clear(Hash_ll *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_ll(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_ll(Hash_ll *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_ll(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_int_int

static void free_entry_ii(Entry_ii *e){
    if(!e) return;
    free_entry_ii((Entry_ii *)e->next);
    free(e);
}

Hash_ii *new_hash_ii(){
    return new_hash_ii2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_ii *new_hash_ii1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_ii2(size, DEFAULT_LOAD_FACTOR);
}

Hash_ii *new_hash_ii2(int32_t size, double load_factor){
    Hash_ii *res=malloc(sizeof(Hash_ii));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_ii *));
    res->pairs=new_llist_l();
    return res;
}

Entry_ii *hash_ii_get(Hash_ii *hp, int key){
    int32_t tmpHash=hash_int(key);
    Entry_ii *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_ii *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_ii_put(Hash_ii *hp, int key, int value){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ii *e=hp->table[i];

    for(;e!=NULL;e=(Entry_ii *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_ii));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_ii_resize(hp, hp->tableSize*2);
}

static void hash_ii_resize(Hash_ii *hp, int32_t newCapacity){
    Entry_ii **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_ii **newTable=(Entry_ii **)calloc(hp->tableSize, sizeof(Entry_ii *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_ii *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_ii *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_ii_remove(Hash_ii *hp, int key){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ii *e=hp->table[i];
    Entry_ii *pre=NULL;

    for(;e!=NULL;e=(Entry_ii *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_ii *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_ii_clear(Hash_ii *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_ii(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_ii(Hash_ii *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_ii(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_str_double

static void free_entry_sd(Entry_sd *e){
    if(!e) return;
    free_entry_sd((Entry_sd *)e->next);
    free(e->key);
    free(e);
}

Hash_sd *new_hash_sd(){
    return new_hash_sd2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_sd *new_hash_sd1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_sd2(size, DEFAULT_LOAD_FACTOR);
}

Hash_sd *new_hash_sd2(int32_t size, double load_factor){
    Hash_sd *res=malloc(sizeof(Hash_sd));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_sd *));
    res->pairs=new_llist_l();
    return res;
}

Entry_sd *hash_sd_get(Hash_sd *hp, char *key){
    int32_t tmpHash=hash_str(key);
    Entry_sd *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_sd *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_sd_put(Hash_sd *hp, char *key, double value){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_sd *e=hp->table[i];

    for(;e!=NULL;e=(Entry_sd *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_sd));
    e->hash=tmpHash;
    e->key=util_str_deep_copy(key);
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_sd_resize(hp, hp->tableSize*2);
}

static void hash_sd_resize(Hash_sd *hp, int32_t newCapacity){
    Entry_sd **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_sd **newTable=(Entry_sd **)calloc(hp->tableSize, sizeof(Entry_sd *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_sd *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_sd *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_sd_remove(Hash_sd *hp, char *key){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_sd *e=hp->table[i];
    Entry_sd *pre=NULL;

    for(;e!=NULL;e=(Entry_sd *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_sd *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_sd_clear(Hash_sd *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_sd(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_sd(Hash_sd *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_sd(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_str_int

static void free_entry_si(Entry_si *e){
    if(!e) return;
    free_entry_si((Entry_si *)e->next);
    free(e->key);
    free(e);
}

Hash_si *new_hash_si(){
    return new_hash_si2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_si *new_hash_si1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_si2(size, DEFAULT_LOAD_FACTOR);
}

Hash_si *new_hash_si2(int32_t size, double load_factor){
    Hash_si *res=malloc(sizeof(Hash_si));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_si *));
    res->pairs=new_llist_l();
    return res;
}

Entry_si *hash_si_get(Hash_si *hp, char *key){
    int32_t tmpHash=hash_str(key);
    Entry_si *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_si *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_si_put(Hash_si *hp, char *key, int value){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_si *e=hp->table[i];

    for(;e!=NULL;e=(Entry_si *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_si));
    e->hash=tmpHash;
    e->key=util_str_deep_copy(key);
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_si_resize(hp, hp->tableSize*2);
}

static void hash_si_resize(Hash_si *hp, int32_t newCapacity){
    Entry_si **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_si **newTable=(Entry_si **)calloc(hp->tableSize, sizeof(Entry_si *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_si *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_si *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_si_remove(Hash_si *hp, char *key){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_si *e=hp->table[i];
    Entry_si *pre=NULL;

    for(;e!=NULL;e=(Entry_si *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_si *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_si_clear(Hash_si *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_si(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_si(Hash_si *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_si(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_str_long

static void free_entry_sl(Entry_sl *e){
    if(!e) return;
    free_entry_sl((Entry_sl *)e->next);
    free(e->key);
    free(e);
}

Hash_sl *new_hash_sl(){
    return new_hash_sl2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_sl *new_hash_sl1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_sl2(size, DEFAULT_LOAD_FACTOR);
}

Hash_sl *new_hash_sl2(int32_t size, double load_factor){
    Hash_sl *res=malloc(sizeof(Hash_sl));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_sl *));
    res->pairs=new_llist_l();
    return res;
}

Entry_sl *hash_sl_get(Hash_sl *hp, char *key){
    int32_t tmpHash=hash_str(key);
    Entry_sl *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_sl *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_sl_put(Hash_sl *hp, char *key, int64_t value){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_sl *e=hp->table[i];

    for(;e!=NULL;e=(Entry_sl *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_sl));
    e->hash=tmpHash;
    e->key=util_str_deep_copy(key);
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_sl_resize(hp, hp->tableSize*2);
}

static void hash_sl_resize(Hash_sl *hp, int32_t newCapacity){
    Entry_sl **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_sl **newTable=(Entry_sl **)calloc(hp->tableSize, sizeof(Entry_sl *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_sl *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_sl *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_sl_remove(Hash_sl *hp, char *key){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_sl *e=hp->table[i];
    Entry_sl *pre=NULL;

    for(;e!=NULL;e=(Entry_sl *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_sl *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_sl_clear(Hash_sl *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_sl(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_sl(Hash_sl *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_sl(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_void_double

static void free_entry_vd(Entry_vd *e, void(*free_key)(void *key)){
    if(!e) return;
    free_entry_vd((Entry_vd *)e->next, free_key);
    if(free_key) free_key(e->key);
    free(e);
}

Hash_vd *new_hash_vd(){
    return new_hash_vd2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_vd *new_hash_vd1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_vd2(size, DEFAULT_LOAD_FACTOR);
}

Hash_vd *new_hash_vd2(int32_t size, double load_factor){
    Hash_vd *res=malloc(sizeof(Hash_vd));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_vd *));
    res->pairs=new_llist_l();
    return res;
}

Entry_vd *hash_vd_get(Hash_vd *hp, void *key, int hash, int(*compare)(void *a, void *b)){
    Entry_vd *e=hp->table[indexFor(hash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_vd *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_vd_put(Hash_vd *hp, void *key, int hash, double value, int(*compare)(void *a, void *b)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vd *e=hp->table[i];

    for(;e!=NULL;e=(Entry_vd *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_vd));
    e->hash=hash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_vd_resize(hp, hp->tableSize*2);
}

static void hash_vd_resize(Hash_vd *hp, int32_t newCapacity){
    Entry_vd **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_vd **newTable=(Entry_vd **)calloc(hp->tableSize, sizeof(Entry_vd *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_vd *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_vd *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_vd_remove(Hash_vd *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vd *e=hp->table[i];
    Entry_vd *pre=NULL;

    for(;e!=NULL;e=(Entry_vd *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_vd *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            if(free_key) free_key(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_vd_clear(Hash_vd *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_vd(hp->table[i], free_key);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_vd(Hash_vd *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_vd(hp->table[i], free_key);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_void_int

static void free_entry_vi(Entry_vi *e, void(*free_key)(void *key)){
    if(!e) return;
    free_entry_vi((Entry_vi *)e->next, free_key);
    if(free_key) free_key(e->key);
    free(e);
}

Hash_vi *new_hash_vi(){
    return new_hash_vi2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_vi *new_hash_vi1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_vi2(size, DEFAULT_LOAD_FACTOR);
}

Hash_vi *new_hash_vi2(int32_t size, double load_factor){
    Hash_vi *res=malloc(sizeof(Hash_vi));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_vi *));
    res->pairs=new_llist_l();
    return res;
}

Entry_vi *hash_vi_get(Hash_vi *hp, void *key, int hash, int(*compare)(void *a, void *b)){
    Entry_vi *e=hp->table[indexFor(hash, hp->tableSize)];
    for(;e;e=e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_vi_put(Hash_vi *hp, void *key, int hash, int value, int(*compare)(void *a, void *b)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vi *e=hp->table[i];

    for(;e;e=e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_vi));
    e->hash=hash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_vi_resize(hp, hp->tableSize*2);
}

static void hash_vi_resize(Hash_vi *hp, int32_t newCapacity){
    Entry_vi **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_vi **newTable=(Entry_vi **)calloc(hp->tableSize, sizeof(Entry_vi *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_vi *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_vi *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_vi_remove(Hash_vi *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vi *e=hp->table[i];
    Entry_vi *pre=NULL;

    for(;e!=NULL;e=(Entry_vi *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_vi *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            if(free_key) free_key(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_vi_clear(Hash_vi *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_vi(hp->table[i], free_key);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_vi(Hash_vi *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_vi(hp->table[i], free_key);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_void_long

static void free_entry_vl(Entry_vl *e, void(*free_key)(void *key)){
    if(!e) return;
    free_entry_vl((Entry_vl *)e->next, free_key);
    if(free_key) free_key(e->key);
    free(e);
}

Hash_vl *new_hash_vl(){
    return new_hash_vl2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

Hash_vl *new_hash_vl1(int32_t size){
    int n=size/DEFAULT_INITIAL_CAPACITY;
    size=DEFAULT_INITIAL_CAPACITY;
    while(n=n>>1) size<<1;
    return new_hash_vl2(size, DEFAULT_LOAD_FACTOR);
}

Hash_vl *new_hash_vl2(int32_t size, double load_factor){
    Hash_vl *res=malloc(sizeof(Hash_vl));
    res->tableSize=size;
    res->size=0;
    res->threshold=(int32_t)(size*load_factor);
    res->table=calloc(size, sizeof(Entry_vl *));
    res->pairs=new_llist_l();
    return res;
}

Entry_vl *hash_vl_get(Hash_vl *hp, void *key, int hash, int(*compare)(void *a, void *b)){
    Entry_vl *e=hp->table[indexFor(hash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_vl *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_vl_put(Hash_vl *hp, void *key, int hash, int64_t value, int(*compare)(void *a, void *b)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vl *e=hp->table[i];

    for(;e!=NULL;e=(Entry_vl *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_vl));
    e->hash=hash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_vl_resize(hp, hp->tableSize*2);
}

static void hash_vl_resize(Hash_vl *hp, int32_t newCapacity){
    Entry_vl **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_vl **newTable=(Entry_vl **)calloc(hp->tableSize, sizeof(Entry_vl *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_vl *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_vl *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_vl_remove(Hash_vl *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vl *e=hp->table[i];
    Entry_vl *pre=NULL;

    for(;e!=NULL;e=(Entry_vl *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_vl *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            if(free_key) free_key(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_vl_clear(Hash_vl *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_vl(hp->table[i], free_key);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_vl(Hash_vl *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_vl(hp->table[i], free_key);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== arrayList_str

AList_str *new_alist_str(int64_t initialCapacity){
    AList_str *res=(AList_str *)malloc(sizeof(AList_str));
    res->tableSize=initialCapacity;
    res->size=0;
    res->elementData=(char **)calloc(initialCapacity, sizeof(char *));
    return res;
}

void alist_str_add(AList_str *list, char *str){
    alist_str_ensure(list, list->size+1);
    list->elementData[list->size]=(char *)malloc((strlen(str)+1)*sizeof(char));
    strcpy(list->elementData[list->size], str);
    list->size++;
}

void alist_str_add_with_no_copy(AList_str *list, char *str){
    alist_str_ensure(list, list->size+1);
    list->elementData[list->size]=str;
    list->size++;
}

static void alist_str_ensure(AList_str *list, int64_t minCapacity){
    if(minCapacity>list->tableSize){
        int64_t newCapacity=list->tableSize*3/2+1;
        list->tableSize=newCapacity;
        list->elementData=(char **)realloc(list->elementData, newCapacity*sizeof(char *));
    }
}

void free_alist_str(AList_str *list){
    int i;
    for(i=0;i<list->size;i++){
        free(list->elementData[i]);
    }
    free(list->elementData);
    free(list);
}

//=============== arrayList_mem

AList_mem *new_alist_mem(int64_t initialCapacity){
    AList_mem *res=(AList_mem *)malloc(sizeof(AList_mem));
    res->tableSize=initialCapacity;
    res->size=0;
    res->elementData=(void **)calloc(initialCapacity, sizeof(void *));
    res->elementLength=(int64_t *)calloc(initialCapacity, sizeof(int64_t));
    return res;
}

void alist_mem_add(AList_mem *list, void *data, int64_t size){
    alist_mem_ensure(list, list->size+1);
    list->elementData[list->size]=(void *)calloc(size+1, 1);
    memcpy(list->elementData[list->size], data, size);
    list->elementLength[list->size]=size;
    list->size++;
}

static void alist_mem_ensure(AList_mem *list, int64_t minCapacity){
    if(minCapacity>list->tableSize){
        int64_t newCapacity=list->tableSize*3/2+1;
        list->tableSize=newCapacity;
        list->elementData=(void **)realloc(list->elementData, newCapacity*sizeof(void *));
        list->elementLength=(int64_t *)realloc(list->elementLength, newCapacity*sizeof(int64_t));
    }
}

void free_alist_mem(AList_mem *list){
    int i;
    for(i=0;i<list->size;i++){
        free(list->elementData[i]);
    }
    free(list->elementData);
    free(list->elementLength);
    free(list);
}

//=============== arrayList_int

AList_i *new_alist_i(int64_t initialCapacity){
    AList_i *res=(AList_i *)malloc(sizeof(AList_i));
    res->tableSize=initialCapacity;
    res->size=0;
    res->elementData=(int *)calloc(initialCapacity, sizeof(int));
    return res;
}

AList_i *alist_i_deep_copy(AList_i *list){
    AList_i *res=(AList_i *)malloc(sizeof(AList_i));
    res->tableSize=list->tableSize;
    res->size=list->size;
    res->elementData=(int *)malloc(list->tableSize*sizeof(int));
    memcpy(res->elementData, list->elementData, list->size*sizeof(int));
    return res;
}

inline void alist_i_add(AList_i *list, int value){
    alist_i_ensure(list, list->size+1);
    list->elementData[list->size++]=value;
}

static void alist_i_ensure(AList_i *list, int64_t minCapacity){
    if(minCapacity>list->tableSize){
        int64_t newCapacity=list->tableSize*3/2+1;
        list->tableSize=newCapacity;
        list->elementData=(int *)realloc(list->elementData, newCapacity*sizeof(int));
    }
}

void free_alist_i(AList_i *list){
    free(list->elementData);
    free(list);
}

//=============== arrayList_long

AList_l *new_alist_l(int64_t initialCapacity){
    AList_l *res=(AList_l *)malloc(sizeof(AList_l));
    res->tableSize=initialCapacity;
    res->size=0;
    res->elementData=(int64_t *)calloc(initialCapacity, sizeof(int64_t));
    return res;
}

AList_l *alist_l_deep_copy(AList_l *list){
    AList_l *res=(AList_l *)malloc(sizeof(AList_l));
    res->tableSize=list->tableSize;
    res->size=list->size;
    res->elementData=(int64_t *)malloc(list->tableSize*sizeof(int64_t));
    memcpy(res->elementData, list->elementData, list->size*sizeof(int64_t));
    return res;
}

inline void alist_l_add(AList_l *list, int64_t value){
    alist_l_ensure(list, list->size+1);
    list->elementData[list->size++]=value;
}

static void alist_l_ensure(AList_l *list, int64_t minCapacity){
    if(minCapacity>list->tableSize){
        int64_t newCapacity=list->tableSize*3/2+1;
        list->tableSize=newCapacity;
        list->elementData=(int64_t *)realloc(list->elementData, newCapacity*sizeof(int64_t));
    }
}

void free_alist_l(AList_l *list){
    free(list->elementData);
    free(list);
}

//=============== arrayList_double

AList_d *new_alist_d(int64_t initialCapacity){
    AList_d *res=(AList_d *)malloc(sizeof(AList_d));
    res->tableSize=initialCapacity;
    res->size=0;
    res->elementData=(double *)calloc(initialCapacity, sizeof(double));
    return res;
}

AList_d *alist_d_deep_copy(AList_d *list){
    AList_d *res=(AList_d *)malloc(sizeof(AList_d));
    res->tableSize=list->tableSize;
    res->size=list->size;
    res->elementData=(double *)malloc(list->tableSize*sizeof(double));
    memcpy(res->elementData, list->elementData, list->size*sizeof(double));
    return res;
}

inline void alist_d_add(AList_d *list, double value){
    alist_d_ensure(list, list->size+1);
    list->elementData[list->size++]=value;
}

static void alist_d_ensure(AList_d *list, int64_t minCapacity){
    if(minCapacity>list->tableSize){
        int64_t newCapacity=list->tableSize*3/2+1;
        list->tableSize=newCapacity;
        list->elementData=(double *)realloc(list->elementData, newCapacity*sizeof(double));
    }
}

void free_alist_d(AList_d *list){
    free(list->elementData);
    free(list);
}

//=============== string_builder

SBuilder *new_s_builder(int64_t capacity){
    SBuilder *res=(SBuilder *)malloc(sizeof(SBuilder));
    res->tableSize=capacity;
    res->size=0;
    res->str=(char *)malloc(capacity*sizeof(char));
    res->str[0]='\0';
    return res;
}

SBuilder *s_builder_deep_copy(SBuilder *sb){
    SBuilder *res=(SBuilder *)malloc(sizeof(SBuilder));
    res->tableSize=sb->tableSize;
    res->size=sb->size;
    res->str=(char *)malloc((sb->tableSize)*sizeof(char));
    int i;
    for(i=0;i<sb->size;i++){
        res->str[i]=sb->str[i];
    }
    res->str[i]='\0';
    return res;
}

void s_builder_add_char(SBuilder *sb, char c){
    s_builder_ensure(sb, sb->size+2);
    sb->str[sb->size++]=c;
    sb->str[sb->size]='\0';
}

void s_builder_add_int(SBuilder *sb, int64_t v){
    char str[20];
    int len=sprintf(str, "%lld", v);
    s_builder_ensure(sb, sb->size+len+1);
    memcpy(sb->str+sb->size, str, len*sizeof(char));
    sb->size+=len;
    sb->str[sb->size]='\0';
}

void s_builder_add_double(SBuilder *sb, double v){
    char str[100];
    int len=sprintf(str, "%f", v);
    s_builder_ensure(sb, sb->size+len+1);
    memcpy(sb->str+sb->size, str, len*sizeof(char));
    sb->size+=len;
    sb->str[sb->size]='\0';
}

void s_builder_add_str(SBuilder *sb, char *str){
    int len=strlen(str);
    s_builder_ensure(sb, sb->size+len+1);
    memcpy(sb->str+sb->size, str, len*sizeof(char));
    sb->size+=len;
    sb->str[sb->size]='\0';
}

void s_builder_add_str_with_len(SBuilder *sb, char *str, int len){
    s_builder_ensure(sb, sb->size+len+1);
    memcpy(sb->str+sb->size, str, len*sizeof(char));
    sb->size+=len;
    sb->str[sb->size]='\0';
}

void s_builder_add_str_with_len2(SBuilder *sb, char *str, int len){
    sb->tableSize=sb->size+len+1;

    sb->str=(char *)realloc(sb->str, sb->tableSize*sizeof(char));
    memcpy(sb->str+sb->size, str, len*sizeof(char));

    sb->size+=len;
    sb->str[sb->size]='\0';
}

void s_builder_compress(SBuilder *sb){
    if(!sb) return;
    sb->tableSize=sb->size+1;
    sb->str=realloc(sb->str, sb->tableSize);
}

void s_builder_ensure(SBuilder *sb, int64_t minCapacity){
    if(minCapacity>sb->tableSize){
        int64_t newCapacity=minCapacity*3/2+1;
        sb->tableSize=newCapacity;
        sb->str=(char *)realloc(sb->str, newCapacity*sizeof(char));
    }
}

void free_s_builder(SBuilder *sb){
    free(sb->str);
    free(sb);
}

#ifdef _linux

//=============== queue

Queue *new_queue(int size){
    Queue *res=(Queue *)malloc(sizeof(Queue));
    res->size=size;
    res->head=0;
    res->tail=0;
    res->count=0;
    res->array=(void **)calloc(size, sizeof(void *));
    res->is_finished=0;
    pthread_mutex_init(&(res->locker), NULL);
    pthread_cond_init(&(res->cond_put), NULL);
    pthread_cond_init(&(res->cond_get), NULL);
    return res;
}

void queue_put(Queue *q, void *data){
    pthread_mutex_lock(&(q->locker));
    if(q->is_finished){
        pthread_mutex_unlock(&(q->locker));
        fprintf(stderr, "Error: queue is finished, cannot put data!\n");
        exit(0);
    }
    while(q->count>=q->size){
        pthread_cond_wait(&(q->cond_get), &(q->locker));
    }
    q->count++;
    q->array[q->head]=data;
    q->head=(q->head+1)%(q->size);
    pthread_cond_signal(&(q->cond_put));
    pthread_mutex_unlock(&(q->locker));
}

void queue_set_finished(Queue *q){
    pthread_mutex_lock(&(q->locker));
    q->is_finished=1;
    pthread_cond_broadcast(&(q->cond_put));
    pthread_mutex_unlock(&(q->locker));
}

void *queue_get(Queue *q){
    pthread_mutex_lock(&(q->locker));
    while(q->count<=0){
        if(q->is_finished){
            pthread_mutex_unlock(&(q->locker));
            return NULL;
        }else{
            pthread_cond_wait(&(q->cond_put), &(q->locker));
        }
    }
    q->count--;
    void *res=q->array[q->tail];
    q->array[q->tail]=NULL;
    q->tail=(q->tail+1)%(q->size);
    pthread_cond_signal(&(q->cond_get));
    pthread_mutex_unlock(&(q->locker));
    return res;
}

void free_queue(Queue *q){
    free(q->array);
    pthread_mutex_destroy(&(q->locker));
    pthread_cond_destroy(&(q->cond_put));
    pthread_cond_destroy(&(q->cond_get));
    free(q);
}

//===============

OrderQueue *new_order_queue(int size){
    OrderQueue *res=(OrderQueue *)malloc(sizeof(OrderQueue));
    res->size=size;
    res->head=0;
    res->tail=0;
    res->count=0;
    res->index=0;
    res->array=(void **)calloc(size, sizeof(void *));
    res->is_finished=0;
    pthread_mutex_init(&(res->locker), NULL);
    pthread_cond_init(&(res->cond_put), NULL);
    pthread_cond_init(&(res->cond_get), NULL);
    return res;
}

void order_queue_put(OrderQueue *q, void *data, int index){
    pthread_mutex_lock(&(q->locker));
    if(q->is_finished){
        pthread_mutex_unlock(&(q->locker));
        fprintf(stderr, "Error: queue is finished, cannot put data!\n");
        exit(0);
    }
    while(index>q->index || q->count>=q->size){
        pthread_cond_wait(&(q->cond_get), &(q->locker));
    }
    q->count++;
    q->index++;
    q->array[q->head]=data;
    q->head=(q->head+1)%(q->size);
    pthread_cond_signal(&(q->cond_put));
    pthread_cond_broadcast(&(q->cond_get));
    pthread_mutex_unlock(&(q->locker));
}

void order_queue_set_finished(OrderQueue *q){
    pthread_mutex_lock(&(q->locker));
    q->is_finished=1;
    pthread_mutex_unlock(&(q->locker));
}

void *order_queue_get(OrderQueue *q){
    pthread_mutex_lock(&(q->locker));
    while(q->count<=0){
        if(q->is_finished){
            pthread_mutex_unlock(&(q->locker));
            return NULL;
        }else{
            pthread_cond_wait(&(q->cond_put), &(q->locker));
        }
    }
    q->count--;
    void *res=q->array[q->tail];
    q->array[q->tail]=NULL;
    q->tail=(q->tail+1)%(q->size);
    pthread_cond_broadcast(&(q->cond_get));
    pthread_mutex_unlock(&(q->locker));
    return res;
}

void free_order_queue(OrderQueue *q){
    free(q->array);
    pthread_mutex_destroy(&(q->locker));
    pthread_cond_destroy(&(q->cond_put));
    pthread_cond_destroy(&(q->cond_get));
    free(q);
}

//=============== array_deque_long

ADeque_l *new_adeque_l(int64_t initialSize){
    ADeque_l *q=(ADeque_l *)calloc(1, sizeof(ADeque_l));
    q->elements=(int64_t *)calloc(initialSize, sizeof(int64_t));
    q->l_elements=initialSize;
    return q;
}

static void adeque_l_double_capacity(ADeque_l *q){
    int64_t p=q->head;
    int64_t n=q->l_elements;
    int64_t r=n-p;
    int64_t newCapacity=n<<2;
    if(newCapacity<0){
        fprintf(stderr, "Sorry, deque too big\n");
        exit(0);
    }
    int64_t *a=calloc(newCapacity, sizeof(int64_t));
    memcpy(a, q->elements+p, r*sizeof(int64_t));
    memcpy(a+r, q->elements, p*sizeof(int64_t));
    free(q->elements);
    q->elements=a;
    q->l_elements=newCapacity;
    q->head=0;
    q->tail=n;
}

void adeque_l_add_first(ADeque_l *q, int64_t value){
    int64_t *p=(int64_t *)malloc(sizeof(int64_t));
    *(p)=value;
    q->elements[q->head = (q->head - 1) & (q->l_elements-1)] = (int64_t)p;
    if(q->head==q->tail) adeque_l_double_capacity(q);
}

void adeque_l_add_last(ADeque_l *q, int64_t value){
    int64_t *p=(int64_t *)malloc(sizeof(int64_t));
    *(p)=value;
    q->elements[q->tail]=(int64_t)p;
    if ( (q->tail = (q->tail + 1) & (q->l_elements-1)) == q->head) adeque_l_double_capacity(q);
}

int64_t *adeque_l_poll_first(ADeque_l *q){
    int64_t h=q->head;
    int64_t *result=(int64_t *)(q->elements[h]);
    if(!result) return NULL;
    q->elements[h]=(int64_t)NULL;
    q->head = (h + 1) & (q->l_elements - 1);
    return result;
}

int64_t *adeque_l_poll_last(ADeque_l *q){
    int64_t t=(q->tail-1)&(q->l_elements-1);
    int64_t *result=(int64_t *)(q->elements[t]);
    if(!result) return NULL;
    q->elements[t]=(int64_t)NULL;
    q->tail=t;
    return result;
}

int64_t *adeque_l_peek_first(ADeque_l *q){
    return (int64_t *)(q->elements[q->head]);
}

int64_t *adeque_l_peek_last(ADeque_l *q){
    return (int64_t *)(q->elements[(q->tail-1)&(q->l_elements-1)]);
}

void adeque_l_add(ADeque_l *q, int64_t value){
    adeque_l_add_last(q, value);
}

int64_t *adeque_l_poll(ADeque_l *q){
    return adeque_l_poll_first(q);
}

int64_t *adeque_l_peek(ADeque_l *q){
    return adeque_l_peek_first(q);
}

void free_adeque_l(ADeque_l *q){
    int i;
    for(i=0;i<q->l_elements;i++){
        int64_t *p=(int64_t *)q->elements[i];
        if(p) free(p);
    }
    free(q->elements);
    free(q);
}

//=============== PriorityQueue

PriorityQueue *new_priority_queue(int capacity, int (*compare)(void *, void *)){
    PriorityQueue *q=malloc(sizeof(PriorityQueue));
    q->capacity=capacity;
    q->queue=(void **)calloc(capacity, sizeof(void *));
    q->compare=compare;
    q->size=0;
    return q;
}

int priority_queue_add(PriorityQueue *q, void *value){
    int i=q->size;
    if(i>=q->capacity){
        int oldCapacity=q->capacity;
        int newCapacity = oldCapacity + ((oldCapacity < 64) ? (oldCapacity + 2) : (oldCapacity >> 1));
        q->capacity=newCapacity;
        q->queue=realloc(q->queue, newCapacity*sizeof(void *));
        memset(q->queue+oldCapacity, 0, (newCapacity-oldCapacity)*sizeof(void *));
    }
    q->size=i+1;
    if(i==0) q->queue[0]=value;
    else{
        while(i>0){
            int parent=(i-1)>>1;
            void *e=q->queue[parent];
            if(q->compare(value, e)>=0) break;
            q->queue[i] =e;
            i=parent;
        }
        q->queue[i]=value;
    }
}

void *priority_queue_poll(PriorityQueue *q){
    if(q->size==0) return NULL;
    int s=--q->size;
    void *result=q->queue[0];
    void *x=q->queue[s];
    q->queue[s]=NULL;
    if(s!=0){
        int k=0;
        int half=q->size>>1;
        while(k<half) {
            int child=(k<<1)+1;
            void *c=q->queue[child];
            int right=child+1;
            if (right<q->size && q->compare(c, q->queue[right])>0) c=q->queue[child=right];
            if (q->compare(x, c)<=0) break;
            q->queue[k]=c;
            k=child;
        }
        q->queue[k] = x;
    }
    return result;
}

void free_priority_queue(PriorityQueue *q, void (*myfree)(void *)){
    if(myfree){
        int i;
        for(i=0;i<q->size;i++){
            void *x=q->queue[i];
            if(x) myfree(x);
        }
    }
    free(q->queue);
    free(q);
}

//=============== thread pool

pthread_t my_threads_array[50000];
int my_threads_array_index=0;

void split_span_for_threads(int thread_num, int64_t start, int64_t end){
    int64_t sp=(end-start)/(int64_t)thread_num;
    int64_t last=(end-start)%(int64_t)thread_num;
    int64_t s, e=start;
    int t;
    for(t=0;t<thread_num;t++){
        s=e;
        e=s+sp;
        if(t<last) e++;
        starts_thread[t]=s;
        ends_thread[t]=e;
    }
}

void go_run_thread(void *(*func)(void *arg), void *arg){
    int stat=pthread_create(my_threads_array+my_threads_array_index++, NULL, func, arg);
    if(stat!=0){
        fprintf(stderr, "create thread error, state=%d\n", stat);
        exit(0);
    }
}

void wg_wait_thread(){
    int i;
    for(i=0;i<my_threads_array_index;i++) pthread_join(my_threads_array[i], NULL);
    my_threads_array_index=0;
}

ThreadPool *new_thread_pool(int pool_thread_num){
    ThreadPool *p=(ThreadPool *)calloc(1, sizeof(ThreadPool));

    int i;

    p->thread_num=pool_thread_num;
    p->is_wait=calloc(pool_thread_num, sizeof(int));
    for(i=0;i<pool_thread_num;i++) p->is_wait[i]=1;
    //--
    pthread_mutex_init(&(p->locker), NULL);
    pthread_cond_init(&(p->cond), NULL);
    pthread_cond_init(&(p->cond2), NULL);
    //--
    p->threads=(pthread_t *)malloc(pool_thread_num*sizeof(pthread_t));
    for(i=0;i<pool_thread_num;i++){
        char *para=(char *)malloc(100*sizeof(char));
        sprintf(para, "%d\t%lld", i, p);
        int stat=pthread_create(p->threads+i, NULL, thread_routine, (void *)para);
        if(stat!=0){
            fprintf(stderr, "create thread error, state=%d\n", stat);
            free(para);
            exit(0);
        }
    }

    return p;
}

static void *thread_routine(void *args){
    char *para=(char *)args;

    int i=0;
    while(para[++i]!='\t');
    para[i]='\0';
    int threadIndex=atoi(para);
    ThreadPool *p=(ThreadPool *)atoll(para+i+1);
    para[i]='\t';
    free(para);

    while(1){
        pthread_mutex_lock(&(p->locker));
        if(p->is_wait[threadIndex]) pthread_cond_wait(&(p->cond), &(p->locker));
        if(p->is_finished){
            pthread_mutex_unlock(&(p->locker));
            break;
        }
        pthread_mutex_unlock(&(p->locker));

        while(1){
            ThreadWorker *w=NULL;
            pthread_mutex_lock(&(p->locker));
            w=p->head;
            if(w) p->head=w->next;
            pthread_mutex_unlock(&(p->locker));
            if(!w) break;
            (*(w->process))(w->arg);
            //(*(w->process))((((int64_t)threadIndex)<<32)|((int64_t)(w->arg)));
            free(w);
        }

        pthread_mutex_lock(&(p->locker));
        p->is_wait[threadIndex]=1;
        p->fininshed_num++;
        if(p->fininshed_num==p->thread_num) pthread_cond_signal(&(p->cond2));
        pthread_mutex_unlock(&(p->locker));
    }
}

void thread_pool_add_worker(ThreadPool *p, void *(*func)(void *arg), void *arg){
    ThreadWorker *w=(ThreadWorker *)calloc(1, sizeof(ThreadWorker));
    w->process=func;
    w->arg=arg;
    w->next=p->head;
    p->head=w;
}

void thread_pool_invoke_all(ThreadPool *p){
    pthread_mutex_lock(&(p->locker));
    //--
    p->fininshed_num=0;
    memset(p->is_wait, 0, p->thread_num*sizeof(int));
    pthread_cond_broadcast(&(p->cond));
    pthread_cond_wait(&(p->cond2), &(p->locker));
    //--
    pthread_mutex_unlock(&(p->locker));
}

void free_thread_pool(ThreadPool *p){
    pthread_mutex_lock(&(p->locker));
    //--
    p->is_finished=1;
    p->fininshed_num=0;
    memset(p->is_wait, 0, p->thread_num*sizeof(int));
    pthread_cond_broadcast(&(p->cond));
    //--
    pthread_mutex_unlock(&(p->locker));
    //--
    int i;
    for(i=0;i<p->thread_num;i++) pthread_join(p->threads[i], NULL);
    //--
    pthread_mutex_destroy(&(p->locker));
    pthread_cond_destroy(&(p->cond));
    pthread_cond_destroy(&(p->cond2));
    free(p->is_wait);
    free(p->threads);
    free(p);
}

//=============== file

AList_l *get_file_list(char *path){
    DIR *dir=opendir(path);
    if(dir==NULL) return NULL;

    int l_path=strlen(path);

    AList_l *list=new_alist_l(16);

    struct dirent *ptr;
    while((ptr=readdir(dir))!=NULL){
        char *name=ptr->d_name;
        int l_name=strlen(name);
        //--
        if(strcmp(name, ".")==0 || strcmp(name, "..")==0) continue;
        else{
            if(path[l_path-1]=='/'){
                char *file=calloc(l_path+l_name+1, sizeof(char));
                memcpy(file, path, l_path*sizeof(char));
                memcpy(file+l_path, name, l_name*sizeof(char));
                alist_l_add(list, file);
            }else{
                char *file=calloc(l_path+l_name+2, sizeof(char));
                memcpy(file, path, l_path*sizeof(char));
                file[l_path]='/';
                memcpy(file+l_path+1, name, l_name*sizeof(char));
                alist_l_add(list, file);
            }
        }
    }

    closedir(dir);

    return list;
}

//return type: 1:file, 2:directory, 0:error
int get_file_type(char *path){
	struct stat s_buf;
 	stat(path,&s_buf);

	if(S_ISREG(s_buf.st_mode)) return 1;
    if(S_ISDIR(s_buf.st_mode)) return 2;

 	return 0;
}

Pair *split_file_size(char *file, int num){
    Pair *res=calloc(num, sizeof(Pair));

    FILE *fp=fopen(file, "r");
    if(fp==NULL){fprintf(stderr, "open file(%s)failed, please check!\n");exit(0);}

    fseek(fp, 0L, SEEK_END);
    int64_t filesize = ftell(fp);

    int64_t i, sp=filesize/(int64_t)num;

    if(sp<1000000){
        res->left=0;
        res->right=filesize;
    }else{
        for(i=0;i<(int64_t)num;i++){
            fseek(fp, (i+1)*sp, SEEK_SET);
            while(1){
                if(fgetc(fp)=='\n') break;
                fseek(fp, -2, SEEK_CUR);
            }
            res[i].left= i==0? 0:res[i-1].right;
            res[i].right=i==(num-1) ? filesize:ftell(fp);
        }
    }

    fclose(fp);

    return res;
}

#else

//=============== File operation

int util_str_starts(char *str, char *sub){
	int i=0;
	while(1){
		char c1=str[i];
		char c2=sub[i];
		if(c2=='\0') return 1;
		if(c1!=c2) return 0;
		i++;
	}
}

int util_str_ends(char *str, char *sub){
    int i=strlen(str)-1;
    int j=strlen(sub)-1;
    if(i<j) return 0;
    for(;i>=0&&j>=0;i--,j--){
        if(str[i]!=sub[j]) return 0;
    }
    return 1;
}

void get_all_files_with_prefix(char *path, char *prefix, AList_l *files){
    struct _finddata_t fa;
    long handle;

    char *tmp1=calloc(200, sizeof(char));
    sprintf(tmp1, "%s/*", path);

    if((handle = _findfirst(tmp1,&fa)) == -1L) return;

    do{
        if(strcmp(fa.name, ".")==0 || strcmp(fa.name, "..")==0) continue;
        if(fa.attrib&_A_SUBDIR){
            char *tmp2=calloc(200, sizeof(char));
            sprintf(tmp2, "%s/%s", path, fa.name);
            get_all_files_with_prefix(tmp2, prefix, files);
            free(tmp2);
        }else if(util_str_starts(fa.name, prefix)){
            char *tmp2=calloc(200, sizeof(char));
            sprintf(tmp2, "%s/%s", path, fa.name);
            alist_l_add(files, tmp2);
        }
    }while(_findnext(handle,&fa) == 0);

    free(tmp1);

    _findclose(handle);
}

void get_all_files_with_suffix(char *path, char *suffix, AList_l *files){
    struct _finddata_t fa;
    long handle;

    char *tmp1=calloc(200, sizeof(char));
    sprintf(tmp1, "%s/*", path);

    if((handle = _findfirst(tmp1,&fa)) == -1L) return;

    do{
        if(strcmp(fa.name, ".")==0 || strcmp(fa.name, "..")==0) continue;
        if(fa.attrib&_A_SUBDIR){
            char *tmp2=calloc(200, sizeof(char));
            sprintf(tmp2, "%s/%s", path, fa.name);
            get_all_files_with_suffix(tmp2, suffix, files);
            free(tmp2);
        }else if(util_str_ends(fa.name, suffix)){
            char *tmp2=calloc(200, sizeof(char));
            sprintf(tmp2, "%s/%s", path, fa.name);
            alist_l_add(files, tmp2);
        }
    }while(_findnext(handle,&fa) == 0);

    free(tmp1);

    _findclose(handle);
}

#endif

//=============== java random

MyRand* new_my_rand1() {
    MyRand* mr = calloc(1, sizeof(MyRand));
    int64_t seed = clock();
    mr->seed = (seed ^ (int64_t)0x5DEECE66D) & (((int64_t)1 << 48) - 1);
    return mr;
}

MyRand* new_my_rand2(int64_t seed) {
    MyRand* mr = calloc(1, sizeof(MyRand));
    mr->seed = (seed ^ (int64_t)0x5DEECE66D) & (((int64_t)1 << 48) - 1);
    return mr;
}

static int my_rand_next(MyRand *mr, int bits) {
    int64_t oldseed, nextseed;;
    while (1) {
        oldseed = mr->seed;
        nextseed = (oldseed * (int64_t)0x5DEECE66D + (int64_t)0xB) & (((int64_t)1 << 48) - 1);
        if (oldseed != nextseed) {
            mr->seed = nextseed;
            break;
        }
    }
    return (int)((uint64_t)nextseed >> (48 - bits));
}

int my_rand_next_int(MyRand *mr, int bound) {
    int r = my_rand_next(mr, 31);
    int m = bound - 1;
    if ((bound & m) == 0)  // i.e., bound is a power of 2
        r = (int)((bound * (int64_t)r) >> 31);
    else {
        int u;
        for (u = r;
            u - (r = u % bound) + m < 0;
            u = my_rand_next(mr, 31))
            ;
    }
    return r;
}

double my_rand_next_double(MyRand *mr) {
    int n1 = my_rand_next(mr, 26);
    int n2 = my_rand_next(mr, 27);
    return (((int64_t)(n1) << 27) + n2) * 0x1.0p-53;
}

double my_rand_next_gaussian(MyRand* mr) {
    if (mr->haveNextNextGaussian) {
        mr->haveNextNextGaussian = 0;
        return mr->nextNextGaussian;
    }else {
        double v1, v2, s;
        do {
            v1 = 2.0 * my_rand_next_double(mr) - 1.0; // between -1 and 1
            v2 = 2.0 * my_rand_next_double(mr) - 1.0; // between -1 and 1
            s = v1 * v1 + v2 * v2;
        } while (s >= 1 || s == 0);
        double multiplier = sqrt(-2.0 * log(s) / s);
        mr->nextNextGaussian = v2 * multiplier;
        mr->haveNextNextGaussian = 1;
        return v1 * multiplier;
    }
}

//===============





