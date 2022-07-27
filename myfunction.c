/**
 * myfunction.c
 *
 *  Created on: 2016-8-9
 *      Author: chilianjiang
 */

#include "myfunction.h"

unsigned char bam_nt16_table[256] = {
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	 1, 2, 4, 8, 15,15,15,15, 15,15,15,15, 15, 0 /*=*/,15,15,
	15, 1,14, 2, 13,15,15, 4, 11,15,15,12, 15, 3,15,15,
	15,15, 5, 6,  8,15, 7, 9, 15,10,15,15, 15,15,15,15,
	15, 1,14, 2, 13,15,15, 4, 11,15,15,12, 15, 3,15,15,
	15,15, 5, 6,  8,15, 7, 9, 15,10,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15
};

unsigned char bam_int_2_base[16]={'N', 'A', 'C', 'N', 'G', 'N', 'N', 'N', 'T', 'N', 'N', 'N', 'N', 'N', 'N', 'N'};

//==================== get hardware information

#ifdef _linux

char *get_uuid(){
    FILE *pipe=popen("cat /proc/sys/kernel/random/boot_id", "r");
    if(!pipe) return NULL;

    char buff[128];

    if(fgets(buff, 128, pipe)){
        int len=chmop(buff);
        char *res=my_new(len+1, sizeof(char));
        memcpy(res, buff, len*sizeof(char));
        pclose(pipe);
        return res;
    }

    return NULL;
}

char *get_processor_id(){
    int sockfd;
    struct ifreq tmp;

    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0) return NULL;

    memset(&tmp,0,sizeof(struct ifreq));
    strncpy(tmp.ifr_name,"eth0",sizeof(tmp.ifr_name)-1);
    if((ioctl(sockfd,SIOCGIFHWADDR, &tmp))<0) return NULL;

    char *mac_addr=calloc(30, sizeof(char));
    sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x",
            (unsigned char)tmp.ifr_hwaddr.sa_data[0],
            (unsigned char)tmp.ifr_hwaddr.sa_data[1],
            (unsigned char)tmp.ifr_hwaddr.sa_data[2],
            (unsigned char)tmp.ifr_hwaddr.sa_data[3],
            (unsigned char)tmp.ifr_hwaddr.sa_data[4],
            (unsigned char)tmp.ifr_hwaddr.sa_data[5]
            );
    close(sockfd);

    return mac_addr;
}

char *get_serial_number(){
    return NULL;
}

#else

char *get_uuid(){
    FILE* pipe=_popen("wmic csproduct get UUID", "r");
    if(!pipe) return NULL;

    char buff[128];

    while(fgets(buff, 128, pipe)){
        if(strncmp(buff, "UUID", 4)==0){
            if(fgets(buff, 128, pipe)==NULL){
                _pclose(pipe);
                return NULL;
            }else{
                int len=chmop(buff);
                char *res=my_new(len+1, sizeof(char));
                memcpy(res, buff, len*sizeof(char));
                _pclose(pipe);
                return res;
            }
        }
    }

    _pclose(pipe);
    return NULL;
}

char *get_processor_id(){
    FILE* pipe=_popen("wmic cpu get processorid", "r");
    if(!pipe) return NULL;

    char buff[128];

    while(fgets(buff, 128, pipe)){
        if(strncmp(buff, "ProcessorId", 11)==0){
            if(fgets(buff, 128, pipe)==NULL){
                _pclose(pipe);
                return NULL;
            }else{
                int len=chmop(buff);
                char *res=my_new(len+1, sizeof(char));
                memcpy(res, buff, len*sizeof(char));
                _pclose(pipe);
                return res;
            }
        }
    }

    _pclose(pipe);
    return NULL;
}

char *get_serial_number(){
    FILE* pipe=_popen("wmic bios get serialnumber", "r");
    if(!pipe) return NULL;

    char buff[128];

    while(fgets(buff, 128, pipe)){
        if(strncmp(buff, "SerialNumber", 12)==0){
            if(fgets(buff, 128, pipe)==NULL){
                _pclose(pipe);
                return NULL;
            }else{
                int len=chmop(buff);
                char *res=my_new(len+1, sizeof(char));
                memcpy(res, buff, len*sizeof(char));
                _pclose(pipe);
                return res;
            }
        }
    }

    _pclose(pipe);
    return NULL;
}

#endif // _linux

//==================== avx

void *calloc_align_ptr(int64_t size, int8_t align_bytes){
	void *base_ptr = my_new(size+align_bytes, 1);
	void *mem_ptr = (void *)(
                          ((int64_t)((int64_t)base_ptr+align_bytes-1))
                          &~
                          ((int64_t)(align_bytes-1))
                          );
	if(mem_ptr==base_ptr) mem_ptr=(int64_t)base_ptr+align_bytes;
	*((int8_t *)mem_ptr-1)= (int8_t)((int64_t)mem_ptr-(int64_t)base_ptr);
	return mem_ptr;
}

void free_align_ptr(void *ptr){
	free((void *)((int64_t)ptr-*((int8_t *)ptr-1)));
}

//==================== string

char *getTime(){
	char *res=(char *)my_new(200, sizeof(char));
	//--
	time_t now;
	struct tm *s;
	time(&now);
	s=localtime(&now);
	//--
	int mon_t=s->tm_mon+1;
	int day_t=s->tm_mday;
	int hour_t=s->tm_hour;
	int min_t=s->tm_min;
	int sec_t=s->tm_sec;
	char *mon=(char *)my_new(10, sizeof(char));
	char *day=(char *)my_new(10, sizeof(char));
	char *hour=(char *)my_new(10, sizeof(char));
	char *min=(char *)my_new(10, sizeof(char));
	char *sec=(char *)my_new(10, sizeof(char));
	if(mon_t<10) sprintf(mon, "0%d", mon_t);
	else sprintf(mon, "%d", mon_t);
	if(day_t<10) sprintf(day, "0%d", day_t);
	else sprintf(day, "%d", day_t);
	if(hour_t<10) sprintf(hour, "0%d", hour_t);
	else sprintf(hour, "%d", hour_t);
	if(min_t<10) sprintf(min, "0%d", min_t);
	else sprintf(min, "%d", min_t);
	if(sec_t<10) sprintf(sec, "0%d", sec_t);
	else sprintf(sec, "%d", sec_t);
	sprintf(res, "%d-%s-%s %s:%s:%s", s->tm_year+1900, mon, day, hour, min, sec);
	//--
	free(mon);
	free(day);
	free(hour);
	free(min);
	free(sec);
	return res;
}

void mylog(char *str){
	char *nowTime=getTime();
    fprintf(stdout, "%s %s\n", nowTime, str);
	free(nowTime);
    fflush(stdout);
}

void myerror(char *str){
	char *nowTime=getTime();
    fprintf(stderr, "%s %s\n", nowTime, str);
	free(nowTime);
    fflush(stderr);
}

double format_double(double d, int nBits){
    if(nBits<0) return d;
    else{
        double d1=pow(10.0, nBits);
        double d2=d>0 ? (0.5/d1):(-0.5/d1);
        int64_t l=(int64_t)((d+d2)*d1);
        return (double)l/d1;
    }
}

int64_t double_2_int64_by_union(double d){
    int64_t *res=(int64_t *)(&d);
    return *(res);
}

int is_integer(char *str){
    if(str[0]=='-') str++;
    char c;
    while((c=*(str++))){
        if(!isdigit(c)) return 0;
    }
    return 1;
}

int is_double(char *str){
    if(str[0]=='-') str++;

    if(str[0]=='i'){
        if(str[1]=='n'){
            if(str[2]=='f'){
                if(str[3]=='\0') return 1;
                else return 0;
            }else return 0;
        }else return 0;
    }else if(str[0]=='n'){
        if(str[1]=='a'){
            if(str[2]=='n'){
                if(str[3]=='\0') return 1;
                else return 0;
            }else return 0;
        }else return 0;
    }else if(str[0]=='e' || str[0]=='E') return 0;

    int dot_num=0, e_num=0;
    char c;
    while((c=*(str++))){
        if(isdigit(c)) continue;
        else if(c=='.') dot_num++;
        else if(c=='E' || c=='e') e_num++;
        else return 0;
    }

    if(dot_num>1 || e_num>1) return 0;
    return 1;
}

inline int is_digit(char *str){
    return is_integer(str) || is_double(str);
}


int chmop(char *str){
	int i=strlen(str)-1;
	for(;i>=0;i--){
		if(str[i]!='\n' && str[i]!=' ' && str[i]!='\t' && str[i]!='\r') break;
		str[i]='\0';
	}
	return i+1;
}

int chmop_with_len(char *str, int len){
	int i=len-1;
	for(;i>=0;i--){
		if(str[i]!='\n' && str[i]!=' ' && str[i]!='\t' && str[i]!='\r') break;
		str[i]='\0';
	}
	return i+1;
}

void freeArray(int len, void **array){
	int i;
	for(i=0;i<len;i++) free(array[i]);
	free(array);
}

char *str_copy_range_of(char *str, int l_str, int from, int to, int *resLen){
    int len=(to-from)<(l_str-from) ? (to-from):(l_str-from);
    char *res=my_new2((len+1)*sizeof(char));
    memcpy(res, str+from, len);
    res[len]='\0';
    if(resLen) *(resLen)=len;
    return res;
}

char *substring(char *str, int start, int end){
    int len=end-start;
    char *res=my_new2((len+1)*sizeof(char));
    memcpy(res, str+start, len);
    res[len]='\0';
    return res;
}

char *substring2(char *str, int64_t start, int64_t end){
        int64_t len=end-start+1;
        char *res=(char *)my_new2(len*sizeof(char));
        int64_t i=start,j=0;
        for(;i<end;i++,j++) res[j]=str[i];
        res[len-1]='\0';
        return res;
}

char *str_2_lower_no_copy(char *str){
    int i=-1;
    while(str[++i]) str[i]=tolower(str[i]);
    return str;
}

char *str_2_upper_no_copy(char *str){
    int i=-1;
    while(str[++i]) str[i]=toupper(str[i]);
    return str;
}

char* str_2_lower_no_copy_2(char *str, int len){
    int i=-1;
    while(++i<len) str[i]=tolower(str[i]);
    return str;
}

char* str_2_upper_no_copy_2(char *str, int len){
    int i=-1;
    while(++i<len) str[i]=toupper(str[i]);
    return str;
}

char *str_2_lower_with_copy(char *str){
    return str_2_lower_with_copy_2(str, strlen(str));
}

char *str_2_upper_with_copy(char *str){
    return str_2_upper_with_copy_2(str, strlen(str));
}

char* str_2_lower_with_copy_2(char *str, int len){
    char *newStr=(char *)my_new2((len+1)*sizeof(char));
    memcpy(newStr, str, len);
    newStr[len]='\0';
    return str_2_lower_no_copy_2(newStr, len);
}

char* str_2_upper_with_copy_2(char *str, int len){
    char *newStr=(char *)my_new2((len+1)*sizeof(char));
    memcpy(newStr, str, len);
    newStr[len]='\0';
    return str_2_upper_no_copy_2(newStr, len);
}

int str_tab_index(char *str, char delimiter, int *tabs, int is_newline_break){
    int i, j;
    if(is_newline_break){
        for(i=0,j=0;;i++){
            char c=str[i];
            if(c==delimiter) tabs[j++]=i;
            else if(c=='\n' || c=='\0'){
                tabs[j++]=i;
                break;
            }
        }
    }else{
        for(i=0,j=0;;i++){
            char c=str[i];
            if(c==delimiter) tabs[j++]=i;
            else if(c=='\0'){
                tabs[j++]=i;
                break;
            }
        }
    }
    return j;
}

char **split(char *str, char c, int *arrayNum){
    return split_with_len(str, strlen(str), c, arrayNum);
}

char **split_with_len(char *str, int len, char c, int *arrayNum){
        int i=0;
        if(len==0){
            *(arrayNum)=0;
            return NULL;
        }
        int *start=(int *)my_new(len, sizeof(int));
        int *end=(int *)my_new(len, sizeof(int));
        int num=0;
        for(;i<len;i++){
          char c1=str[i];
          char c2=str[i+1];
          if(c1!=c && i==0) start[num]=0;
          if(c1==c && c2!=c) start[num]=i+1;
          if(c1!=c && (c2==c || c2=='\0')) end[num++]=i+1;
        }
        *(arrayNum)=num;
        char **res=my_new(num, sizeof(char *));
        int index=0;
        for(i=0;i<num;i++){
          res[i]=substring(str, start[index], end[index]);
          index++;
        }
        free(start);
        free(end);
        return res;
}

char *str_copy(char *str){
    return str_copy_with_len(str, strlen(str));
}

char *str_copy_with_len(char *str, int len){
    char *res=my_new2((len+1));
    memcpy(res, str, len);
    res[len]='\0';
    return res;
}

char *str_copy_and_ptr_end(char *buff, char *str){
    while(*(str)){
        *(buff)=*(str);
        buff++;
        str++;
    }
    *(buff)='\0';
    return buff;
}

int str_starts(char *str, char *sub){
	int i=0;
	while(1){
		char c1=str[i];
		char c2=sub[i];
		if(c2=='\0') return 1;
		if(c1!=c2) return 0;
		i++;
	}
}

int str_ends(char *str, char *sub){
    int i=strlen(str)-1;
    int j=strlen(sub)-1;
    if(i<j) return 0;
    for(;i>=0&&j>=0;i--,j--){
        if(str[i]!=sub[j]) return 0;
    }
    return 1;
}

int str_indexOf(char *str, char *sub){
    if(!str || !sub) return -1;

    int i, len=strlen(str);
    for(i=0;i<len;i++){
        int j=i;
        while(1){
            char c1=str[j];
            char c2=sub[j-i];
            if(c2=='\0') return i;
            if(c1!=c2) break;
            j++;
        }
    }

    return -1;
}

int str_indexOf_from(char *str, char *sub, int from){
    if(!str || !sub) return -1;

    int i, len=strlen(str);
    for(i=from;i<len;i++){
        int j=i;
        while(1){
            char c1=str[j];
            char c2=sub[j-i];
            if(c2=='\0') return i;
            if(c1!=c2) break;
            j++;
        }
    }

    return -1;
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (!orig)
        return NULL;
    if (!rep)
        rep = "";
    len_rep = strlen(rep);
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = my_new2(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

void str_replace_char_with_no_copy(char *str, char pre, char now){
    while(*(str)){if(*(str)==pre) *(str)=now;str++;}
}

char *str_reverse(char *str){
    int len=strlen(str), i, j;
    char *res=(char *)my_new2((len+1)*sizeof(char));

    for(i=0,j=len-1;i<len;i++,j--){
        res[i]=str[j];
    }
    res[i]='\0';

    return res;
}

//================ file operation

int64_t get_file_size(char *path){
    int64_t filesize = -1;
    FILE *fp;
    fp = fopen(path, "r");
    if(fp == NULL) return filesize;
    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    fclose(fp);
    return filesize;
}

void *read_file_content(char *path, int64_t *resSize){
    int64_t one_read_file_size=4096;
    int64_t size=get_file_size(path);
    if(size==-1){
        fprintf(stderr, "%s is not exists, exit!\n", path);
        return 0;
    }
    *(resSize)=size;
    //--
    void *buff=my_new(size+1, 1);
    void *copy=buff;
    //--
    FILE *f=fopen(path, "r");
    while(size){
        int64_t num=size<one_read_file_size ? size:one_read_file_size;
        int64_t tmp=fread(buff, num, 1, f);
        buff = (int64_t)buff + tmp;
        size-=tmp;
    }
    fclose(f);
    //--
    return copy;
}

void write_file_content(char *path, void *buff, int64_t size){
    int64_t one_write_file_size=4096;
    FILE *f=fopen(path, "w+");
    if(!f){
        fprintf(stderr, "%s cannot be written, exit!\n", path);
        exit(0);
    }
    while(size){
        int64_t num=size<one_write_file_size ? size:one_write_file_size;
        int64_t tmp=fwrite(buff, num, 1, f);
        buff = (int64_t)buff + tmp;
        size-=tmp;
    }
    fclose(f);
}

void write_string_with_multi_lines(FILE *f, int strLen, char *str, int oneLineLen){
    int i;
    for(i=0;i<strLen;){
        fprintf(f, "%c", str[i]);
        i++;
        if((i%oneLineLen)==0) fprintf(f, "\n");
    }
    if(i%oneLineLen) fprintf(f, "\n");
}

#ifdef _linux

GzStream *gz_stream_open(char *file, char *mode){
    if(file==NULL){
        char buff[1000];
        debug_info(buff);
        fprintf(stderr, "%s: file's name cannot be null.\n", buff);
        exit(0);
    }
    if(mode==NULL){
        char buff[1000];
        debug_info(buff);
        fprintf(stderr, "%s: operate file mode cannot be null.\n", buff);
        exit(0);
    }
    if(strcmp(mode, "w")!=0 && strcmp(mode, "r")!=0){
        char buff[1000];
        debug_info(buff);
        fprintf(stderr, "%s: operate mode(%s) error, it should be \"w\" or \"r\".\n", buff);
        exit(0);
    }
    //--
    GzStream *gz=my_new(1, sizeof(GzStream));
    if(mode[0]=='r'){
#ifdef _use_bz2
        if(str_ends(file, ".bz2")){
            FILE *f=fopen(file, mode);
            if(f==NULL){fprintf(stderr, "open (%s) error, please check!\n", file);exit(0);}
            gz->bz2File=BZ2_bzReadOpen(&(gz->bzerror), f, 0, 0, NULL, 0);
            if(gz->bzerror!=BZ_OK){
                BZ2_bzReadClose(&(gz->bzerror), gz->bz2File);
                fprintf(stderr, "open (%s) .bz2 format error, please check!\n", file);
                exit(0);
            }
        }else
#endif // _use_bz2
        {
            gz->fp=gzopen(file, mode);
            if(gz->fp<=0){fprintf(stderr, "open (%s) error, please check!\n", file);exit(0);}
        }
        gz->is_write=0;
    }else{
        if(str_ends(file, ".gz") || str_ends(file, ".bam")){
            gz->fp=gzopen(file, mode);
            if(gz->fp<=0){fprintf(stderr, "create (%s) error, please check!\n", file);exit(0);}
        }
#ifdef _use_bz2
        else if(str_ends(file, ".bz2")){
            FILE *f=fopen(file, mode);
            if(f==NULL){fprintf(stderr, "create (%s) error, please check!\n", file);exit(0);}
            gz->bz2File=BZ2_bzWriteOpen(&(gz->bzerror), f, 9, 0, 30);
            if(gz->bzerror != BZ_OK){
                BZ2_bzWriteClose(&(gz->bzerror), gz->bz2File, 0, NULL, NULL);
                fprintf(stderr, "create (%s) .bz2 format error, please check!\n", file);
                exit(0);
            }
        }
#endif // _use_bz2
        else{
            gz->out=fopen(file, mode);
            if(gz->out<=0){fprintf(stderr, "create (%s) error, please check!\n", file);exit(0);}
        }
        gz->is_write=1;
    }
    gz->buf=my_new(gz_stream_buff_size, sizeof(char));

    return gz;
}

int gz_read_util(GzStream *gz, char delimiter, char *buff, int max_len, int *res_len){
    if(gz->is_eof && gz->begin>=gz->end) return 0;
    int len=0;
    while(1){
        if(gz->begin>=gz->end){
            gz->begin=0;
            if(gz->fp) gz->end=gzread(gz->fp, gz->buf, gz_stream_buff_size);
#ifdef _use_bz2
            else if(gz->bz2File) gz->end=BZ2_bzRead(&(gz->bzerror), gz->bz2File, gz->buf, gz_stream_buff_size);
#endif // _use_bz2
            if(gz->end<gz_stream_buff_size) gz->is_eof=1;
        }
        while(gz->begin<gz->end){
            char c=gz->buf[gz->begin++];
            if(c==delimiter){
                buff[len]='\0';
                *(res_len)=len;
                return 1;
            }
            buff[len++]=c;
            if(len>=max_len){
                buff[len]='\0';
                *(res_len)=len;
                return 1;
            }
        }
        if(gz->is_eof){
            if(len==0){
                return 0;
            }else{
                buff[len]='\0';
                *(res_len)=len;
                return 1;
            }
        }
    }
}

int gz_read(GzStream *gz, char *str, int len){
    if(gz->is_eof && gz->begin>=gz->end) return 0;
    int res_len=0;
    while(1){
        if(gz->begin>=gz->end){
            gz->begin=0;
            if(gz->fp) gz->end=gzread(gz->fp, gz->buf, gz_stream_buff_size);
#ifdef _use_bz2
            else if(gz->bz2File) gz->end=BZ2_bzRead(&(gz->bzerror), gz->bz2File, gz->buf, gz_stream_buff_size);
#endif // _use_bz2
            if(gz->end<gz_stream_buff_size) gz->is_eof=1;
        }
        int last=gz->end-gz->begin;
        if(len<=last){
            memcpy(str, gz->buf+gz->begin, len);
            gz->begin+=len;
            res_len+=len;
            return res_len;
        }
        memcpy(str, gz->buf+gz->begin, last);
        gz->begin+=last;
        res_len+=last;
        str+=last;
        len-=last;
        if(gz->is_eof) return res_len;
    }
}

void gz_write(GzStream *gz, char *str, int len){
    while(1){
        int last=gz_stream_buff_size-gz->begin;
        if(len<=last){
            memcpy(gz->buf+gz->begin, str, len);
            gz->begin+=len;
            return;
        }
        //--
        memcpy(gz->buf+gz->begin, str, last);
        if(gz->fp) gzwrite(gz->fp, gz->buf, gz_stream_buff_size);
#ifdef _use_bz2
        else if(gz->bz2File) BZ2_bzWrite(&(gz->bzerror), gz->bz2File, gz->buf, gz_stream_buff_size);
#endif // _use_bz2
        else fwrite(gz->buf, 1, gz_stream_buff_size, gz->out);
        //--
        gz->begin=0;
        str+=last;
        len-=last;
    }
}

void gz_write_char(GzStream *gz, char c){
    if(gz->begin==gz_stream_buff_size){
        if(gz->fp) gzwrite(gz->fp, gz->buf, gz_stream_buff_size);
#ifdef _use_bz2
        else if(gz->bz2File) BZ2_bzWrite(&(gz->bzerror), gz->bz2File, gz->buf, gz_stream_buff_size);
#endif // _use_bz2
        else fwrite(gz->buf, 1, gz_stream_buff_size, gz->out);
        gz->begin=0;
    }
    gz->buf[gz->begin++]=c;
}

void gz_stream_destory(GzStream *gz){
    if(gz->is_write){
        if(gz->fp){
            gzwrite(gz->fp, gz->buf, gz->begin);
            gzclose(gz->fp);
        }
#ifdef _use_bz2
        else if(gz->bz2File){
            BZ2_bzWrite(&(gz->bzerror), gz->bz2File, gz->buf, gz->begin);
            BZ2_bzWriteClose(&(gz->bzerror), gz->bz2File, 0, NULL, NULL);
        }
#endif // _use_bz2
        else{
            fwrite(gz->buf, 1, gz->begin, gz->out);
            fclose(gz->out);
        }
    }else{
        if(gz->fp) gzclose(gz->fp);
#ifdef _use_bz2
        else BZ2_bzReadClose(&(gz->bzerror), gz->bz2File);
#endif // _use_bz2
    }
    if(gz->buf) free(gz->buf);
    free(gz);
}

#endif // _linux

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac)\
          {\
          a += F(b,c,d) + x + ac;\
          a = ROTATE_LEFT(a,s);\
          a += b;\
          }
#define GG(a,b,c,d,x,s,ac)\
          {\
          a += G(b,c,d) + x + ac;\
          a = ROTATE_LEFT(a,s);\
          a += b;\
          }
#define HH(a,b,c,d,x,s,ac)\
          {\
          a += H(b,c,d) + x + ac;\
          a = ROTATE_LEFT(a,s);\
          a += b;\
          }
#define II(a,b,c,d,x,s,ac)\
          {\
          a += I(b,c,d) + x + ac;\
          a = ROTATE_LEFT(a,s);\
          a += b;\
          }

typedef struct{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
}MD5_CTX;

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
void MD5Final(MD5_CTX *context,unsigned char digest[16], unsigned char *PADDING);
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);
void MD5Transform(unsigned int state[4],unsigned char block[64]);

void MD5Init(MD5_CTX *context) {
    context->count[0] = 0;
    context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen) {
    unsigned int i = 0,index = 0,partlen = 0;
    index = (context->count[0] >> 3) & 0x3F;
    partlen = 64 - index;
    context->count[0] += inputlen << 3;
    if(context->count[0] < (inputlen << 3))
        context->count[1]++;
    context->count[1] += inputlen >> 29;

    if(inputlen >= partlen) {
       memcpy(&context->buffer[index],input,partlen);
       MD5Transform(context->state,context->buffer);
       for(i = partlen;i+64 <= inputlen;i+=64)
           MD5Transform(context->state,&input[i]);
       index = 0;
    }
    else {
        i = 0;
    }
    memcpy(&context->buffer[index],&input[i],inputlen-i);
}

void MD5Final(MD5_CTX *context,unsigned char digest[16], unsigned char *PADDING) {
    unsigned int index = 0,padlen = 0;
    unsigned char bits[8];
    index = (context->count[0] >> 3) & 0x3F;
    padlen = (index < 56)?(56-index):(120-index);
    MD5Encode(bits,context->count,8);
    MD5Update(context,PADDING,padlen);
    MD5Update(context,bits,8);
    MD5Encode(digest,context->state,16);
}

void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len) {
    unsigned int i = 0,j = 0;
    while(j < len) {
        output[j] = input[i] & 0xFF;
        output[j+1] = (input[i] >> 8) & 0xFF;
        output[j+2] = (input[i] >> 16) & 0xFF;
        output[j+3] = (input[i] >> 24) & 0xFF;
        i++;
        j+=4;
    }
}

void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len) {
     unsigned int i = 0,j = 0;
     while(j < len) {
        output[i] = (input[j]) |
                    (input[j+1] << 8) |
                    (input[j+2] << 16) |
                    (input[j+3] << 24);
        i++;
        j+=4;
     }
}

void MD5Transform(unsigned int state[4],unsigned char block[64]) {
    unsigned int a = state[0];
    unsigned int b = state[1];
    unsigned int c = state[2];
    unsigned int d = state[3];
    unsigned int x[64];
    MD5Decode(x,block,64);
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
    FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
    FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
    FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
    FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
    FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
    FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
    FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
    FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */
    GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */
    GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */
    GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */
    GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */
    GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */
    HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
    HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
    HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
    HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
    HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */
    HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */
    II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
    II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
    II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
    II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
    II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
    II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
    II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */
    II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
    II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

char *get_md5(char *str, int str_len, int is_file){
    unsigned char PADDING[]={
                        0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                        };

    MD5_CTX md5;
    MD5Init(&md5);

    if(is_file){
        int64_t size=1024*1024;
        char buff[1024*1024];

        FILE *f=fopen(str, "r");
        if(f==NULL){
            fprintf(stderr, "open file(%s) error, please check!\n", str);
            exit(0);
        }

        fseek(f, 0, SEEK_END);
        int64_t file_size=ftell(f);
        rewind(f);

        int64_t i, num=file_size/size;
        for(i=0;i<num;i++){
            if(fread(buff, 1, size, f)!=size){
                fprintf(stderr, "Error in read file(%s), in calculate_MD5\n", str);
                exit(0);
            }
            MD5Update(&md5, buff, size);
        }
        int64_t last=file_size-num*size;
        if(last>0){
            if(fread(buff, 1, last, f)!=last){
                fprintf(stderr, "Error in read file(%s), in calculate_MD5\n", str);
                exit(0);
            }
            MD5Update(&md5, buff, last);
        }

        fclose(f);
    }else{
        MD5Update(&md5, str, str_len);
    }

    unsigned char decrypt[16];
    MD5Final(&md5, decrypt, PADDING);

    char *res=my_new(33, sizeof(char));
    int i;
    for(i=0;i<16;i++) {
        sprintf(res+i*2, "%02x", decrypt[i]);
    }

    return res;
}

//================ sam bam

int to_bam_header(int l_chrNames, char **chrNames, int *chrLengths, char *ID, char *SM, char *LB, char *buff){
    int i;

    char *original=buff;
    buff[0]='B';
    buff[1]='A';
    buff[2]='M';
    buff[3]=1;
    buff+=8;

    for(i=0;i<l_chrNames;i++){
        buff+=sprintf(buff, "@SQ\tSN:%s\tLN:%d\n", chrNames[i], chrLengths[i]);
    }
    buff+=sprintf(buff, "@RG\tID:%s\tSM:%s\tLB:%s\tPL:ILLUMINA\tPU:run\n", ID, SM, LB);

    {
        int32_t *ptr_original=(int32_t *)(original+4);
        *(ptr_original)=buff-original-8;
        int32_t *ptr_buff=(int32_t *)buff;
        *(ptr_buff)=l_chrNames;
        buff+=4;
    }

    for(i=0;i<l_chrNames;i++){
        char *name=chrNames[i];
        int len=chrLengths[i];
        {
            int32_t *ptr_buff=(int32_t *)buff;
            *(ptr_buff)=strlen(name)+1;
            buff+=4;
        }

        buff+=sprintf(buff, "%s", name);
        buff++;

        {
            int32_t *ptr_buff=(int32_t *)buff;
            *(ptr_buff)=len;
            buff+=4;
        }
    }

    return buff-original;
}

#ifdef _linux

void skip_bam_header(GzStream *bam, char *buff){
    int32_t i, datasize, n_targets, name_len, target_len;
    gz_read(bam, buff, 4);
    gz_read(bam, &datasize, 4);
    gz_read(bam, buff, datasize);
    gz_read(bam, &n_targets, 4);
    for(i=0;i<n_targets;i++){
        gz_read(bam, &name_len, 4);
        gz_read(bam, buff, name_len);
        gz_read(bam, &target_len, 4);
    }
}

BamHeader *read_bam_header(GzStream *bam, char *buff){
    BamHeader *header=calloc(1, sizeof(BamHeader));

    int32_t i, datasize, n_targets, name_len, target_len;
    gz_read(bam, buff, 4);
    gz_read(bam, &datasize, 4);
    gz_read(bam, buff, datasize);
    header->header=str_copy_with_len(buff, datasize);
    header->l_header=datasize;
    gz_read(bam, &n_targets, 4);
    header->l_chr=n_targets;
    header->chr_names=calloc(n_targets, sizeof(char *));
    header->l_chr_names=calloc(n_targets, sizeof(int));
    header->chr_lens=calloc(n_targets, sizeof(int));
    for(i=0;i<n_targets;i++){
        gz_read(bam, &name_len, 4);
        gz_read(bam, buff, name_len);
        header->chr_names[i]=str_copy_with_len(buff, name_len-1);
        header->l_chr_names[i]=name_len-1;
        gz_read(bam, &target_len, 4);
        header->chr_lens[i]=target_len;
    }

    return header;
}

int read_bam_to_sam(GzStream *bam, int l_chrNames, char **chrNames, char *buff, char *sam){
    int32_t datasize;
    if(gz_read(bam, &datasize, 4)==0) return 0;
    gz_read(bam, buff, datasize);
    return bam_to_sam(l_chrNames, chrNames, datasize, buff, sam);
}

void free_bamHeader(BamHeader *header){
    int i;
    for(i=0;i<header->l_chr;i++) free(header->chr_names[i]);
    free(header->chr_names);
    free(header->l_chr_names);
    free(header->chr_lens);
    free(header->header);
    free(header);
}

#endif // _linux

int bam_to_sam(int l_chrNames, char **chrNames, int datasize, char *buff, char *sam){
    char *original=buff;
    char *original_sam=sam;

    int32_t i, j, len;

    uint32_t *x=(uint32_t *)buff;
    int tid=x[0];
    int pos=x[1];
    int bin=x[2]>>16;
    int qual=(0xFF00&x[2])>>8;
    int l_qname=0xFF&x[2];
    int flag=x[3]>>16;
    int n_cigar=0xFFFF&x[3];
    int l_qseq=x[4];
    int mtid=x[5];
    int mpos=x[6];
    int isize=x[7];
    buff+=32;

    //--name
    memcpy(sam, buff, l_qname-1);
    sam[l_qname-1]='\t';
    sam+=l_qname;
    buff+=l_qname;

    //--flag
    sam+=sprintf(sam, "%d\t", flag);

    //--chr
    if(tid==-1){sam[0]='*';sam[1]='\t';sam+=2;}
    else sam+=sprintf(sam, "%s\t", chrNames[tid]);

    //--pos
    if(pos==-1) sam+=sprintf(sam, "*\t");
    else sam+=sprintf(sam, "%d\t", pos+1);

    //--qual
    sam+=sprintf(sam, "%d\t", qual);

    //--cigar
    x=(int32_t *)buff;
    for(i=0;i<n_cigar;i++){
        int n=x[i]>>4;
        int o=x[i]&0xF;
        if(o==0) sam+=sprintf(sam, "%dM", n);
        else if(o==1) sam+=sprintf(sam, "%dI", n);
        else if(o==2) sam+=sprintf(sam, "%dD", n);
        else if(o==3) sam+=sprintf(sam, "%dN", n);
        else if(o==4) sam+=sprintf(sam, "%dS", n);
        else if(o==5) sam+=sprintf(sam, "%dH", n);
        else if(o==6) sam+=sprintf(sam, "%dP", n);
    }
    sam[0]='\t';
    sam++;
    buff+=n_cigar*4;

    //--mchr
    if(mtid==-1){sam[0]='*';sam[1]='\t';sam+=2;}
    else if(mtid==tid){sam[0]='=';sam[1]='\t';sam+=2;}
    else sam+=sprintf(sam, "%s\t", chrNames[mtid]);

    //--mpos
    if(mpos==-1) sam+=sprintf(sam, "*\t");
    else sam+=sprintf(sam, "%d\t", mpos+1);

    //--isize
    sam+=sprintf(sam, "%d\t", isize);

    //--seq
    uint8_t p=*(buff++);
    for(i=0;i<l_qseq;i++){
        *(sam++)=bam_int_2_base[(p>>(4*(1-i%2)))&0xF];
        if(i%2==1 && i!=(l_qseq-1)) p=*(buff++);
    }
    *(sam++)='\t';

    //--qual
    for(i=0;i<l_qseq;i++) *(sam++)=*(buff++)+33;
    *(sam++)='\t';

    while((buff-original)<datasize){
        *(sam++)=*(buff++);
        *(sam++)=*(buff++);
        *(sam++)=':';
        char type=*(buff++);
        if(type=='A'){
            *(sam++)=type;
            *(sam++)=':';
            *(sam++)=*(buff++);
        }else if(type=='c'){
            *(sam++)='i';
            *(sam++)=':';
            int8_t *ptr=buff;
            sam+=sprintf(sam, "%d", ptr[0]);
            buff++;
        }else if(type=='s'){
            *(sam++)='i';
            *(sam++)=':';
            int16_t *ptr=buff;
            sam+=sprintf(sam, "%d", ptr[0]);
            buff+=2;
        }else if(type=='i'){
            *(sam++)='i';
            *(sam++)=':';
            int32_t *ptr=buff;
            sam+=sprintf(sam, "%d", ptr[0]);
            buff+=4;
        }else if(type=='C'){
            *(sam++)='i';
            *(sam++)=':';
            uint8_t *ptr=buff;
            sam+=sprintf(sam, "%d", ptr[0]);
            buff++;
        }else if(type=='S'){
            *(sam++)='i';
            *(sam++)=':';
            uint16_t *ptr=buff;
            sam+=sprintf(sam, "%d", ptr[0]);
            buff+=2;
        }else if(type=='I'){
            *(sam++)='i';
            *(sam++)=':';
            uint32_t *ptr=buff;
            sam+=sprintf(sam, "%d", ptr[0]);
            buff+=4;
        }else if(type=='f'){
            *(sam++)=type;
            *(sam++)=':';
            float *ptr=buff;
            sam+=sprintf(sam, "%f", ptr[0]);
            buff+=4;
        }else if(type=='d'){
            *(sam++)=type;
            *(sam++)=':';
            double *ptr=buff;
            sam+=sprintf(sam, "%f", ptr[0]);
            buff+=8;
        }else if(type=='Z' || type=='H'){
            *(sam++)=type;
            *(sam++)=':';
            len=strlen(buff);
            memcpy(sam, buff, len);
            sam+=len;
            buff+=(len+1);
        }
        *(sam++)='\t';
    }
    sam--;
    sam[0]='\0';
    sam[1]='\0';

    return sam-original_sam;
}

int sam_to_bam(int l_chrNames, char **chrNames, char *sam, int *tabs, char *buff){
    int i, j, l_tabs=0;

    char *original=buff;
    buff+=36;

    //-- calculate tabs positions
    for(i=0,j=0;;i++){
        char c=sam[i];
        if(c=='\t') tabs[j++]=i;
        else if(c=='\n' || c=='\0'){
            tabs[j++]=i;
            break;
        }
    }
    l_tabs=j;

    //-- unmapped, not convert
    i=tabs[1]+1;
    if(sam[i]=='*') return 0;

    //-- qual
    i=tabs[3]+1;
    j=tabs[4];
    sam[j]='\0';
    int qual=isdigit(sam[i])? atoi(sam+i) : 0;
    sam[j]='\t';

    //-- name
    i=0;
    j=tabs[0];
    sam[j]='\0';
    int l_qname=j-i+1;
    memcpy(buff, sam+i, (l_qname)*sizeof(char));
    buff+=l_qname;
    sam[j]='\t';

    //-- flag
    i=tabs[0]+1;
    j=tabs[1];
    sam[j]='\0';
    int flag=atoi(sam+i);
    sam[j]='\t';

    //-- tid
    int tid=-1;
    if(chrNames==NULL) tid=0;
    else{
        i=tabs[1]+1;
        j=tabs[2];
        sam[j]='\0';
        char *str=sam+i;
        for(i=0;i<l_chrNames;i++){if(strcmp(str, chrNames[i])==0){tid=i;break;}}
        sam[j]='\t';
        if(tid==-1){fprintf(stderr, "Error tid: %s\n", sam);exit(0);}
    }

    //-- pos
    i=tabs[2]+1;
    j=tabs[3];
    sam[j]='\0';
    int pos=isdigit(sam[i]) ? atoi(sam+i)-1 : -1;
    sam[j]='\t';

    //-- cigar
    i=tabs[4]+1;
    j=tabs[5];
    sam[j]='\0';
    int bin=0, n_cigar=0;
    int32_t *cigar=NULL;
    {
        char *s, *t, *str=sam+i;
        int op;
        int64_t x;
        if(str[0]!='*'){
            for(s=str;*s;++s){if(isalpha(*s)) ++n_cigar;}
            cigar=(int32_t *)my_new(n_cigar, sizeof(int32_t));
            for(s=str,i=0;i<n_cigar;++i){
                x=strtol(s, &t, 10);
                op=toupper(*t);
                s=t+1;
                if(op=='M') op = 0;
                else if(op=='I') op = 1;
                else if(op=='D') op = 2;
                else if(op=='N') op = 3;
                else if(op=='S') op = 4;
                else if(op=='H') op = 5;
                else if(op=='P') op = 6;
                cigar[i]=(int32_t)(x<<4|op);
                int32_t *ptr_buff=(int32_t *)buff;
                *(ptr_buff)=cigar[i];
                buff+=4;
            }
            bin=bam_reg2bin(pos, bam_calend(pos, n_cigar, cigar));
        }else bin=bam_reg2bin(pos, pos+1);
    }
    sam[j]='\t';

    //-- mtid
    i=tabs[5]+1;
    int mtid=-1;
    if(sam[i]=='=') mtid=tid;
    else if(sam[i]=='*' || chrNames==NULL) mtid=-1;
    else{
        j=tabs[6];
        sam[j]='\0';
        char *str=sam+i;
        for(i=0;i<l_chrNames;i++){if(strcmp(str, chrNames[i])==0){mtid=i;break;}}
        sam[j]='\t';
        if(mtid==-1){fprintf(stderr, "Error mtid: %s\n", sam);exit(0);}
    }

    //-- mpos
    i=tabs[6]+1;
    j=tabs[7];
    sam[j]='\0';
    int mpos=isdigit(sam[i]) ? atoi(sam+i)-1:-1;
    sam[j]='\t';

    //-- isize
    i=tabs[7]+1;
    j=tabs[8];
    sam[j]='\0';
    int isize=(sam[i]=='-' || isdigit(sam[i])) ? atoi(sam+i):0;
    sam[j]='\t';

    //-- seq
    int l_qseq=0;
    {
        uint8_t p;

        i=tabs[8]+1;
        j=tabs[9];
        sam[j]='\0';
        char *str=sam+i;
        l_qseq=j-i;
		for(i=0;i<l_qseq;++i){
			if((i%2)==0) p=0;
            p|=bam_nt16_table[(int)str[i]] << 4*(1-i%2);
            if((i%2)==1){*(buff)=p;buff++;}
		}
		if((l_qseq%2)==1){*(buff)=p;buff++;}
		sam[j]='\t';

		i=tabs[9]+1;
        j=tabs[10];
        sam[j]='\0';
		str=sam+i;
		if((j-i)==1 && str[0]=='*'){    //strcmp(str, "*")==0
            for(i=0;i<l_qseq;++i){*(buff)=0xff;buff++;}
		}else{
            for(i=0;i<l_qseq;++i){*(buff)=str[i]-33;buff++;}
		}
        sam[j]='\t';
    }

    //--
    char pre;
    int index, max=l_tabs-1;
    for(index=10;index<max;index++,sam[j]=pre){
        i=tabs[index]+1;
        j=tabs[index+1];
        pre=sam[j];
        sam[j]='\0';
        char *str=sam+i;
        //--
        *(buff)=str[0];buff++;
        *(buff)=str[1];buff++;
        char type=str[3];
        if(type=='A' || type=='a' || type=='c' || type=='C'){ // c and C for backward compatibility
				*(buff)='A';buff++;
                *(buff)=str[5];buff++;
        }else if(type=='I' || type=='i'){
            int64_t x=atoll(str+5);
            if(x<0){
                if(x>=-127){
                    *(buff)='c';buff++;
                    int8_t *ptr_buff=(int8_t *)buff;
                    *(ptr_buff)=(int8_t)x;
                    buff++;
                }else if(x>=-32767){
                    *(buff)='s';buff++;
                    int16_t *ptr_buff=(int16_t *)buff;
                    *(ptr_buff)=(int16_t)x;
                    buff+=2;
                }else{
                    *(buff)='i';buff++;
                    int32_t *ptr_buff=(int32_t *)buff;
                    *(ptr_buff)=(int32_t)x;
                    buff+=4;
                }
            }else{
                if(x<=255){
                    *(buff)='C';buff++;
                    uint8_t *ptr_buff=(uint8_t *)buff;
                    *(ptr_buff)=(uint8_t)x;
                    buff++;
                }else if(x<=65535){
                    *(buff)='S';buff++;
                    uint16_t *ptr_buff=(uint16_t *)buff;
                    *(ptr_buff)=(uint16_t)x;
                    buff+=2;
                }else{
                    *(buff)='I';buff++;
                    uint32_t *ptr_buff=(uint32_t *)buff;
                    *(ptr_buff)=(uint32_t)x;
                    buff+=4;
                }
            }
        }else if(type=='f'){
            *(buff)=type;buff++;
            float *ptr_buff=(float *)buff;
            *(ptr_buff)=atof(str+5);
            buff+=4;
        }else if(type=='d'){
            *(buff)=type;buff++;
            double *ptr_buff=(double *)buff;
            *(ptr_buff)=atof(str+9);
            buff+=8;
        }else if(type=='Z' || type=='H'){
            *(buff)=type;buff++;
            int len=j-i-5+1;
            memcpy(buff, str+5, len*sizeof(char));
            buff+=len;
        }
    }

    //--
    uint32_t x[8];
    x[0] = tid;
	x[1] = pos;
	x[2] = (uint32_t)bin<<16 | qual<<8 | l_qname;
	x[3] = (uint32_t)flag<<16 | n_cigar;
	x[4] = l_qseq;
	x[5] = mtid;
	x[6] = mpos;
	x[7] = isize;

	int len=buff-original;
	int32_t block_len=len-4;

	uint32_t *ptr_original=(uint32_t *)original;
    ptr_original[0]=block_len;
    for(i=0;i<8;i++) ptr_original[i+1]=x[i];

    if(cigar) free(cigar);
    return len;
}

BamBuff *sam_to_bam_buff(int l_chrNames, char **chrNames, char *sam, int *tabs, char *buff){
    BamBuff *bam=my_new(1, sizeof(BamBuff));

    int i, j, l_tabs=0;

    char *original=buff;
    buff+=36;

    //-- calculate tabs positions
    for(i=0,j=0;;i++){
        char c=sam[i];
        if(c=='\t') tabs[j++]=i;
        else if(c=='\n' || c=='\0'){
            tabs[j++]=i;
            sam[i]='\0';
            break;
        }
    }
    l_tabs=j;

    //-- name
    i=0;
    j=tabs[0];
    sam[j]='\0';
    int l_qname=j-i+1;
    memcpy(buff, sam+i, (l_qname)*sizeof(char));
    buff+=l_qname;
    bam->name=atoll(sam+i);
    sam[j]='\t';

    //-- flag
    i=tabs[0]+1;
    j=tabs[1];
    sam[j]='\0';
    int flag=atoi(sam+i);
    bam->flag=flag;
    sam[j]='\t';

    //-- unmapped
    i=tabs[1]+1;
    if(sam[i]=='*' || flag&0x4 || flag&0x100 || flag&0x200 || flag&0x400){
    //if(sam[i]=='*' || flag&0x4){
        free(bam);
        return NULL;
    }

    //-- tid
    int tid=-1;
    if(chrNames==NULL) tid=0;
    else{
        i=tabs[1]+1;
        j=tabs[2];
        sam[j]='\0';
        char *str=sam+i;
        for(i=0;i<l_chrNames;i++){if(strcmp(str, chrNames[i])==0){tid=i;break;}}
        sam[j]='\t';
        if(tid==-1){fprintf(stderr, "Error tid: %s\n", sam);exit(0);}
    }
    bam->chr=tid;

    //-- pos
    i=tabs[2]+1;
    j=tabs[3];
    sam[j]='\0';
    int pos=isdigit(sam[i]) ? atoi(sam+i)-1 : -1;
    bam->pos=pos+1;
    sam[j]='\t';

    //-- qual
    i=tabs[3]+1;
    j=tabs[4];
    sam[j]='\0';
    int qual=isdigit(sam[i])? atoi(sam+i) : 0;
    sam[j]='\t';

    //-- cigar
    i=tabs[4]+1;
    j=tabs[5];
    sam[j]='\0';
    int bin=0, n_cigar=0;
    int32_t *cigar=NULL;
    int32_t read_cover_reference_length=0;
    int len_S=0;
    {
        char *s, *t, *str=sam+i;
        int op;
        int64_t x;
        if(str[0]!='*'){
            for(s=str;*s;++s){if(isalpha(*s)) ++n_cigar;}
            cigar=(int32_t *)my_new(n_cigar, sizeof(int32_t));
            for(s=str,i=0;i<n_cigar;++i){
                x=strtol(s, &t, 10);
                op=toupper(*t);
                s=t+1;
                if (op == 'M'){
                    op = 0;
                    read_cover_reference_length+=x;
                }else if (op == 'I'){
                    op = 1;
                }else if (op == 'D'){
                    op = 2;
                    read_cover_reference_length+=x;
                }else if (op == 'N'){
                    op = 3;
                    read_cover_reference_length+=x;
                }else if (op == 'S'){
                    op = 4;
                    len_S+=x;
                }else if (op == 'H'){
                    op = 5;
                }else if (op == 'P'){
                    op = 6;
                }
                cigar[i]=(int32_t)(x<<4|op);
                int32_t *ptr_buff=(int32_t *)buff;
                *(ptr_buff)=cigar[i];
                buff+=4;
            }
            bin=bam_reg2bin(pos, bam_calend(pos, n_cigar, cigar));
        }else bin=bam_reg2bin(pos, pos+1);
    }
    bam->ref_start=pos;
    bam->ref_end=pos+len_S+read_cover_reference_length;
    sam[j]='\t';

    //-- coordinate
    int32_t coordinate;
    {
        if((flag&16)){
            coordinate=pos+1+read_cover_reference_length-1;
            for(i=n_cigar-1;i>=0;i--){
                int op=cigar[i]&0x0F;
                int x=cigar[i]>>4;
                if((op==4) || (op==5)) coordinate+=x;
                else break;
            }
            coordinate=-coordinate;
        }else{
            coordinate=pos+1;
            for(i=0;i<n_cigar;i++){
                int op=cigar[i]&0x0F;
                int x=cigar[i]>>4;
                if((op==4) || (op==5)) coordinate-=x;
                else break;
            }
        }
    }
    bam->coordinate=coordinate;

    //-- score
    int score=0;
    i=tabs[9]+1;
    j=tabs[10];
    for(;i<j;i++){
        int n=sam[i]-33;
        if(n>15) score+=n;
    }
    score=score<bam_max_score ? score:bam_max_score;
    bam->score=score;

    //-- mtid
    i=tabs[5]+1;
    int mtid=-1;
    if(sam[i]=='=') mtid=tid;
    else if(sam[i]=='*' || chrNames==NULL) mtid=-1;
    else{
        j=tabs[6];
        sam[j]='\0';
        char *str=sam+i;
        for(i=0;i<l_chrNames;i++){if(strcmp(str, chrNames[i])==0){mtid=i;break;}}
        sam[j]='\t';
        if(mtid==-1){fprintf(stderr, "Error mtid: %s\n", sam);exit(0);}
    }
    bam->mtid=mtid;

    //-- mpos
    i=tabs[6]+1;
    j=tabs[7];
    sam[j]='\0';
    int mpos=isdigit(sam[i]) ? atoi(sam+i)-1:-1;
    sam[j]='\t';

    //-- isize
    i=tabs[7]+1;
    j=tabs[8];
    sam[j]='\0';
    int isize=(sam[i]=='-' || isdigit(sam[i])) ? atoi(sam+i):0;
    sam[j]='\t';

    //-- seq
    int l_qseq=0;
    {
        uint8_t p=0;

        i=tabs[8]+1;
        j=tabs[9];
        sam[j]='\0';
        char *str=sam+i;
        l_qseq=j-i;
		for(i=0;i<l_qseq;++i){
            if((i%2)==0) p=0;
            p|=bam_nt16_table[(int)str[i]] << 4*(1-i%2);
            if((i%2)==1){*(buff)=p;buff++;}
		}
		if((l_qseq%2)==1){*(buff)=p;buff++;}
		sam[j]='\t';

		i=tabs[9]+1;
        j=tabs[10];
        sam[j]='\0';
		str=sam+i;
		if((j-i)==1 && str[0]=='*'){    //strcmp(str, "*")==0
            for(i=0;i<l_qseq;++i){*(buff)=0xff;buff++;}
		}else{
            for(i=0;i<l_qseq;++i){*(buff)=str[i]-33;buff++;}
		}
        sam[j]='\t';
    }

    //--
    char pre;
    int index, max=l_tabs-1;
    for(index=10;index<max;index++,sam[j]=pre){
        i=tabs[index]+1;
        j=tabs[index+1];
        pre=sam[j];
        sam[j]='\0';
        char *str=sam+i;
        //--
        *(buff)=str[0];buff++;
        *(buff)=str[1];buff++;
        char type=str[3];
        if(type=='A' || type=='a' || type=='c' || type=='C'){ // c and C for backward compatibility
				*(buff)='A';buff++;
                *(buff)=str[5];buff++;
        }else if(type=='I' || type=='i'){
            int64_t x=atoll(str+5);
            if(x<0){
                if(x>=-127){
                    *(buff)='c';buff++;
                    int8_t *ptr_buff=(int8_t *)buff;
                    *(ptr_buff)=(int8_t)x;
                    buff++;
                }else if(x>=-32767){
                    *(buff)='s';buff++;
                    int16_t *ptr_buff=(int16_t *)buff;
                    *(ptr_buff)=(int16_t)x;
                    buff+=2;
                }else{
                    *(buff)='i';buff++;
                    int32_t *ptr_buff=(int32_t *)buff;
                    *(ptr_buff)=(int32_t)x;
                    buff+=4;
                }
            }else{
                if(x<=255){
                    *(buff)='C';buff++;
                    uint8_t *ptr_buff=(uint8_t *)buff;
                    *(ptr_buff)=(uint8_t)x;
                    buff++;
                }else if(x<=65535){
                    *(buff)='S';buff++;
                    uint16_t *ptr_buff=(uint16_t *)buff;
                    *(ptr_buff)=(uint16_t)x;
                    buff+=2;
                }else{
                    *(buff)='I';buff++;
                    uint32_t *ptr_buff=(uint32_t *)buff;
                    *(ptr_buff)=(uint32_t)x;
                    buff+=4;
                }
            }
        }else if(type=='f'){
            *(buff)=type;buff++;
            float *ptr_buff=(float *)buff;
            *(ptr_buff)=atof(str+5);
            buff+=4;
        }else if(type=='d'){
            *(buff)=type;buff++;
            double *ptr_buff=(double *)buff;
            *(ptr_buff)=atof(str+9);
            buff+=8;
        }else if(type=='Z' || type=='H'){
            *(buff)=type;buff++;
            int len=j-i-5+1;
            memcpy(buff, str+5, len*sizeof(char));
            buff+=len;
        }
    }

    //--
    uint32_t x[8];
    x[0] = tid;
	x[1] = pos;
	x[2] = (uint32_t)bin<<16 | qual<<8 | l_qname;
	x[3] = (uint32_t)flag<<16 | n_cigar;
	x[4] = l_qseq;
	x[5] = mtid;
	x[6] = mpos;
	x[7] = isize;

	int len=buff-original;
	int32_t block_len=len-4;

	uint32_t *ptr_original=(uint32_t *)original;
    ptr_original[0]=block_len;
    for(i=0;i<8;i++) ptr_original[i+1]=x[i];

    if(cigar) free(cigar);

    bam->l_bam=len;
    bam->bam=str_copy_with_len(original, len);
    return bam;
}

uint32_t bam_calend(int pos, int n_cigar, int32_t *cigar){
    uint32_t k, end;
	end = pos;
	for (k=0; k<n_cigar; ++k){
		int op = cigar[k] & 15;
		if (op == 0 || op == 2 || op == 3)
			end += cigar[k] >> 4;
	}
	return end;
}

int bam_reg2bin(uint32_t beg, uint32_t end){
	--end;
	if (beg>>14 == end>>14) return 4681 + (beg>>14);
	if (beg>>17 == end>>17) return  585 + (beg>>17);
	if (beg>>20 == end>>20) return   73 + (beg>>20);
	if (beg>>23 == end>>23) return    9 + (beg>>23);
	if (beg>>26 == end>>26) return    1 + (beg>>26);
	return 0;
}

//==================== copy

int *copy_int_array(int size, int *array){
    int *res=my_new2(size*sizeof(int));
    memcpy(res, array, size*sizeof(int));
    return res;
}

int64_t *copy_long_array(int size, int64_t *array){
    int64_t *res=my_new2(size*sizeof(int64_t));
    memcpy(res, array, size*sizeof(int64_t));
    return res;
}

double *copy_double_array(int size, double *array){
    double *res=my_new2(size*sizeof(double));
    memcpy(res, array, size*sizeof(double));
    return res;
}

//==================== sort

static int QUICKSORT_THRESHOLD = 286;
static int INSERTION_SORT_THRESHOLD = 47;
static int MAX_RUN_COUNT = 67;
static int MAX_RUN_LENGTH = 33;
static int COUNTING_SORT_THRESHOLD_FOR_SHORT_OR_CHAR = 3200;
static int NUM_SHORT_VALUES = 1 << 16;
static int MIN_MERGE = 32;
static int MIN_GALLOP = 7;
static int INITIAL_TMP_STORAGE_LENGTH = 256;

void mysort_int(int *a, int left, int length){
    int right=left+length-1, i, j, k, m, n;

    if( (right - left) < QUICKSORT_THRESHOLD ) {
        mysort_int_inner(a, left, right, 1);
        return;
    }

    int *run=my_new2((MAX_RUN_COUNT + 1)*sizeof(int));
    int count = 0; run[0] = left;

        // Check if the array is nearly sorted
        for (k = left; k < right; run[count] = k) {
            if (a[k] < a[k + 1]) { // ascending
                while (++k <= right && a[k - 1] <= a[k]);
            } else if (a[k] > a[k + 1]) { // descending
                while (++k <= right && a[k - 1] >= a[k]);
                int lo,hi;
                for (lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                    int t = a[lo]; a[lo] = a[hi]; a[hi] = t;
                }
            } else { // equal
                for (m = MAX_RUN_LENGTH; ++k <= right && a[k - 1] == a[k]; ) {
                    if (--m == 0) {
                        mysort_int_inner(a, left, right, true);
                        free(run);
                        return;
                    }
                }
            }

            /*
             * The array is not highly structured,
             * use Quicksort instead of merge sort.
             */
            if (++count == MAX_RUN_COUNT) {
                mysort_int_inner(a, left, right, 1);
                free(run);
                return;
            }
        }

        // Check special cases
        // Implementation note: variable "right" is increased by 1.
        if (run[count] == right++) { // The last run contains one element
            run[++count] = right;
        } else if (count == 1) { // The array is already sorted
            free(run);
            return;
        }

        // Determine alternation base for merge
        char odd = 0;
        for (n = 1; (n <<= 1) < count; odd ^= 1);

        // Use or create temporary array b for merging
        int *b;                 // temp array; alternates with a
        int ao, bo;              // array offsets from 'left'
        int blen = right - left; // space needed for b

        int *work=my_new2(blen*sizeof(int));
        int workBase=0;

        if (odd == 0) {
            //System.arraycopy(a, left, work, workBase, blen);
            for(i=0;i<blen;i++){
                work[i+workBase]=a[i+left];
            }

            b = a;
            bo = 0;
            a = work;
            ao = workBase - left;
        } else {
            b = work;
            ao = 0;
            bo = workBase - left;
        }

        // Merging
        int last;
        for (; count > 1; count = last) {
            for ( k = (last = 0) + 2; k <= count; k += 2) {
                int hi = run[k], mi = run[k - 1];
                int p, q;
                for (i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                    if (q >= hi || p < mi && a[p + ao] <= a[q + ao]) {
                        b[i + bo] = a[p++ + ao];
                    } else {
                        b[i + bo] = a[q++ + ao];
                    }
                }
                run[++last] = hi;
            }
            if ((count & 1) != 0) {
                int lo;
                for ( i = right, lo = run[count - 1]; --i >= lo;
                    b[i + bo] = a[i + ao]
                );
                run[++last] = right;
            }
            int *t = a; a = b; b = t;
            int o = ao; ao = bo; bo = o;
        }

        free(work);
        free(run);
}

void mysort_int_inner(int *a, int left, int right, int leftmost){
        int length = right - left + 1, i, j, k;

        // Use insertion sort on tiny arrays
        if (length < INSERTION_SORT_THRESHOLD) {
            if (leftmost) {
                /*
                 * Traditional (without sentinel) insertion sort,
                 * optimized for server VM, is used in case of
                 * the leftmost part.
                 */
                for (i = left, j = i; i < right; j = ++i) {
                    int ai = a[i + 1];
                    while (ai < a[j]) {
                        a[j + 1] = a[j];
                        if (j-- == left) {
                            break;
                        }
                    }
                    a[j + 1] = ai;
                }
            } else {
                /*
                 * Skip the longest ascending sequence.
                 */
                do {
                    if (left >= right) {
                        return;
                    }
                } while (a[++left] >= a[left - 1]);

                /*
                 * Every element from adjoining part plays the role
                 * of sentinel, therefore this allows us to avoid the
                 * left range check on each iteration. Moreover, we use
                 * the more optimized algorithm, so called pair insertion
                 * sort, which is faster (in the context of Quicksort)
                 * than traditional implementation of insertion sort.
                 */
                for (k = left; ++left <= right; k = ++left) {
                    int a1 = a[k], a2 = a[left];

                    if (a1 < a2) {
                        a2 = a1; a1 = a[left];
                    }
                    while (a1 < a[--k]) {
                        a[k + 2] = a[k];
                    }
                    a[++k + 1] = a1;

                    while (a2 < a[--k]) {
                        a[k + 1] = a[k];
                    }
                    a[k + 1] = a2;
                }
                int last = a[right];

                while (last < a[--right]) {
                    a[right + 1] = a[right];
                }
                a[right + 1] = last;
            }
            return;
        }

        // Inexpensive approximation of length / 7
        int seventh = (length >> 3) + (length >> 6) + 1;

        /*
         * Sort five evenly spaced elements around (and including) the
         * center element in the range. These elements will be used for
         * pivot selection as described below. The choice for spacing
         * these elements was empirically determined to work well on
         * a wide variety of inputs.
         */
        int e3 = (left + right) >> 1; // The midpoint
        int e2 = e3 - seventh;
        int e1 = e2 - seventh;
        int e4 = e3 + seventh;
        int e5 = e4 + seventh;

        // Sort these elements using insertion sort
        if (a[e2] < a[e1]) { int t = a[e2]; a[e2] = a[e1]; a[e1] = t; }

        if (a[e3] < a[e2]) { int t = a[e3]; a[e3] = a[e2]; a[e2] = t;
            if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
        }
        if (a[e4] < a[e3]) { int t = a[e4]; a[e4] = a[e3]; a[e3] = t;
            if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
            }
        }
        if (a[e5] < a[e4]) { int t = a[e5]; a[e5] = a[e4]; a[e4] = t;
            if (t < a[e3]) { a[e4] = a[e3]; a[e3] = t;
                if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                    if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
                }
            }
        }

        // Pointers
        int less  = left;  // The index of the first element of center part
        int great = right; // The index before the first element of right part

        if (a[e1] != a[e2] && a[e2] != a[e3] && a[e3] != a[e4] && a[e4] != a[e5]) {
            /*
             * Use the second and fourth of the five sorted elements as pivots.
             * These values are inexpensive approximations of the first and
             * second terciles of the array. Note that pivot1 <= pivot2.
             */
            int pivot1 = a[e2];
            int pivot2 = a[e4];

            /*
             * The first and the last elements to be sorted are moved to the
             * locations formerly occupied by the pivots. When partitioning
             * is complete, the pivots are swapped back into their final
             * positions, and excluded from subsequent sorting.
             */
            a[e2] = a[left];
            a[e4] = a[right];

            /*
             * Skip elements, which are less or greater than pivot values.
             */
            while (a[++less] < pivot1);
            while (a[--great] > pivot2);

            /*
             * Partitioning:
             *
             *   left part           center part                   right part
             * +--------------------------------------------------------------+
             * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
             * +--------------------------------------------------------------+
             *               ^                          ^       ^
             *               |                          |       |
             *              less                        k     great
             *
             * Invariants:
             *
             *              all in (left, less)   < pivot1
             *    pivot1 <= all in [less, k)     <= pivot2
             *              all in (great, right) > pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            outer1:
            for (k = less - 1; ++k <= great; ) {
                int ak = a[k];
                if (ak < pivot1) { // Move a[k] to left part
                    a[k] = a[less];
                    /*
                     * Here and below we use "a[i] = b; i++;" instead
                     * of "a[i++] = b;" due to performance issue.
                     */
                    a[less] = ak;
                    ++less;
                } else if (ak > pivot2) { // Move a[k] to right part
                    while (a[great] > pivot2) {
                        if (great-- == k) {
                            goto outer1;
                        }
                    }
                    if (a[great] < pivot1) { // a[great] <= pivot2
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // pivot1 <= a[great] <= pivot2
                        a[k] = a[great];
                    }
                    /*
                     * Here and below we use "a[i] = b; i--;" instead
                     * of "a[i--] = b;" due to performance issue.
                     */
                    a[great] = ak;
                    --great;
                }
            }

            // Swap pivots into their final positions
            a[left]  = a[less  - 1]; a[less  - 1] = pivot1;
            a[right] = a[great + 1]; a[great + 1] = pivot2;

            // Sort left and right parts recursively, excluding known pivots
            mysort_int_inner(a, left, less - 2, leftmost);
            mysort_int_inner(a, great + 2, right, 0);

            /*
             * If center part is too large (comprises > 4/7 of the array),
             * swap internal pivot values to ends.
             */
            if (less < e1 && e5 < great) {
                /*
                 * Skip elements, which are equal to pivot values.
                 */
                while (a[less] == pivot1) {
                    ++less;
                }

                while (a[great] == pivot2) {
                    --great;
                }

                /*
                 * Partitioning:
                 *
                 *   left part         center part                  right part
                 * +----------------------------------------------------------+
                 * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                 * +----------------------------------------------------------+
                 *              ^                        ^       ^
                 *              |                        |       |
                 *             less                      k     great
                 *
                 * Invariants:
                 *
                 *              all in (*,  less) == pivot1
                 *     pivot1 < all in [less,  k)  < pivot2
                 *              all in (great, *) == pivot2
                 *
                 * Pointer k is the first index of ?-part.
                 */
                outer2:
                for (k = less - 1; ++k <= great; ) {
                    int ak = a[k];
                    if (ak == pivot1) { // Move a[k] to left part
                        a[k] = a[less];
                        a[less] = ak;
                        ++less;
                    } else if (ak == pivot2) { // Move a[k] to right part
                        while (a[great] == pivot2) {
                            if (great-- == k) {
                                goto outer2;
                            }
                        }
                        if (a[great] == pivot1) { // a[great] < pivot2
                            a[k] = a[less];
                            /*
                             * Even though a[great] equals to pivot1, the
                             * assignment a[less] = pivot1 may be incorrect,
                             * if a[great] and pivot1 are floating-point zeros
                             * of different signs. Therefore in float and
                             * double sorting methods we have to use more
                             * accurate assignment a[less] = a[great].
                             */
                            a[less] = pivot1;
                            ++less;
                        } else { // pivot1 < a[great] < pivot2
                            a[k] = a[great];
                        }
                        a[great] = ak;
                        --great;
                    }
                }
            }

            // Sort center part recursively
            mysort_int_inner(a, less, great, 0);

        } else { // Partitioning with one pivot
            /*
             * Use the third of the five sorted elements as pivot.
             * This value is inexpensive approximation of the median.
             */
            int pivot = a[e3];

            /*
             * Partitioning degenerates to the traditional 3-way
             * (or "Dutch National Flag") schema:
             *
             *   left part    center part              right part
             * +-------------------------------------------------+
             * |  < pivot  |   == pivot   |     ?    |  > pivot  |
             * +-------------------------------------------------+
             *              ^              ^        ^
             *              |              |        |
             *             less            k      great
             *
             * Invariants:
             *
             *   all in (left, less)   < pivot
             *   all in [less, k)     == pivot
             *   all in (great, right) > pivot
             *
             * Pointer k is the first index of ?-part.
             */
            for (k = less; k <= great; ++k) {
                if (a[k] == pivot) {
                    continue;
                }
                int ak = a[k];
                if (ak < pivot) { // Move a[k] to left part
                    a[k] = a[less];
                    a[less] = ak;
                    ++less;
                } else { // a[k] > pivot - Move a[k] to right part
                    while (a[great] > pivot) {
                        --great;
                    }
                    if (a[great] < pivot) { // a[great] <= pivot
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // a[great] == pivot
                        /*
                         * Even though a[great] equals to pivot, the
                         * assignment a[k] = pivot may be incorrect,
                         * if a[great] and pivot are floating-point
                         * zeros of different signs. Therefore in float
                         * and double sorting methods we have to use
                         * more accurate assignment a[k] = a[great].
                         */
                        a[k] = pivot;
                    }
                    a[great] = ak;
                    --great;
                }
            }

            /*
             * Sort left and right parts recursively.
             * All elements from center part are equal
             * and, therefore, already sorted.
             */
            mysort_int_inner(a, left, less - 1, leftmost);
            mysort_int_inner(a, great + 1, right, 0);
        }
}

void mysort_long(int64_t *a, int left, int length){
        int right=left+length-1, i, j, k, m, n;

        // Use Quicksort on small arrays
        if (right - left < QUICKSORT_THRESHOLD) {
            mysort_long_inner(a, left, right, true);
            return;
        }

        /*
         * Index run[i] is the start of i-th run
         * (ascending or descending sequence).
         */
        //int[] run = new int[MAX_RUN_COUNT + 1];
        int *run=my_new2((MAX_RUN_COUNT + 1)*sizeof(int));
        int count = 0; run[0] = left;

        // Check if the array is nearly sorted
        for (k = left; k < right; run[count] = k) {
            if (a[k] < a[k + 1]) { // ascending
                while (++k <= right && a[k - 1] <= a[k]);
            } else if (a[k] > a[k + 1]) { // descending
                while (++k <= right && a[k - 1] >= a[k]);
                int lo, hi;
                for (lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                    int64_t t = a[lo]; a[lo] = a[hi]; a[hi] = t;
                }
            } else { // equal
                for (m = MAX_RUN_LENGTH; ++k <= right && a[k - 1] == a[k]; ) {
                    if (--m == 0) {
                        mysort_long_inner(a, left, right, true);
                        free(run);
                        return;
                    }
                }
            }

            /*
             * The array is not highly structured,
             * use Quicksort instead of merge sort.
             */
            if (++count == MAX_RUN_COUNT) {
                mysort_long_inner(a, left, right, true);
                free(run);
                return;
            }
        }

        // Check special cases
        // Implementation note: variable "right" is increased by 1.
        if (run[count] == right++) { // The last run contains one element
            run[++count] = right;
        } else if (count == 1) { // The array is already sorted
            free(run);
            return;
        }

        // Determine alternation base for merge
        char odd = 0;
        for (n = 1; (n <<= 1) < count; odd ^= 1);

        // Use or create temporary array b for merging
        int64_t *b;                 // temp array; alternates with a
        int ao, bo;              // array offsets from 'left'
        int blen = right - left; // space needed for b

        int64_t *work=my_new2(blen*sizeof(int64_t));
        int workBase=0;

        if (odd == 0) {
            //System.arraycopy(a, left, work, workBase, blen);
            for(i=0;i<blen;i++){
                work[i+workBase]=a[i+left];
            }
            b = a;
            bo = 0;
            a = work;
            ao = workBase - left;
        } else {
            b = work;
            ao = 0;
            bo = workBase - left;
        }

        // Merging
        int last;
        for (; count > 1; count = last) {
            for (k = (last = 0) + 2; k <= count; k += 2) {
                int hi = run[k], mi = run[k - 1];
                int p, q;
                for (i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                    if (q >= hi || p < mi && a[p + ao] <= a[q + ao]) {
                        b[i + bo] = a[p++ + ao];
                    } else {
                        b[i + bo] = a[q++ + ao];
                    }
                }
                run[++last] = hi;
            }
            if ((count & 1) != 0) {
                int lo;
                for (i = right, lo = run[count - 1]; --i >= lo;
                    b[i + bo] = a[i + ao]
                );
                run[++last] = right;
            }
            int64_t *t = a; a = b; b = t;
            int o = ao; ao = bo; bo = o;
        }

        free(work);
        free(run);
}

void mysort_long_inner(int64_t *a, int left, int right, int leftmost){
        int length = right - left + 1, i, j, k, m, n;

        // Use insertion sort on tiny arrays
        if (length < INSERTION_SORT_THRESHOLD) {
            if (leftmost) {
                /*
                 * Traditional (without sentinel) insertion sort,
                 * optimized for server VM, is used in case of
                 * the leftmost part.
                 */
                for (i = left, j = i; i < right; j = ++i) {
                    int64_t ai = a[i + 1];
                    while (ai < a[j]) {
                        a[j + 1] = a[j];
                        if (j-- == left) {
                            break;
                        }
                    }
                    a[j + 1] = ai;
                }
            } else {
                /*
                 * Skip the longest ascending sequence.
                 */
                do {
                    if (left >= right) {
                        return;
                    }
                } while (a[++left] >= a[left - 1]);

                /*
                 * Every element from adjoining part plays the role
                 * of sentinel, therefore this allows us to avoid the
                 * left range check on each iteration. Moreover, we use
                 * the more optimized algorithm, so called pair insertion
                 * sort, which is faster (in the context of Quicksort)
                 * than traditional implementation of insertion sort.
                 */
                for (k = left; ++left <= right; k = ++left) {
                    int64_t a1 = a[k], a2 = a[left];

                    if (a1 < a2) {
                        a2 = a1; a1 = a[left];
                    }
                    while (a1 < a[--k]) {
                        a[k + 2] = a[k];
                    }
                    a[++k + 1] = a1;

                    while (a2 < a[--k]) {
                        a[k + 1] = a[k];
                    }
                    a[k + 1] = a2;
                }
                int64_t last = a[right];

                while (last < a[--right]) {
                    a[right + 1] = a[right];
                }
                a[right + 1] = last;
            }
            return;
        }

        // Inexpensive approximation of length / 7
        int seventh = (length >> 3) + (length >> 6) + 1;

        /*
         * Sort five evenly spaced elements around (and including) the
         * center element in the range. These elements will be used for
         * pivot selection as described below. The choice for spacing
         * these elements was empirically determined to work well on
         * a wide variety of inputs.
         */
        int e3 = (left + right) >> 1; // The midpoint
        int e2 = e3 - seventh;
        int e1 = e2 - seventh;
        int e4 = e3 + seventh;
        int e5 = e4 + seventh;

        // Sort these elements using insertion sort
        if (a[e2] < a[e1]) { int64_t t = a[e2]; a[e2] = a[e1]; a[e1] = t; }

        if (a[e3] < a[e2]) { int64_t t = a[e3]; a[e3] = a[e2]; a[e2] = t;
            if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
        }
        if (a[e4] < a[e3]) { int64_t t = a[e4]; a[e4] = a[e3]; a[e3] = t;
            if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
            }
        }
        if (a[e5] < a[e4]) { int64_t t = a[e5]; a[e5] = a[e4]; a[e4] = t;
            if (t < a[e3]) { a[e4] = a[e3]; a[e3] = t;
                if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                    if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
                }
            }
        }

        // Pointers
        int less  = left;  // The index of the first element of center part
        int great = right; // The index before the first element of right part

        if (a[e1] != a[e2] && a[e2] != a[e3] && a[e3] != a[e4] && a[e4] != a[e5]) {
            /*
             * Use the second and fourth of the five sorted elements as pivots.
             * These values are inexpensive approximations of the first and
             * second terciles of the array. Note that pivot1 <= pivot2.
             */
            int64_t pivot1 = a[e2];
            int64_t pivot2 = a[e4];

            /*
             * The first and the last elements to be sorted are moved to the
             * locations formerly occupied by the pivots. When partitioning
             * is complete, the pivots are swapped back into their final
             * positions, and excluded from subsequent sorting.
             */
            a[e2] = a[left];
            a[e4] = a[right];

            /*
             * Skip elements, which are less or greater than pivot values.
             */
            while (a[++less] < pivot1);
            while (a[--great] > pivot2);

            /*
             * Partitioning:
             *
             *   left part           center part                   right part
             * +--------------------------------------------------------------+
             * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
             * +--------------------------------------------------------------+
             *               ^                          ^       ^
             *               |                          |       |
             *              less                        k     great
             *
             * Invariants:
             *
             *              all in (left, less)   < pivot1
             *    pivot1 <= all in [less, k)     <= pivot2
             *              all in (great, right) > pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            outer1:
            for (k = less - 1; ++k <= great; ) {
                int64_t ak = a[k];
                if (ak < pivot1) { // Move a[k] to left part
                    a[k] = a[less];
                    /*
                     * Here and below we use "a[i] = b; i++;" instead
                     * of "a[i++] = b;" due to performance issue.
                     */
                    a[less] = ak;
                    ++less;
                } else if (ak > pivot2) { // Move a[k] to right part
                    while (a[great] > pivot2) {
                        if (great-- == k) {
                            goto outer1;
                        }
                    }
                    if (a[great] < pivot1) { // a[great] <= pivot2
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // pivot1 <= a[great] <= pivot2
                        a[k] = a[great];
                    }
                    /*
                     * Here and below we use "a[i] = b; i--;" instead
                     * of "a[i--] = b;" due to performance issue.
                     */
                    a[great] = ak;
                    --great;
                }
            }

            // Swap pivots into their final positions
            a[left]  = a[less  - 1]; a[less  - 1] = pivot1;
            a[right] = a[great + 1]; a[great + 1] = pivot2;

            // Sort left and right parts recursively, excluding known pivots
            mysort_long_inner(a, left, less - 2, leftmost);
            mysort_long_inner(a, great + 2, right, false);

            /*
             * If center part is too large (comprises > 4/7 of the array),
             * swap internal pivot values to ends.
             */
            if (less < e1 && e5 < great) {
                /*
                 * Skip elements, which are equal to pivot values.
                 */
                while (a[less] == pivot1) {
                    ++less;
                }

                while (a[great] == pivot2) {
                    --great;
                }

                /*
                 * Partitioning:
                 *
                 *   left part         center part                  right part
                 * +----------------------------------------------------------+
                 * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                 * +----------------------------------------------------------+
                 *              ^                        ^       ^
                 *              |                        |       |
                 *             less                      k     great
                 *
                 * Invariants:
                 *
                 *              all in (*,  less) == pivot1
                 *     pivot1 < all in [less,  k)  < pivot2
                 *              all in (great, *) == pivot2
                 *
                 * Pointer k is the first index of ?-part.
                 */
                outer2:
                for (k = less - 1; ++k <= great; ) {
                    int64_t ak = a[k];
                    if (ak == pivot1) { // Move a[k] to left part
                        a[k] = a[less];
                        a[less] = ak;
                        ++less;
                    } else if (ak == pivot2) { // Move a[k] to right part
                        while (a[great] == pivot2) {
                            if (great-- == k) {
                                goto outer2;
                            }
                        }
                        if (a[great] == pivot1) { // a[great] < pivot2
                            a[k] = a[less];
                            /*
                             * Even though a[great] equals to pivot1, the
                             * assignment a[less] = pivot1 may be incorrect,
                             * if a[great] and pivot1 are floating-point zeros
                             * of different signs. Therefore in float and
                             * double sorting methods we have to use more
                             * accurate assignment a[less] = a[great].
                             */
                            a[less] = pivot1;
                            ++less;
                        } else { // pivot1 < a[great] < pivot2
                            a[k] = a[great];
                        }
                        a[great] = ak;
                        --great;
                    }
                }
            }

            // Sort center part recursively
            mysort_long_inner(a, less, great, false);

        } else { // Partitioning with one pivot
            /*
             * Use the third of the five sorted elements as pivot.
             * This value is inexpensive approximation of the median.
             */
            int64_t pivot = a[e3];

            /*
             * Partitioning degenerates to the traditional 3-way
             * (or "Dutch National Flag") schema:
             *
             *   left part    center part              right part
             * +-------------------------------------------------+
             * |  < pivot  |   == pivot   |     ?    |  > pivot  |
             * +-------------------------------------------------+
             *              ^              ^        ^
             *              |              |        |
             *             less            k      great
             *
             * Invariants:
             *
             *   all in (left, less)   < pivot
             *   all in [less, k)     == pivot
             *   all in (great, right) > pivot
             *
             * Pointer k is the first index of ?-part.
             */
            for (k = less; k <= great; ++k) {
                if (a[k] == pivot) {
                    continue;
                }
                int64_t ak = a[k];
                if (ak < pivot) { // Move a[k] to left part
                    a[k] = a[less];
                    a[less] = ak;
                    ++less;
                } else { // a[k] > pivot - Move a[k] to right part
                    while (a[great] > pivot) {
                        --great;
                    }
                    if (a[great] < pivot) { // a[great] <= pivot
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // a[great] == pivot
                        /*
                         * Even though a[great] equals to pivot, the
                         * assignment a[k] = pivot may be incorrect,
                         * if a[great] and pivot are floating-point
                         * zeros of different signs. Therefore in float
                         * and double sorting methods we have to use
                         * more accurate assignment a[k] = a[great].
                         */
                        a[k] = pivot;
                    }
                    a[great] = ak;
                    --great;
                }
            }

            /*
             * Sort left and right parts recursively.
             * All elements from center part are equal
             * and, therefore, already sorted.
             */
            mysort_long_inner(a, left, less - 1, leftmost);
            mysort_long_inner(a, great + 1, right, false);
        }
}

void mysort_longlong(int64_t *a, int left, int length){
        int right=left+length-1, i, j, k, m, n;

        // Use Quicksort on small arrays
        if (right - left < QUICKSORT_THRESHOLD) {
            mysort_longlong_inner(a, left, right, true);
            return;
        }

        /*
         * Index run[i] is the start of i-th run
         * (ascending or descending sequence).
         */
        //int[] run = new int[MAX_RUN_COUNT + 1];
        int *run=my_new2((MAX_RUN_COUNT + 1)*sizeof(int));
        int count = 0; run[0] = left;

        // Check if the array is nearly sorted
        for (k = left; k < right; run[count] = k) {
            if (a[k] < a[k + 1]) { // ascending
                while (++k <= right && a[k - 1] <= a[k]);
            } else if (a[k] > a[k + 1]) { // descending
                while (++k <= right && a[k - 1] >= a[k]);
                int lo, hi;
                for (lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                    int64_t t = a[lo]; a[lo] = a[hi]; a[hi] = t;
                }
            } else { // equal
                for (m = MAX_RUN_LENGTH; ++k <= right && a[k - 1] == a[k]; ) {
                    if (--m == 0) {
                        mysort_longlong_inner(a, left, right, true);
                        free(run);
                        return;
                    }
                }
            }

            /*
             * The array is not highly structured,
             * use Quicksort instead of merge sort.
             */
            if (++count == MAX_RUN_COUNT) {
                mysort_longlong_inner(a, left, right, true);
                free(run);
                return;
            }
        }

        // Check special cases
        // Implementation note: variable "right" is increased by 1.
        if (run[count] == right++) { // The last run contains one element
            run[++count] = right;
        } else if (count == 1) { // The array is already sorted
            free(run);
            return;
        }

        // Determine alternation base for merge
        char odd = 0;
        for (n = 1; (n <<= 1) < count; odd ^= 1);

        // Use or create temporary array b for merging
        int64_t *b;                 // temp array; alternates with a
        int ao, bo;              // array offsets from 'left'
        int blen = right - left; // space needed for b

        int64_t *work=my_new2(blen*sizeof(int64_t));
        int workBase=0;

        if (odd == 0) {
            //System.arraycopy(a, left, work, workBase, blen);
            for(i=0;i<blen;i++){
                work[i+workBase]=a[i+left];
            }
            b = a;
            bo = 0;
            a = work;
            ao = workBase - left;
        } else {
            b = work;
            ao = 0;
            bo = workBase - left;
        }

        // Merging
        int last;
        for (; count > 1; count = last) {
            for (k = (last = 0) + 2; k <= count; k += 2) {
                int hi = run[k], mi = run[k - 1];
                int p, q;
                for (i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                    if (q >= hi || p < mi && a[p + ao] <= a[q + ao]) {
                        b[i + bo] = a[p++ + ao];
                    } else {
                        b[i + bo] = a[q++ + ao];
                    }
                }
                run[++last] = hi;
            }
            if ((count & 1) != 0) {
                int lo;
                for (i = right, lo = run[count - 1]; --i >= lo;
                    b[i + bo] = a[i + ao]
                );
                run[++last] = right;
            }
            int64_t *t = a; a = b; b = t;
            int o = ao; ao = bo; bo = o;
        }

        free(work);
        free(run);
}

void mysort_longlong_inner(int64_t *a, int left, int right, int leftmost){
        int length = right - left + 1, i, j, k, m, n;

        // Use insertion sort on tiny arrays
        if (length < INSERTION_SORT_THRESHOLD) {
            if (leftmost) {
                /*
                 * Traditional (without sentinel) insertion sort,
                 * optimized for server VM, is used in case of
                 * the leftmost part.
                 */
                for (i = left, j = i; i < right; j = ++i) {
                    int64_t ai = a[i + 1];
                    while (ai < a[j]) {
                        a[j + 1] = a[j];
                        if (j-- == left) {
                            break;
                        }
                    }
                    a[j + 1] = ai;
                }
            } else {
                /*
                 * Skip the longest ascending sequence.
                 */
                do {
                    if (left >= right) {
                        return;
                    }
                } while (a[++left] >= a[left - 1]);

                /*
                 * Every element from adjoining part plays the role
                 * of sentinel, therefore this allows us to avoid the
                 * left range check on each iteration. Moreover, we use
                 * the more optimized algorithm, so called pair insertion
                 * sort, which is faster (in the context of Quicksort)
                 * than traditional implementation of insertion sort.
                 */
                for (k = left; ++left <= right; k = ++left) {
                    int64_t a1 = a[k], a2 = a[left];

                    if (a1 < a2) {
                        a2 = a1; a1 = a[left];
                    }
                    while (a1 < a[--k]) {
                        a[k + 2] = a[k];
                    }
                    a[++k + 1] = a1;

                    while (a2 < a[--k]) {
                        a[k + 1] = a[k];
                    }
                    a[k + 1] = a2;
                }
                int64_t last = a[right];

                while (last < a[--right]) {
                    a[right + 1] = a[right];
                }
                a[right + 1] = last;
            }
            return;
        }

        // Inexpensive approximation of length / 7
        int seventh = (length >> 3) + (length >> 6) + 1;

        /*
         * Sort five evenly spaced elements around (and including) the
         * center element in the range. These elements will be used for
         * pivot selection as described below. The choice for spacing
         * these elements was empirically determined to work well on
         * a wide variety of inputs.
         */
        int e3 = (left + right) >> 1; // The midpoint
        int e2 = e3 - seventh;
        int e1 = e2 - seventh;
        int e4 = e3 + seventh;
        int e5 = e4 + seventh;

        // Sort these elements using insertion sort
        if (a[e2] < a[e1]) { int64_t t = a[e2]; a[e2] = a[e1]; a[e1] = t; }

        if (a[e3] < a[e2]) { int64_t t = a[e3]; a[e3] = a[e2]; a[e2] = t;
            if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
        }
        if (a[e4] < a[e3]) { int64_t t = a[e4]; a[e4] = a[e3]; a[e3] = t;
            if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
            }
        }
        if (a[e5] < a[e4]) { int64_t t = a[e5]; a[e5] = a[e4]; a[e4] = t;
            if (t < a[e3]) { a[e4] = a[e3]; a[e3] = t;
                if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                    if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
                }
            }
        }

        // Pointers
        int less  = left;  // The index of the first element of center part
        int great = right; // The index before the first element of right part

        if (a[e1] != a[e2] && a[e2] != a[e3] && a[e3] != a[e4] && a[e4] != a[e5]) {
            /*
             * Use the second and fourth of the five sorted elements as pivots.
             * These values are inexpensive approximations of the first and
             * second terciles of the array. Note that pivot1 <= pivot2.
             */
            int64_t pivot1 = a[e2];
            int64_t pivot2 = a[e4];

            /*
             * The first and the last elements to be sorted are moved to the
             * locations formerly occupied by the pivots. When partitioning
             * is complete, the pivots are swapped back into their final
             * positions, and excluded from subsequent sorting.
             */
            a[e2] = a[left];
            a[e4] = a[right];

            /*
             * Skip elements, which are less or greater than pivot values.
             */
            while (a[++less] < pivot1);
            while (a[--great] > pivot2);

            /*
             * Partitioning:
             *
             *   left part           center part                   right part
             * +--------------------------------------------------------------+
             * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
             * +--------------------------------------------------------------+
             *               ^                          ^       ^
             *               |                          |       |
             *              less                        k     great
             *
             * Invariants:
             *
             *              all in (left, less)   < pivot1
             *    pivot1 <= all in [less, k)     <= pivot2
             *              all in (great, right) > pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            outer1:
            for (k = less - 1; ++k <= great; ) {
                int64_t ak = a[k];
                if (ak < pivot1) { // Move a[k] to left part
                    a[k] = a[less];
                    /*
                     * Here and below we use "a[i] = b; i++;" instead
                     * of "a[i++] = b;" due to performance issue.
                     */
                    a[less] = ak;
                    ++less;
                } else if (ak > pivot2) { // Move a[k] to right part
                    while (a[great] > pivot2) {
                        if (great-- == k) {
                            goto outer1;
                        }
                    }
                    if (a[great] < pivot1) { // a[great] <= pivot2
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // pivot1 <= a[great] <= pivot2
                        a[k] = a[great];
                    }
                    /*
                     * Here and below we use "a[i] = b; i--;" instead
                     * of "a[i--] = b;" due to performance issue.
                     */
                    a[great] = ak;
                    --great;
                }
            }

            // Swap pivots into their final positions
            a[left]  = a[less  - 1]; a[less  - 1] = pivot1;
            a[right] = a[great + 1]; a[great + 1] = pivot2;

            // Sort left and right parts recursively, excluding known pivots
            mysort_longlong_inner(a, left, less - 2, leftmost);
            mysort_longlong_inner(a, great + 2, right, false);

            /*
             * If center part is too large (comprises > 4/7 of the array),
             * swap internal pivot values to ends.
             */
            if (less < e1 && e5 < great) {
                /*
                 * Skip elements, which are equal to pivot values.
                 */
                while (a[less] == pivot1) {
                    ++less;
                }

                while (a[great] == pivot2) {
                    --great;
                }

                /*
                 * Partitioning:
                 *
                 *   left part         center part                  right part
                 * +----------------------------------------------------------+
                 * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                 * +----------------------------------------------------------+
                 *              ^                        ^       ^
                 *              |                        |       |
                 *             less                      k     great
                 *
                 * Invariants:
                 *
                 *              all in (*,  less) == pivot1
                 *     pivot1 < all in [less,  k)  < pivot2
                 *              all in (great, *) == pivot2
                 *
                 * Pointer k is the first index of ?-part.
                 */
                outer2:
                for (k = less - 1; ++k <= great; ) {
                    int64_t ak = a[k];
                    if (ak == pivot1) { // Move a[k] to left part
                        a[k] = a[less];
                        a[less] = ak;
                        ++less;
                    } else if (ak == pivot2) { // Move a[k] to right part
                        while (a[great] == pivot2) {
                            if (great-- == k) {
                                goto outer2;
                            }
                        }
                        if (a[great] == pivot1) { // a[great] < pivot2
                            a[k] = a[less];
                            /*
                             * Even though a[great] equals to pivot1, the
                             * assignment a[less] = pivot1 may be incorrect,
                             * if a[great] and pivot1 are floating-point zeros
                             * of different signs. Therefore in float and
                             * double sorting methods we have to use more
                             * accurate assignment a[less] = a[great].
                             */
                            a[less] = pivot1;
                            ++less;
                        } else { // pivot1 < a[great] < pivot2
                            a[k] = a[great];
                        }
                        a[great] = ak;
                        --great;
                    }
                }
            }

            // Sort center part recursively
            mysort_longlong_inner(a, less, great, false);

        } else { // Partitioning with one pivot
            /*
             * Use the third of the five sorted elements as pivot.
             * This value is inexpensive approximation of the median.
             */
            int64_t pivot = a[e3];

            /*
             * Partitioning degenerates to the traditional 3-way
             * (or "Dutch National Flag") schema:
             *
             *   left part    center part              right part
             * +-------------------------------------------------+
             * |  < pivot  |   == pivot   |     ?    |  > pivot  |
             * +-------------------------------------------------+
             *              ^              ^        ^
             *              |              |        |
             *             less            k      great
             *
             * Invariants:
             *
             *   all in (left, less)   < pivot
             *   all in [less, k)     == pivot
             *   all in (great, right) > pivot
             *
             * Pointer k is the first index of ?-part.
             */
            for (k = less; k <= great; ++k) {
                if (a[k] == pivot) {
                    continue;
                }
                int64_t ak = a[k];
                if (ak < pivot) { // Move a[k] to left part
                    a[k] = a[less];
                    a[less] = ak;
                    ++less;
                } else { // a[k] > pivot - Move a[k] to right part
                    while (a[great] > pivot) {
                        --great;
                    }
                    if (a[great] < pivot) { // a[great] <= pivot
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // a[great] == pivot
                        /*
                         * Even though a[great] equals to pivot, the
                         * assignment a[k] = pivot may be incorrect,
                         * if a[great] and pivot are floating-point
                         * zeros of different signs. Therefore in float
                         * and double sorting methods we have to use
                         * more accurate assignment a[k] = a[great].
                         */
                        a[k] = pivot;
                    }
                    a[great] = ak;
                    --great;
                }
            }

            /*
             * Sort left and right parts recursively.
             * All elements from center part are equal
             * and, therefore, already sorted.
             */
            mysort_longlong_inner(a, left, less - 1, leftmost);
            mysort_longlong_inner(a, great + 1, right, false);
        }
}

void mysort_short(short *a, int left, int length){
        int right=left+length-1, i, j, k, m, n;

        int shiftSize=sizeof(short)*8-1;
        short minValue=1<<shiftSize;

        // Use counting sort on large arrays
        if (right - left > COUNTING_SORT_THRESHOLD_FOR_SHORT_OR_CHAR) {
            //int[] count = new int[NUM_SHORT_VALUES];
            int *count=my_new2(NUM_SHORT_VALUES*sizeof(int));

            for (i = left - 1; ++i <= right;

                count[a[i] - minValue]++
            );
            for (i = NUM_SHORT_VALUES, k = right + 1; k > left; ) {
                while (count[--i] == 0);
                short value = (short) (i + minValue);
                int s = count[i];

                do {
                    a[--k] = value;
                } while (--s > 0);
            }

            free(count);
        } else { // Use Dual-Pivot Quicksort on small arrays
            mysort_short_doSort(a, left, right, NULL, 0, 0);
        }

}

void mysort_short_doSort(short *a, int left, int right, short *work, int workBase, int workLen){
        int i, j, k, m, n;

        // Use Quicksort on small arrays
        if (right - left < QUICKSORT_THRESHOLD) {
            mysort_short_inner(a, left, right, true);
            return;
        }

        /*
         * Index run[i] is the start of i-th run
         * (ascending or descending sequence).
         */
        //int[] run = new int[MAX_RUN_COUNT + 1];
        int *run=my_new2((MAX_RUN_COUNT + 1)*sizeof(int));
        int count = 0; run[0] = left;

        // Check if the array is nearly sorted
        for (k = left; k < right; run[count] = k) {
            if (a[k] < a[k + 1]) { // ascending
                while (++k <= right && a[k - 1] <= a[k]);
            } else if (a[k] > a[k + 1]) { // descending
                while (++k <= right && a[k - 1] >= a[k]);
                int lo, hi;
                for (lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                    short t = a[lo]; a[lo] = a[hi]; a[hi] = t;
                }
            } else { // equal
                for (m = MAX_RUN_LENGTH; ++k <= right && a[k - 1] == a[k]; ) {
                    if (--m == 0) {
                        mysort_short_inner(a, left, right, true);
                        free(run);
                        return;
                    }
                }
            }

            /*
             * The array is not highly structured,
             * use Quicksort instead of merge sort.
             */
            if (++count == MAX_RUN_COUNT) {
                mysort_short_inner(a, left, right, true);
                free(run);
                return;
            }
        }

        // Check special cases
        // Implementation note: variable "right" is increased by 1.
        if (run[count] == right++) { // The last run contains one element
            run[++count] = right;
        } else if (count == 1) { // The array is already sorted
            free(run);
            return;
        }

        // Determine alternation base for merge
        char odd = 0;
        for (n = 1; (n <<= 1) < count; odd ^= 1);

        // Use or create temporary array b for merging
        short *b;                 // temp array; alternates with a
        int ao, bo;              // array offsets from 'left'
        int blen = right - left; // space needed for b

        work=my_new2(blen*sizeof(short));
        workBase=0;

        if (odd == 0) {
            //System.arraycopy(a, left, work, workBase, blen);
            for(i=0;i<blen;i++){
                work[i+workBase]=a[i+left];
            }

            b = a;
            bo = 0;
            a = work;
            ao = workBase - left;
        } else {
            b = work;
            ao = 0;
            bo = workBase - left;
        }

        // Merging
        int last;
        for (; count > 1; count = last) {
            for (k = (last = 0) + 2; k <= count; k += 2) {
                int hi = run[k], mi = run[k - 1];
                int p, q;
                for (i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                    if (q >= hi || p < mi && a[p + ao] <= a[q + ao]) {
                        b[i + bo] = a[p++ + ao];
                    } else {
                        b[i + bo] = a[q++ + ao];
                    }
                }
                run[++last] = hi;
            }
            if ((count & 1) != 0) {
                int lo;
                for (i = right, lo = run[count - 1]; --i >= lo;
                    b[i + bo] = a[i + ao]
                );
                run[++last] = right;
            }
            short *t = a; a = b; b = t;
            int o = ao; ao = bo; bo = o;
        }

        free(work);
        free(run);
}

void mysort_short_inner(short *a, int left, int right, int leftmost){
        int length = right - left + 1, i, j, k, m, n;

        // Use insertion sort on tiny arrays
        if (length < INSERTION_SORT_THRESHOLD) {
            if (leftmost) {
                /*
                 * Traditional (without sentinel) insertion sort,
                 * optimized for server VM, is used in case of
                 * the leftmost part.
                 */
                for (i = left, j = i; i < right; j = ++i) {
                    short ai = a[i + 1];
                    while (ai < a[j]) {
                        a[j + 1] = a[j];
                        if (j-- == left) {
                            break;
                        }
                    }
                    a[j + 1] = ai;
                }
            } else {
                /*
                 * Skip the longest ascending sequence.
                 */
                do {
                    if (left >= right) {
                        return;
                    }
                } while (a[++left] >= a[left - 1]);

                /*
                 * Every element from adjoining part plays the role
                 * of sentinel, therefore this allows us to avoid the
                 * left range check on each iteration. Moreover, we use
                 * the more optimized algorithm, so called pair insertion
                 * sort, which is faster (in the context of Quicksort)
                 * than traditional implementation of insertion sort.
                 */
                for (k = left; ++left <= right; k = ++left) {
                    short a1 = a[k], a2 = a[left];

                    if (a1 < a2) {
                        a2 = a1; a1 = a[left];
                    }
                    while (a1 < a[--k]) {
                        a[k + 2] = a[k];
                    }
                    a[++k + 1] = a1;

                    while (a2 < a[--k]) {
                        a[k + 1] = a[k];
                    }
                    a[k + 1] = a2;
                }
                short last = a[right];

                while (last < a[--right]) {
                    a[right + 1] = a[right];
                }
                a[right + 1] = last;
            }
            return;
        }

        // Inexpensive approximation of length / 7
        int seventh = (length >> 3) + (length >> 6) + 1;

        /*
         * Sort five evenly spaced elements around (and including) the
         * center element in the range. These elements will be used for
         * pivot selection as described below. The choice for spacing
         * these elements was empirically determined to work well on
         * a wide variety of inputs.
         */
        int e3 = (left + right) >> 1; // The midpoint
        int e2 = e3 - seventh;
        int e1 = e2 - seventh;
        int e4 = e3 + seventh;
        int e5 = e4 + seventh;

        // Sort these elements using insertion sort
        if (a[e2] < a[e1]) { short t = a[e2]; a[e2] = a[e1]; a[e1] = t; }

        if (a[e3] < a[e2]) { short t = a[e3]; a[e3] = a[e2]; a[e2] = t;
            if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
        }
        if (a[e4] < a[e3]) { short t = a[e4]; a[e4] = a[e3]; a[e3] = t;
            if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
            }
        }
        if (a[e5] < a[e4]) { short t = a[e5]; a[e5] = a[e4]; a[e4] = t;
            if (t < a[e3]) { a[e4] = a[e3]; a[e3] = t;
                if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                    if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
                }
            }
        }

        // Pointers
        int less  = left;  // The index of the first element of center part
        int great = right; // The index before the first element of right part

        if (a[e1] != a[e2] && a[e2] != a[e3] && a[e3] != a[e4] && a[e4] != a[e5]) {
            /*
             * Use the second and fourth of the five sorted elements as pivots.
             * These values are inexpensive approximations of the first and
             * second terciles of the array. Note that pivot1 <= pivot2.
             */
            short pivot1 = a[e2];
            short pivot2 = a[e4];

            /*
             * The first and the last elements to be sorted are moved to the
             * locations formerly occupied by the pivots. When partitioning
             * is complete, the pivots are swapped back into their final
             * positions, and excluded from subsequent sorting.
             */
            a[e2] = a[left];
            a[e4] = a[right];

            /*
             * Skip elements, which are less or greater than pivot values.
             */
            while (a[++less] < pivot1);
            while (a[--great] > pivot2);

            /*
             * Partitioning:
             *
             *   left part           center part                   right part
             * +--------------------------------------------------------------+
             * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
             * +--------------------------------------------------------------+
             *               ^                          ^       ^
             *               |                          |       |
             *              less                        k     great
             *
             * Invariants:
             *
             *              all in (left, less)   < pivot1
             *    pivot1 <= all in [less, k)     <= pivot2
             *              all in (great, right) > pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            outer1:
            for (k = less - 1; ++k <= great; ) {
                short ak = a[k];
                if (ak < pivot1) { // Move a[k] to left part
                    a[k] = a[less];
                    /*
                     * Here and below we use "a[i] = b; i++;" instead
                     * of "a[i++] = b;" due to performance issue.
                     */
                    a[less] = ak;
                    ++less;
                } else if (ak > pivot2) { // Move a[k] to right part
                    while (a[great] > pivot2) {
                        if (great-- == k) {
                            goto outer1;
                        }
                    }
                    if (a[great] < pivot1) { // a[great] <= pivot2
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // pivot1 <= a[great] <= pivot2
                        a[k] = a[great];
                    }
                    /*
                     * Here and below we use "a[i] = b; i--;" instead
                     * of "a[i--] = b;" due to performance issue.
                     */
                    a[great] = ak;
                    --great;
                }
            }

            // Swap pivots into their final positions
            a[left]  = a[less  - 1]; a[less  - 1] = pivot1;
            a[right] = a[great + 1]; a[great + 1] = pivot2;

            // Sort left and right parts recursively, excluding known pivots
            mysort_short_inner(a, left, less - 2, leftmost);
            mysort_short_inner(a, great + 2, right, false);

            /*
             * If center part is too large (comprises > 4/7 of the array),
             * swap internal pivot values to ends.
             */
            if (less < e1 && e5 < great) {
                /*
                 * Skip elements, which are equal to pivot values.
                 */
                while (a[less] == pivot1) {
                    ++less;
                }

                while (a[great] == pivot2) {
                    --great;
                }

                /*
                 * Partitioning:
                 *
                 *   left part         center part                  right part
                 * +----------------------------------------------------------+
                 * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                 * +----------------------------------------------------------+
                 *              ^                        ^       ^
                 *              |                        |       |
                 *             less                      k     great
                 *
                 * Invariants:
                 *
                 *              all in (*,  less) == pivot1
                 *     pivot1 < all in [less,  k)  < pivot2
                 *              all in (great, *) == pivot2
                 *
                 * Pointer k is the first index of ?-part.
                 */
                outer2:
                for (k = less - 1; ++k <= great; ) {
                    short ak = a[k];
                    if (ak == pivot1) { // Move a[k] to left part
                        a[k] = a[less];
                        a[less] = ak;
                        ++less;
                    } else if (ak == pivot2) { // Move a[k] to right part
                        while (a[great] == pivot2) {
                            if (great-- == k) {
                                goto outer2;
                            }
                        }
                        if (a[great] == pivot1) { // a[great] < pivot2
                            a[k] = a[less];
                            /*
                             * Even though a[great] equals to pivot1, the
                             * assignment a[less] = pivot1 may be incorrect,
                             * if a[great] and pivot1 are floating-point zeros
                             * of different signs. Therefore in float and
                             * double sorting methods we have to use more
                             * accurate assignment a[less] = a[great].
                             */
                            a[less] = pivot1;
                            ++less;
                        } else { // pivot1 < a[great] < pivot2
                            a[k] = a[great];
                        }
                        a[great] = ak;
                        --great;
                    }
                }
            }

            // Sort center part recursively
            mysort_short_inner(a, less, great, false);

        } else { // Partitioning with one pivot
            /*
             * Use the third of the five sorted elements as pivot.
             * This value is inexpensive approximation of the median.
             */
            short pivot = a[e3];

            /*
             * Partitioning degenerates to the traditional 3-way
             * (or "Dutch National Flag") schema:
             *
             *   left part    center part              right part
             * +-------------------------------------------------+
             * |  < pivot  |   == pivot   |     ?    |  > pivot  |
             * +-------------------------------------------------+
             *              ^              ^        ^
             *              |              |        |
             *             less            k      great
             *
             * Invariants:
             *
             *   all in (left, less)   < pivot
             *   all in [less, k)     == pivot
             *   all in (great, right) > pivot
             *
             * Pointer k is the first index of ?-part.
             */
            for (k = less; k <= great; ++k) {
                if (a[k] == pivot) {
                    continue;
                }
                short ak = a[k];
                if (ak < pivot) { // Move a[k] to left part
                    a[k] = a[less];
                    a[less] = ak;
                    ++less;
                } else { // a[k] > pivot - Move a[k] to right part
                    while (a[great] > pivot) {
                        --great;
                    }
                    if (a[great] < pivot) { // a[great] <= pivot
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // a[great] == pivot
                        /*
                         * Even though a[great] equals to pivot, the
                         * assignment a[k] = pivot may be incorrect,
                         * if a[great] and pivot are floating-point
                         * zeros of different signs. Therefore in float
                         * and double sorting methods we have to use
                         * more accurate assignment a[k] = a[great].
                         */
                        a[k] = pivot;
                    }
                    a[great] = ak;
                    --great;
                }
            }

            /*
             * Sort left and right parts recursively.
             * All elements from center part are equal
             * and, therefore, already sorted.
             */
            mysort_short_inner(a, left, less - 1, leftmost);
            mysort_short_inner(a, great + 1, right, false);
        }

}

void mysort_char(char *a, int left, int length){
        int right=left+length-1, i, j, k, m, n;

        // Use Quicksort on small arrays
        if (right - left < QUICKSORT_THRESHOLD) {
            mysort_char_inner(a, left, right, true);
            return;
        }

        /*
         * Index run[i] is the start of i-th run
         * (ascending or descending sequence).
         */
        //int[] run = new int[MAX_RUN_COUNT + 1];
        int *run=my_new2((MAX_RUN_COUNT + 1)*sizeof(int));
        int count = 0; run[0] = left;

        // Check if the array is nearly sorted
        for (k = left; k < right; run[count] = k) {
            if (a[k] < a[k + 1]) { // ascending
                while (++k <= right && a[k - 1] <= a[k]);
            } else if (a[k] > a[k + 1]) { // descending
                while (++k <= right && a[k - 1] >= a[k]);
                int lo, hi;
                for (lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                    char t = a[lo]; a[lo] = a[hi]; a[hi] = t;
                }
            } else { // equal
                for (m = MAX_RUN_LENGTH; ++k <= right && a[k - 1] == a[k]; ) {
                    if (--m == 0) {
                        mysort_char_inner(a, left, right, true);
                        free(run);
                        return;
                    }
                }
            }

            /*
             * The array is not highly structured,
             * use Quicksort instead of merge sort.
             */
            if (++count == MAX_RUN_COUNT) {
                mysort_char_inner(a, left, right, true);
                free(run);
                return;
            }
        }

        // Check special cases
        // Implementation note: variable "right" is increased by 1.
        if (run[count] == right++) { // The last run contains one element
            run[++count] = right;
        } else if (count == 1) { // The array is already sorted
            free(run);
            return;
        }

        // Determine alternation base for merge
        char odd = 0;
        for (n = 1; (n <<= 1) < count; odd ^= 1);

        // Use or create temporary array b for merging
        char *b;                 // temp array; alternates with a
        int ao, bo;              // array offsets from 'left'
        int blen = right - left; // space needed for b

        char *work=my_new2(blen*sizeof(char));
        int workBase=0;

        if (odd == 0) {
            //System.arraycopy(a, left, work, workBase, blen);
            for(i=0;i<blen;i++){
                work[i+workBase]=a[i+left];
            }
            b = a;
            bo = 0;
            a = work;
            ao = workBase - left;
        } else {
            b = work;
            ao = 0;
            bo = workBase - left;
        }

        // Merging
        int last;
        for (; count > 1; count = last) {
            for (k = (last = 0) + 2; k <= count; k += 2) {
                int hi = run[k], mi = run[k - 1];
                int p, q;
                for (i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                    if (q >= hi || p < mi && a[p + ao] <= a[q + ao]) {
                        b[i + bo] = a[p++ + ao];
                    } else {
                        b[i + bo] = a[q++ + ao];
                    }
                }
                run[++last] = hi;
            }
            if ((count & 1) != 0) {
                int lo;
                for (i = right, lo = run[count - 1]; --i >= lo;
                    b[i + bo] = a[i + ao]
                );
                run[++last] = right;
            }
            char *t = a; a = b; b = t;
            int o = ao; ao = bo; bo = o;
        }

        free(work);
        free(run);
}

void mysort_char_inner(char *a, int left, int right, int leftmost){
        int length = right - left + 1, i, j, k, m, n;

        // Use insertion sort on tiny arrays
        if (length < INSERTION_SORT_THRESHOLD) {
            if (leftmost) {
                /*
                 * Traditional (without sentinel) insertion sort,
                 * optimized for server VM, is used in case of
                 * the leftmost part.
                 */
                for (i = left, j = i; i < right; j = ++i) {
                    char ai = a[i + 1];
                    while (ai < a[j]) {
                        a[j + 1] = a[j];
                        if (j-- == left) {
                            break;
                        }
                    }
                    a[j + 1] = ai;
                }
            } else {
                /*
                 * Skip the longest ascending sequence.
                 */
                do {
                    if (left >= right) {
                        return;
                    }
                } while (a[++left] >= a[left - 1]);

                /*
                 * Every element from adjoining part plays the role
                 * of sentinel, therefore this allows us to avoid the
                 * left range check on each iteration. Moreover, we use
                 * the more optimized algorithm, so called pair insertion
                 * sort, which is faster (in the context of Quicksort)
                 * than traditional implementation of insertion sort.
                 */
                for (k = left; ++left <= right; k = ++left) {
                    char a1 = a[k], a2 = a[left];

                    if (a1 < a2) {
                        a2 = a1; a1 = a[left];
                    }
                    while (a1 < a[--k]) {
                        a[k + 2] = a[k];
                    }
                    a[++k + 1] = a1;

                    while (a2 < a[--k]) {
                        a[k + 1] = a[k];
                    }
                    a[k + 1] = a2;
                }
                char last = a[right];

                while (last < a[--right]) {
                    a[right + 1] = a[right];
                }
                a[right + 1] = last;
            }
            return;
        }

        // Inexpensive approximation of length / 7
        int seventh = (length >> 3) + (length >> 6) + 1;

        /*
         * Sort five evenly spaced elements around (and including) the
         * center element in the range. These elements will be used for
         * pivot selection as described below. The choice for spacing
         * these elements was empirically determined to work well on
         * a wide variety of inputs.
         */
        int e3 = (left + right) >> 1; // The midpoint
        int e2 = e3 - seventh;
        int e1 = e2 - seventh;
        int e4 = e3 + seventh;
        int e5 = e4 + seventh;

        // Sort these elements using insertion sort
        if (a[e2] < a[e1]) { char t = a[e2]; a[e2] = a[e1]; a[e1] = t; }

        if (a[e3] < a[e2]) { char t = a[e3]; a[e3] = a[e2]; a[e2] = t;
            if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
        }
        if (a[e4] < a[e3]) { char t = a[e4]; a[e4] = a[e3]; a[e3] = t;
            if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
            }
        }
        if (a[e5] < a[e4]) { char t = a[e5]; a[e5] = a[e4]; a[e4] = t;
            if (t < a[e3]) { a[e4] = a[e3]; a[e3] = t;
                if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                    if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
                }
            }
        }

        // Pointers
        int less  = left;  // The index of the first element of center part
        int great = right; // The index before the first element of right part

        if (a[e1] != a[e2] && a[e2] != a[e3] && a[e3] != a[e4] && a[e4] != a[e5]) {
            /*
             * Use the second and fourth of the five sorted elements as pivots.
             * These values are inexpensive approximations of the first and
             * second terciles of the array. Note that pivot1 <= pivot2.
             */
            char pivot1 = a[e2];
            char pivot2 = a[e4];

            /*
             * The first and the last elements to be sorted are moved to the
             * locations formerly occupied by the pivots. When partitioning
             * is complete, the pivots are swapped back into their final
             * positions, and excluded from subsequent sorting.
             */
            a[e2] = a[left];
            a[e4] = a[right];

            /*
             * Skip elements, which are less or greater than pivot values.
             */
            while (a[++less] < pivot1);
            while (a[--great] > pivot2);

            /*
             * Partitioning:
             *
             *   left part           center part                   right part
             * +--------------------------------------------------------------+
             * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
             * +--------------------------------------------------------------+
             *               ^                          ^       ^
             *               |                          |       |
             *              less                        k     great
             *
             * Invariants:
             *
             *              all in (left, less)   < pivot1
             *    pivot1 <= all in [less, k)     <= pivot2
             *              all in (great, right) > pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            outer1:
            for (k = less - 1; ++k <= great; ) {
                char ak = a[k];
                if (ak < pivot1) { // Move a[k] to left part
                    a[k] = a[less];
                    /*
                     * Here and below we use "a[i] = b; i++;" instead
                     * of "a[i++] = b;" due to performance issue.
                     */
                    a[less] = ak;
                    ++less;
                } else if (ak > pivot2) { // Move a[k] to right part
                    while (a[great] > pivot2) {
                        if (great-- == k) {
                            goto outer1;
                        }
                    }
                    if (a[great] < pivot1) { // a[great] <= pivot2
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // pivot1 <= a[great] <= pivot2
                        a[k] = a[great];
                    }
                    /*
                     * Here and below we use "a[i] = b; i--;" instead
                     * of "a[i--] = b;" due to performance issue.
                     */
                    a[great] = ak;
                    --great;
                }
            }

            // Swap pivots into their final positions
            a[left]  = a[less  - 1]; a[less  - 1] = pivot1;
            a[right] = a[great + 1]; a[great + 1] = pivot2;

            // Sort left and right parts recursively, excluding known pivots
            mysort_char_inner(a, left, less - 2, leftmost);
            mysort_char_inner(a, great + 2, right, false);

            /*
             * If center part is too large (comprises > 4/7 of the array),
             * swap internal pivot values to ends.
             */
            if (less < e1 && e5 < great) {
                /*
                 * Skip elements, which are equal to pivot values.
                 */
                while (a[less] == pivot1) {
                    ++less;
                }

                while (a[great] == pivot2) {
                    --great;
                }

                /*
                 * Partitioning:
                 *
                 *   left part         center part                  right part
                 * +----------------------------------------------------------+
                 * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                 * +----------------------------------------------------------+
                 *              ^                        ^       ^
                 *              |                        |       |
                 *             less                      k     great
                 *
                 * Invariants:
                 *
                 *              all in (*,  less) == pivot1
                 *     pivot1 < all in [less,  k)  < pivot2
                 *              all in (great, *) == pivot2
                 *
                 * Pointer k is the first index of ?-part.
                 */
                outer2:
                for (k = less - 1; ++k <= great; ) {
                    char ak = a[k];
                    if (ak == pivot1) { // Move a[k] to left part
                        a[k] = a[less];
                        a[less] = ak;
                        ++less;
                    } else if (ak == pivot2) { // Move a[k] to right part
                        while (a[great] == pivot2) {
                            if (great-- == k) {
                                goto outer2;
                            }
                        }
                        if (a[great] == pivot1) { // a[great] < pivot2
                            a[k] = a[less];
                            /*
                             * Even though a[great] equals to pivot1, the
                             * assignment a[less] = pivot1 may be incorrect,
                             * if a[great] and pivot1 are floating-point zeros
                             * of different signs. Therefore in float and
                             * double sorting methods we have to use more
                             * accurate assignment a[less] = a[great].
                             */
                            a[less] = pivot1;
                            ++less;
                        } else { // pivot1 < a[great] < pivot2
                            a[k] = a[great];
                        }
                        a[great] = ak;
                        --great;
                    }
                }
            }

            // Sort center part recursively
            mysort_char_inner(a, less, great, false);

        } else { // Partitioning with one pivot
            /*
             * Use the third of the five sorted elements as pivot.
             * This value is inexpensive approximation of the median.
             */
            char pivot = a[e3];

            /*
             * Partitioning degenerates to the traditional 3-way
             * (or "Dutch National Flag") schema:
             *
             *   left part    center part              right part
             * +-------------------------------------------------+
             * |  < pivot  |   == pivot   |     ?    |  > pivot  |
             * +-------------------------------------------------+
             *              ^              ^        ^
             *              |              |        |
             *             less            k      great
             *
             * Invariants:
             *
             *   all in (left, less)   < pivot
             *   all in [less, k)     == pivot
             *   all in (great, right) > pivot
             *
             * Pointer k is the first index of ?-part.
             */
            for (k = less; k <= great; ++k) {
                if (a[k] == pivot) {
                    continue;
                }
                char ak = a[k];
                if (ak < pivot) { // Move a[k] to left part
                    a[k] = a[less];
                    a[less] = ak;
                    ++less;
                } else { // a[k] > pivot - Move a[k] to right part
                    while (a[great] > pivot) {
                        --great;
                    }
                    if (a[great] < pivot) { // a[great] <= pivot
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // a[great] == pivot
                        /*
                         * Even though a[great] equals to pivot, the
                         * assignment a[k] = pivot may be incorrect,
                         * if a[great] and pivot are floating-point
                         * zeros of different signs. Therefore in float
                         * and double sorting methods we have to use
                         * more accurate assignment a[k] = a[great].
                         */
                        a[k] = pivot;
                    }
                    a[great] = ak;
                    --great;
                }
            }

            /*
             * Sort left and right parts recursively.
             * All elements from center part are equal
             * and, therefore, already sorted.
             */
            mysort_char_inner(a, left, less - 1, leftmost);
            mysort_char_inner(a, great + 1, right, false);
        }
}

void mysort_float(float *a, int left, int length){
        int right=left+length-1, i, j, k, m, n;

        // Use Quicksort on small arrays
        if (right - left < QUICKSORT_THRESHOLD) {
            mysort_float_inner(a, left, right, true);
            return;
        }

        /*
         * Index run[i] is the start of i-th run
         * (ascending or descending sequence).
         */
        //int[] run = new int[MAX_RUN_COUNT + 1];
        int *run=my_new2((MAX_RUN_COUNT + 1)*sizeof(int));
        int count = 0; run[0] = left;

        // Check if the array is nearly sorted
        for (k = left; k < right; run[count] = k) {
            if (a[k] < a[k + 1]) { // ascending
                while (++k <= right && a[k - 1] <= a[k]);
            } else if (a[k] > a[k + 1]) { // descending
                while (++k <= right && a[k - 1] >= a[k]);
                int lo, hi;
                for (lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                    float t = a[lo]; a[lo] = a[hi]; a[hi] = t;
                }
            } else { // equal
                for (m = MAX_RUN_LENGTH; ++k <= right && a[k - 1] == a[k]; ) {
                    if (--m == 0) {
                        mysort_float_inner(a, left, right, true);
                        free(run);
                        return;
                    }
                }
            }

            /*
             * The array is not highly structured,
             * use Quicksort instead of merge sort.
             */
            if (++count == MAX_RUN_COUNT) {
                mysort_float_inner(a, left, right, true);
                free(run);
                return;
            }
        }

        // Check special cases
        // Implementation note: variable "right" is increased by 1.
        if (run[count] == right++) { // The last run contains one element
            run[++count] = right;
        } else if (count == 1) { // The array is already sorted
            free(run);
            return;
        }

        // Determine alternation base for merge
        char odd = 0;
        for (n = 1; (n <<= 1) < count; odd ^= 1);

        // Use or create temporary array b for merging
        float *b;                 // temp array; alternates with a
        int ao, bo;              // array offsets from 'left'
        int blen = right - left; // space needed for b

        float *work=my_new2(blen*sizeof(float));
        int workBase=0;

        if (odd == 0) {
            //System.arraycopy(a, left, work, workBase, blen);
            for(i=0;i<blen;i++){
                work[i+workBase]=a[i+left];
            }
            b = a;
            bo = 0;
            a = work;
            ao = workBase - left;
        } else {
            b = work;
            ao = 0;
            bo = workBase - left;
        }

        // Merging
        int last;
        for (; count > 1; count = last) {
            for (k = (last = 0) + 2; k <= count; k += 2) {
                int hi = run[k], mi = run[k - 1];
                int p, q;
                for (i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                    if (q >= hi || p < mi && a[p + ao] <= a[q + ao]) {
                        b[i + bo] = a[p++ + ao];
                    } else {
                        b[i + bo] = a[q++ + ao];
                    }
                }
                run[++last] = hi;
            }
            if ((count & 1) != 0) {
                int lo;
                for (i = right, lo = run[count - 1]; --i >= lo;
                    b[i + bo] = a[i + ao]
                );
                run[++last] = right;
            }
            float *t = a; a = b; b = t;
            int o = ao; ao = bo; bo = o;
        }

        free(work);
        free(run);
}

void mysort_float_inner(float *a, int left, int right, int leftmost){
        int length = right - left + 1, i, j, k, m, n;

        // Use insertion sort on tiny arrays
        if (length < INSERTION_SORT_THRESHOLD) {
            if (leftmost) {
                /*
                 * Traditional (without sentinel) insertion sort,
                 * optimized for server VM, is used in case of
                 * the leftmost part.
                 */
                for (i = left, j = i; i < right; j = ++i) {
                    float ai = a[i + 1];
                    while (ai < a[j]) {
                        a[j + 1] = a[j];
                        if (j-- == left) {
                            break;
                        }
                    }
                    a[j + 1] = ai;
                }
            } else {
                /*
                 * Skip the longest ascending sequence.
                 */
                do {
                    if (left >= right) {
                        return;
                    }
                } while (a[++left] >= a[left - 1]);

                /*
                 * Every element from adjoining part plays the role
                 * of sentinel, therefore this allows us to avoid the
                 * left range check on each iteration. Moreover, we use
                 * the more optimized algorithm, so called pair insertion
                 * sort, which is faster (in the context of Quicksort)
                 * than traditional implementation of insertion sort.
                 */
                for (k = left; ++left <= right; k = ++left) {
                    float a1 = a[k], a2 = a[left];

                    if (a1 < a2) {
                        a2 = a1; a1 = a[left];
                    }
                    while (a1 < a[--k]) {
                        a[k + 2] = a[k];
                    }
                    a[++k + 1] = a1;

                    while (a2 < a[--k]) {
                        a[k + 1] = a[k];
                    }
                    a[k + 1] = a2;
                }
                float last = a[right];

                while (last < a[--right]) {
                    a[right + 1] = a[right];
                }
                a[right + 1] = last;
            }
            return;
        }

        // Inexpensive approximation of length / 7
        int seventh = (length >> 3) + (length >> 6) + 1;

        /*
         * Sort five evenly spaced elements around (and including) the
         * center element in the range. These elements will be used for
         * pivot selection as described below. The choice for spacing
         * these elements was empirically determined to work well on
         * a wide variety of inputs.
         */
        int e3 = (left + right) >> 1; // The midpoint
        int e2 = e3 - seventh;
        int e1 = e2 - seventh;
        int e4 = e3 + seventh;
        int e5 = e4 + seventh;

        // Sort these elements using insertion sort
        if (a[e2] < a[e1]) { float t = a[e2]; a[e2] = a[e1]; a[e1] = t; }

        if (a[e3] < a[e2]) { float t = a[e3]; a[e3] = a[e2]; a[e2] = t;
            if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
        }
        if (a[e4] < a[e3]) { float t = a[e4]; a[e4] = a[e3]; a[e3] = t;
            if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
            }
        }
        if (a[e5] < a[e4]) { float t = a[e5]; a[e5] = a[e4]; a[e4] = t;
            if (t < a[e3]) { a[e4] = a[e3]; a[e3] = t;
                if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                    if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
                }
            }
        }

        // Pointers
        int less  = left;  // The index of the first element of center part
        int great = right; // The index before the first element of right part

        if (a[e1] != a[e2] && a[e2] != a[e3] && a[e3] != a[e4] && a[e4] != a[e5]) {
            /*
             * Use the second and fourth of the five sorted elements as pivots.
             * These values are inexpensive approximations of the first and
             * second terciles of the array. Note that pivot1 <= pivot2.
             */
            float pivot1 = a[e2];
            float pivot2 = a[e4];

            /*
             * The first and the last elements to be sorted are moved to the
             * locations formerly occupied by the pivots. When partitioning
             * is complete, the pivots are swapped back into their final
             * positions, and excluded from subsequent sorting.
             */
            a[e2] = a[left];
            a[e4] = a[right];

            /*
             * Skip elements, which are less or greater than pivot values.
             */
            while (a[++less] < pivot1);
            while (a[--great] > pivot2);

            /*
             * Partitioning:
             *
             *   left part           center part                   right part
             * +--------------------------------------------------------------+
             * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
             * +--------------------------------------------------------------+
             *               ^                          ^       ^
             *               |                          |       |
             *              less                        k     great
             *
             * Invariants:
             *
             *              all in (left, less)   < pivot1
             *    pivot1 <= all in [less, k)     <= pivot2
             *              all in (great, right) > pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            outer1:
            for (k = less - 1; ++k <= great; ) {
                float ak = a[k];
                if (ak < pivot1) { // Move a[k] to left part
                    a[k] = a[less];
                    /*
                     * Here and below we use "a[i] = b; i++;" instead
                     * of "a[i++] = b;" due to performance issue.
                     */
                    a[less] = ak;
                    ++less;
                } else if (ak > pivot2) { // Move a[k] to right part
                    while (a[great] > pivot2) {
                        if (great-- == k) {
                            goto outer1;
                        }
                    }
                    if (a[great] < pivot1) { // a[great] <= pivot2
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // pivot1 <= a[great] <= pivot2
                        a[k] = a[great];
                    }
                    /*
                     * Here and below we use "a[i] = b; i--;" instead
                     * of "a[i--] = b;" due to performance issue.
                     */
                    a[great] = ak;
                    --great;
                }
            }

            // Swap pivots into their final positions
            a[left]  = a[less  - 1]; a[less  - 1] = pivot1;
            a[right] = a[great + 1]; a[great + 1] = pivot2;

            // Sort left and right parts recursively, excluding known pivots
            mysort_float_inner(a, left, less - 2, leftmost);
            mysort_float_inner(a, great + 2, right, false);

            /*
             * If center part is too large (comprises > 4/7 of the array),
             * swap internal pivot values to ends.
             */
            if (less < e1 && e5 < great) {
                /*
                 * Skip elements, which are equal to pivot values.
                 */
                while (a[less] == pivot1) {
                    ++less;
                }

                while (a[great] == pivot2) {
                    --great;
                }

                /*
                 * Partitioning:
                 *
                 *   left part         center part                  right part
                 * +----------------------------------------------------------+
                 * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                 * +----------------------------------------------------------+
                 *              ^                        ^       ^
                 *              |                        |       |
                 *             less                      k     great
                 *
                 * Invariants:
                 *
                 *              all in (*,  less) == pivot1
                 *     pivot1 < all in [less,  k)  < pivot2
                 *              all in (great, *) == pivot2
                 *
                 * Pointer k is the first index of ?-part.
                 */
                outer2:
                for (k = less - 1; ++k <= great; ) {
                    float ak = a[k];
                    if (ak == pivot1) { // Move a[k] to left part
                        a[k] = a[less];
                        a[less] = ak;
                        ++less;
                    } else if (ak == pivot2) { // Move a[k] to right part
                        while (a[great] == pivot2) {
                            if (great-- == k) {
                                goto outer2;
                            }
                        }
                        if (a[great] == pivot1) { // a[great] < pivot2
                            a[k] = a[less];
                            /*
                             * Even though a[great] equals to pivot1, the
                             * assignment a[less] = pivot1 may be incorrect,
                             * if a[great] and pivot1 are floating-point zeros
                             * of different signs. Therefore in float and
                             * double sorting methods we have to use more
                             * accurate assignment a[less] = a[great].
                             */
                            a[less] = pivot1;
                            ++less;
                        } else { // pivot1 < a[great] < pivot2
                            a[k] = a[great];
                        }
                        a[great] = ak;
                        --great;
                    }
                }
            }

            // Sort center part recursively
            mysort_float_inner(a, less, great, false);

        } else { // Partitioning with one pivot
            /*
             * Use the third of the five sorted elements as pivot.
             * This value is inexpensive approximation of the median.
             */
            float pivot = a[e3];

            /*
             * Partitioning degenerates to the traditional 3-way
             * (or "Dutch National Flag") schema:
             *
             *   left part    center part              right part
             * +-------------------------------------------------+
             * |  < pivot  |   == pivot   |     ?    |  > pivot  |
             * +-------------------------------------------------+
             *              ^              ^        ^
             *              |              |        |
             *             less            k      great
             *
             * Invariants:
             *
             *   all in (left, less)   < pivot
             *   all in [less, k)     == pivot
             *   all in (great, right) > pivot
             *
             * Pointer k is the first index of ?-part.
             */
            for (k = less; k <= great; ++k) {
                if (a[k] == pivot) {
                    continue;
                }
                float ak = a[k];
                if (ak < pivot) { // Move a[k] to left part
                    a[k] = a[less];
                    a[less] = ak;
                    ++less;
                } else { // a[k] > pivot - Move a[k] to right part
                    while (a[great] > pivot) {
                        --great;
                    }
                    if (a[great] < pivot) { // a[great] <= pivot
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // a[great] == pivot
                        /*
                         * Even though a[great] equals to pivot, the
                         * assignment a[k] = pivot may be incorrect,
                         * if a[great] and pivot are floating-point
                         * zeros of different signs. Therefore in float
                         * and double sorting methods we have to use
                         * more accurate assignment a[k] = a[great].
                         */
                        a[k] = pivot;
                    }
                    a[great] = ak;
                    --great;
                }
            }

            /*
             * Sort left and right parts recursively.
             * All elements from center part are equal
             * and, therefore, already sorted.
             */
            mysort_float_inner(a, left, less - 1, leftmost);
            mysort_float_inner(a, great + 1, right, false);
        }
}

void mysort_double(double *a, int left, int length){
        int right=left+length-1, i, j, k, m, n;

        // Use Quicksort on small arrays
        if (right - left < QUICKSORT_THRESHOLD) {
            mysort_double_inner(a, left, right, true);
            return;
        }

        /*
         * Index run[i] is the start of i-th run
         * (ascending or descending sequence).
         */
        //int[] run = new int[MAX_RUN_COUNT + 1];
        int *run=my_new2((MAX_RUN_COUNT + 1)*sizeof(int));
        int count = 0; run[0] = left;

        // Check if the array is nearly sorted
        for (k = left; k < right; run[count] = k) {
            if (a[k] < a[k + 1]) { // ascending
                while (++k <= right && a[k - 1] <= a[k]);
            } else if (a[k] > a[k + 1]) { // descending
                while (++k <= right && a[k - 1] >= a[k]);
                int lo, hi;
                for (lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                    double t = a[lo]; a[lo] = a[hi]; a[hi] = t;
                }
            } else { // equal
                for (m = MAX_RUN_LENGTH; ++k <= right && a[k - 1] == a[k]; ) {
                    if (--m == 0) {
                        mysort_double_inner(a, left, right, true);
                        free(run);
                        return;
                    }
                }
            }

            /*
             * The array is not highly structured,
             * use Quicksort instead of merge sort.
             */
            if (++count == MAX_RUN_COUNT) {
                mysort_double_inner(a, left, right, true);
                free(run);
                return;
            }
        }

        // Check special cases
        // Implementation note: variable "right" is increased by 1.
        if (run[count] == right++) { // The last run contains one element
            run[++count] = right;
        } else if (count == 1) { // The array is already sorted
            free(run);
            return;
        }

        // Determine alternation base for merge
        char odd = 0;
        for (n = 1; (n <<= 1) < count; odd ^= 1);

        // Use or create temporary array b for merging
        double *b;                 // temp array; alternates with a
        int ao, bo;              // array offsets from 'left'
        int blen = right - left; // space needed for b

        double *work=my_new2(blen*sizeof(double));
        int workBase=0;

        if (odd == 0) {
            //System.arraycopy(a, left, work, workBase, blen);
            for(i=0;i<blen;i++){
                work[i+workBase]=a[i+left];
            }
            b = a;
            bo = 0;
            a = work;
            ao = workBase - left;
        } else {
            b = work;
            ao = 0;
            bo = workBase - left;
        }

        // Merging
        int last;
        for (; count > 1; count = last) {
            for (k = (last = 0) + 2; k <= count; k += 2) {
                int hi = run[k], mi = run[k - 1];
                int p, q;
                for (i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                    if (q >= hi || p < mi && a[p + ao] <= a[q + ao]) {
                        b[i + bo] = a[p++ + ao];
                    } else {
                        b[i + bo] = a[q++ + ao];
                    }
                }
                run[++last] = hi;
            }
            if ((count & 1) != 0) {
                int lo;
                for (i = right, lo = run[count - 1]; --i >= lo;
                    b[i + bo] = a[i + ao]
                );
                run[++last] = right;
            }
            double *t = a; a = b; b = t;
            int o = ao; ao = bo; bo = o;
        }

        free(work);
        free(run);
}

void mysort_double_inner(double *a, int left, int right, int leftmost){
        int length = right - left + 1, i, j, k, m, n;

        // Use insertion sort on tiny arrays
        if (length < INSERTION_SORT_THRESHOLD) {
            if (leftmost) {
                /*
                 * Traditional (without sentinel) insertion sort,
                 * optimized for server VM, is used in case of
                 * the leftmost part.
                 */
                for (i = left, j = i; i < right; j = ++i) {
                    double ai = a[i + 1];
                    while (ai < a[j]) {
                        a[j + 1] = a[j];
                        if (j-- == left) {
                            break;
                        }
                    }
                    a[j + 1] = ai;
                }
            } else {
                /*
                 * Skip the longest ascending sequence.
                 */
                do {
                    if (left >= right) {
                        return;
                    }
                } while (a[++left] >= a[left - 1]);

                /*
                 * Every element from adjoining part plays the role
                 * of sentinel, therefore this allows us to avoid the
                 * left range check on each iteration. Moreover, we use
                 * the more optimized algorithm, so called pair insertion
                 * sort, which is faster (in the context of Quicksort)
                 * than traditional implementation of insertion sort.
                 */
                for (k = left; ++left <= right; k = ++left) {
                    double a1 = a[k], a2 = a[left];

                    if (a1 < a2) {
                        a2 = a1; a1 = a[left];
                    }
                    while (a1 < a[--k]) {
                        a[k + 2] = a[k];
                    }
                    a[++k + 1] = a1;

                    while (a2 < a[--k]) {
                        a[k + 1] = a[k];
                    }
                    a[k + 1] = a2;
                }
                double last = a[right];

                while (last < a[--right]) {
                    a[right + 1] = a[right];
                }
                a[right + 1] = last;
            }
            return;
        }

        // Inexpensive approximation of length / 7
        int seventh = (length >> 3) + (length >> 6) + 1;

        /*
         * Sort five evenly spaced elements around (and including) the
         * center element in the range. These elements will be used for
         * pivot selection as described below. The choice for spacing
         * these elements was empirically determined to work well on
         * a wide variety of inputs.
         */
        int e3 = (left + right) >> 1; // The midpoint
        int e2 = e3 - seventh;
        int e1 = e2 - seventh;
        int e4 = e3 + seventh;
        int e5 = e4 + seventh;

        // Sort these elements using insertion sort
        if (a[e2] < a[e1]) { double t = a[e2]; a[e2] = a[e1]; a[e1] = t; }

        if (a[e3] < a[e2]) { double t = a[e3]; a[e3] = a[e2]; a[e2] = t;
            if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
        }
        if (a[e4] < a[e3]) { double t = a[e4]; a[e4] = a[e3]; a[e3] = t;
            if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
            }
        }
        if (a[e5] < a[e4]) { double t = a[e5]; a[e5] = a[e4]; a[e4] = t;
            if (t < a[e3]) { a[e4] = a[e3]; a[e3] = t;
                if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                    if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
                }
            }
        }

        // Pointers
        int less  = left;  // The index of the first element of center part
        int great = right; // The index before the first element of right part

        if (a[e1] != a[e2] && a[e2] != a[e3] && a[e3] != a[e4] && a[e4] != a[e5]) {
            /*
             * Use the second and fourth of the five sorted elements as pivots.
             * These values are inexpensive approximations of the first and
             * second terciles of the array. Note that pivot1 <= pivot2.
             */
            double pivot1 = a[e2];
            double pivot2 = a[e4];

            /*
             * The first and the last elements to be sorted are moved to the
             * locations formerly occupied by the pivots. When partitioning
             * is complete, the pivots are swapped back into their final
             * positions, and excluded from subsequent sorting.
             */
            a[e2] = a[left];
            a[e4] = a[right];

            /*
             * Skip elements, which are less or greater than pivot values.
             */
            while (a[++less] < pivot1);
            while (a[--great] > pivot2);

            /*
             * Partitioning:
             *
             *   left part           center part                   right part
             * +--------------------------------------------------------------+
             * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
             * +--------------------------------------------------------------+
             *               ^                          ^       ^
             *               |                          |       |
             *              less                        k     great
             *
             * Invariants:
             *
             *              all in (left, less)   < pivot1
             *    pivot1 <= all in [less, k)     <= pivot2
             *              all in (great, right) > pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            outer1:
            for (k = less - 1; ++k <= great; ) {
                double ak = a[k];
                if (ak < pivot1) { // Move a[k] to left part
                    a[k] = a[less];
                    /*
                     * Here and below we use "a[i] = b; i++;" instead
                     * of "a[i++] = b;" due to performance issue.
                     */
                    a[less] = ak;
                    ++less;
                } else if (ak > pivot2) { // Move a[k] to right part
                    while (a[great] > pivot2) {
                        if (great-- == k) {
                            goto outer1;
                        }
                    }
                    if (a[great] < pivot1) { // a[great] <= pivot2
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // pivot1 <= a[great] <= pivot2
                        a[k] = a[great];
                    }
                    /*
                     * Here and below we use "a[i] = b; i--;" instead
                     * of "a[i--] = b;" due to performance issue.
                     */
                    a[great] = ak;
                    --great;
                }
            }

            // Swap pivots into their final positions
            a[left]  = a[less  - 1]; a[less  - 1] = pivot1;
            a[right] = a[great + 1]; a[great + 1] = pivot2;

            // Sort left and right parts recursively, excluding known pivots
            mysort_double_inner(a, left, less - 2, leftmost);
            mysort_double_inner(a, great + 2, right, false);

            /*
             * If center part is too large (comprises > 4/7 of the array),
             * swap internal pivot values to ends.
             */
            if (less < e1 && e5 < great) {
                /*
                 * Skip elements, which are equal to pivot values.
                 */
                while (a[less] == pivot1) {
                    ++less;
                }

                while (a[great] == pivot2) {
                    --great;
                }

                /*
                 * Partitioning:
                 *
                 *   left part         center part                  right part
                 * +----------------------------------------------------------+
                 * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                 * +----------------------------------------------------------+
                 *              ^                        ^       ^
                 *              |                        |       |
                 *             less                      k     great
                 *
                 * Invariants:
                 *
                 *              all in (*,  less) == pivot1
                 *     pivot1 < all in [less,  k)  < pivot2
                 *              all in (great, *) == pivot2
                 *
                 * Pointer k is the first index of ?-part.
                 */
                outer2:
                for (k = less - 1; ++k <= great; ) {
                    double ak = a[k];
                    if (ak == pivot1) { // Move a[k] to left part
                        a[k] = a[less];
                        a[less] = ak;
                        ++less;
                    } else if (ak == pivot2) { // Move a[k] to right part
                        while (a[great] == pivot2) {
                            if (great-- == k) {
                                goto outer2;
                            }
                        }
                        if (a[great] == pivot1) { // a[great] < pivot2
                            a[k] = a[less];
                            /*
                             * Even though a[great] equals to pivot1, the
                             * assignment a[less] = pivot1 may be incorrect,
                             * if a[great] and pivot1 are floating-point zeros
                             * of different signs. Therefore in float and
                             * double sorting methods we have to use more
                             * accurate assignment a[less] = a[great].
                             */
                            a[less] = pivot1;
                            ++less;
                        } else { // pivot1 < a[great] < pivot2
                            a[k] = a[great];
                        }
                        a[great] = ak;
                        --great;
                    }
                }
            }

            // Sort center part recursively
            mysort_double_inner(a, less, great, false);

        } else { // Partitioning with one pivot
            /*
             * Use the third of the five sorted elements as pivot.
             * This value is inexpensive approximation of the median.
             */
            double pivot = a[e3];

            /*
             * Partitioning degenerates to the traditional 3-way
             * (or "Dutch National Flag") schema:
             *
             *   left part    center part              right part
             * +-------------------------------------------------+
             * |  < pivot  |   == pivot   |     ?    |  > pivot  |
             * +-------------------------------------------------+
             *              ^              ^        ^
             *              |              |        |
             *             less            k      great
             *
             * Invariants:
             *
             *   all in (left, less)   < pivot
             *   all in [less, k)     == pivot
             *   all in (great, right) > pivot
             *
             * Pointer k is the first index of ?-part.
             */
            for (k = less; k <= great; ++k) {
                if (a[k] == pivot) {
                    continue;
                }
                double ak = a[k];
                if (ak < pivot) { // Move a[k] to left part
                    a[k] = a[less];
                    a[less] = ak;
                    ++less;
                } else { // a[k] > pivot - Move a[k] to right part
                    while (a[great] > pivot) {
                        --great;
                    }
                    if (a[great] < pivot) { // a[great] <= pivot
                        a[k] = a[less];
                        a[less] = a[great];
                        ++less;
                    } else { // a[great] == pivot
                        /*
                         * Even though a[great] equals to pivot, the
                         * assignment a[k] = pivot may be incorrect,
                         * if a[great] and pivot are floating-point
                         * zeros of different signs. Therefore in float
                         * and double sorting methods we have to use
                         * more accurate assignment a[k] = a[great].
                         */
                        a[k] = pivot;
                    }
                    a[great] = ak;
                    --great;
                }
            }

            /*
             * Sort left and right parts recursively.
             * All elements from center part are equal
             * and, therefore, already sorted.
             */
            mysort_double_inner(a, left, less - 1, leftmost);
            mysort_double_inner(a, great + 1, right, false);
        }
}

void mysort_void(void **arr, int left, int length, int (*mycompare)(void *a, void *b)){
        int right=left+length-1, i, j, k, m, n;

        // Use Quicksort on small arrays
        if (right - left < QUICKSORT_THRESHOLD) {
            mysort_void_inner(arr, left, right, true, mycompare);
            return;
        }

        /*
         * Index run[i] is the start of i-th run
         * (ascending or descending sequence).
         */
        //int[] run = new int[MAX_RUN_COUNT + 1];
        int *run=my_new2((MAX_RUN_COUNT + 1)*sizeof(int));
        int count = 0; run[0] = left;

        // Check if the array is nearly sorted
        for (k = left; k < right; run[count] = k) {
            if (mycompare(arr[k], arr[k + 1])<0) { // ascending
                while (++k <= right && mycompare(arr[k - 1], arr[k])<=0);
            } else if (mycompare(arr[k], arr[k + 1])>0) { // descending
                while (++k <= right && mycompare(arr[k - 1], arr[k])>=0);
                int lo, hi;
                for (lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                    void *t = arr[lo]; arr[lo] = arr[hi]; arr[hi] = t;
                }
            } else { // equal
                for (m = MAX_RUN_LENGTH; ++k <= right && mycompare(arr[k - 1], arr[k])==0; ) {
                    if (--m == 0) {
                        mysort_void_inner(arr, left, right, true, mycompare);
                        free(run);
                        return;
                    }
                }
            }

            /*
             * The array is not highly structured,
             * use Quicksort instead of merge sort.
             */
            if (++count == MAX_RUN_COUNT) {
                mysort_void_inner(arr, left, right, true, mycompare);
                free(run);
                return;
            }
        }

        // Check special cases
        // Implementation note: variable "right" is increased by 1.
        if (run[count] == right++) { // The last run contains one element
            run[++count] = right;
        } else if (count == 1) { // The array is already sorted
            free(run);
            return;
        }

        // Determine alternation base for merge
        char odd = 0;
        for (n = 1; (n <<= 1) < count; odd ^= 1);

        // Use or create temporary array b for merging
        void **b;                 // temp array; alternates with arr
        int ao, bo;              // array offsets from 'left'
        int blen = right - left; // space needed for b

        void **work=my_new2(blen*sizeof(int64_t));
        int workBase=0;

        if (odd == 0) {
            //System.arraycopy(arr, left, work, workBase, blen);
            for(i=0;i<blen;i++){
                work[i+workBase]=arr[i+left];
            }
            b = arr;
            bo = 0;
            arr = work;
            ao = workBase - left;
        } else {
            b = work;
            ao = 0;
            bo = workBase - left;
        }

        // Merging
        int last;
        for (; count > 1; count = last) {
            for (k = (last = 0) + 2; k <= count; k += 2) {
                int hi = run[k], mi = run[k - 1];
                int p, q;
                for (i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                    if (q >= hi || p < mi && mycompare(arr[p + ao], arr[q + ao])<=0) {
                        b[i + bo] = arr[p++ + ao];
                    } else {
                        b[i + bo] = arr[q++ + ao];
                    }
                }
                run[++last] = hi;
            }
            if ((count & 1) != 0) {
                int lo;
                for (i = right, lo = run[count - 1]; --i >= lo;
                    b[i + bo] = arr[i + ao]
                );
                run[++last] = right;
            }
            void **t = arr; arr = b; b = t;
            int o = ao; ao = bo; bo = o;
        }

        free(work);
        free(run);
}

void mysort_void_inner(void **arr, int left, int right, int leftmost, int (*mycompare)(void *a, void *b)){
        int length = right - left + 1, i, j, k, m, n;

        // Use insertion sort on tiny arrays
        if (length < INSERTION_SORT_THRESHOLD) {
            if (leftmost) {
                /*
                 * Traditional (without sentinel) insertion sort,
                 * optimized for server VM, is used in case of
                 * the leftmost part.
                 */
                for (i = left, j = i; i < right; j = ++i) {
                    void *ai = arr[i + 1];
                    while (mycompare(ai, arr[j])<0) {
                        arr[j + 1] = arr[j];
                        if (j-- == left) {
                            break;
                        }
                    }
                    arr[j + 1] = ai;
                }
            } else {
                /*
                 * Skip the longest ascending sequence.
                 */
                do {
                    if (left >= right) {
                        return;
                    }
                } while (mycompare(arr[++left], arr[left - 1])>=0);

                /*
                 * Every element from adjoining part plays the role
                 * of sentinel, therefore this allows us to avoid the
                 * left range check on each iteration. Moreover, we use
                 * the more optimized algorithm, so called pair insertion
                 * sort, which is faster (in the context of Quicksort)
                 * than traditional implementation of insertion sort.
                 */
                for (k = left; ++left <= right; k = ++left) {
                    void *a1 = arr[k], *a2 = arr[left];

                    if (a1 < a2) {
                        a2 = a1; a1 = arr[left];
                    }
                    while (mycompare(a1, arr[--k])<0) {
                        arr[k + 2] = arr[k];
                    }
                    arr[++k + 1] = a1;

                    while (mycompare(a2, arr[--k])<0) {
                        arr[k + 1] = arr[k];
                    }
                    arr[k + 1] = a2;
                }
                void *last = arr[right];

                while (mycompare(last, arr[--right])<0) {
                    arr[right + 1] = arr[right];
                }
                arr[right + 1] = last;
            }
            return;
        }

        // Inexpensive approximation of length / 7
        int seventh = (length >> 3) + (length >> 6) + 1;

        /*
         * Sort five evenly spaced elements around (and including) the
         * center element in the range. These elements will be used for
         * pivot selection as described below. The choice for spacing
         * these elements was empirically determined to work well on
         * arr wide variety of inputs.
         */
        int e3 = (left + right) >> 1; // The midpoint
        int e2 = e3 - seventh;
        int e1 = e2 - seventh;
        int e4 = e3 + seventh;
        int e5 = e4 + seventh;

        // Sort these elements using insertion sort
        if (mycompare(arr[e2], arr[e1])<0) { void *t = arr[e2]; arr[e2] = arr[e1]; arr[e1] = t; }

        if (mycompare(arr[e3], arr[e2])<0) { void *t = arr[e3]; arr[e3] = arr[e2]; arr[e2] = t;
            if (mycompare(t, arr[e1])<0) { arr[e2] = arr[e1]; arr[e1] = t; }
        }
        if (mycompare(arr[e4], arr[e3])<0) { void *t = arr[e4]; arr[e4] = arr[e3]; arr[e3] = t;
            if (mycompare(t, arr[e2])<0) { arr[e3] = arr[e2]; arr[e2] = t;
                if (mycompare(t, arr[e1])<0) { arr[e2] = arr[e1]; arr[e1] = t; }
            }
        }
        if (mycompare(arr[e5], arr[e4])<0) { void *t = arr[e5]; arr[e5] = arr[e4]; arr[e4] = t;
            if (mycompare(t, arr[e3])<0) { arr[e4] = arr[e3]; arr[e3] = t;
                if (mycompare(t, arr[e2])<0) { arr[e3] = arr[e2]; arr[e2] = t;
                    if (mycompare(t, arr[e1])<0) { arr[e2] = arr[e1]; arr[e1] = t; }
                }
            }
        }

        // Pointers
        int less  = left;  // The index of the first element of center part
        int great = right; // The index before the first element of right part

        if (mycompare(arr[e1], arr[e2])!=0 && mycompare(arr[e2], arr[e3])!=0 && mycompare(arr[e3], arr[e4])!=0 && mycompare(arr[e4], arr[e5])!=0){
            /*
             * Use the second and fourth of the five sorted elements as pivots.
             * These values are inexpensive approximations of the first and
             * second terciles of the array. Note that pivot1 <= pivot2.
             */
            void *pivot1 = arr[e2];
            void *pivot2 = arr[e4];

            /*
             * The first and the last elements to be sorted are moved to the
             * locations formerly occupied by the pivots. When partitioning
             * is complete, the pivots are swapped back into their final
             * positions, and excluded from subsequent sorting.
             */
            arr[e2] = arr[left];
            arr[e4] = arr[right];

            /*
             * Skip elements, which are less or greater than pivot values.
             */
            while (mycompare(arr[++less], pivot1)<0);
            while (mycompare(arr[--great], pivot2)>0);

            /*
             * Partitioning:
             *
             *   left part           center part                   right part
             * +--------------------------------------------------------------+
             * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
             * +--------------------------------------------------------------+
             *               ^                          ^       ^
             *               |                          |       |
             *              less                        k     great
             *
             * Invariants:
             *
             *              all in (left, less)   < pivot1
             *    pivot1 <= all in [less, k)     <= pivot2
             *              all in (great, right) > pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            outer1:
            for (k = less - 1; ++k <= great; ) {
                void *ak = arr[k];
                if (mycompare(ak, pivot1)<0) { // Move arr[k] to left part
                    arr[k] = arr[less];
                    /*
                     * Here and below we use "arr[i] = b; i++;" instead
                     * of "arr[i++] = b;" due to performance issue.
                     */
                    arr[less] = ak;
                    ++less;
                } else if (mycompare(ak, pivot2)>0) { // Move arr[k] to right part
                    while (mycompare(arr[great], pivot2)>0) {
                        if (great-- == k) {
                            goto outer1;
                        }
                    }
                    if (mycompare(arr[great], pivot1)<0) { // arr[great] <= pivot2
                        arr[k] = arr[less];
                        arr[less] = arr[great];
                        ++less;
                    } else { // pivot1 <= arr[great] <= pivot2
                        arr[k] = arr[great];
                    }
                    /*
                     * Here and below we use "arr[i] = b; i--;" instead
                     * of "arr[i--] = b;" due to performance issue.
                     */
                    arr[great] = ak;
                    --great;
                }
            }

            // Swap pivots into their final positions
            arr[left]  = arr[less  - 1]; arr[less  - 1] = pivot1;
            arr[right] = arr[great + 1]; arr[great + 1] = pivot2;

            // Sort left and right parts recursively, excluding known pivots
            mysort_void_inner(arr, left, less - 2, leftmost, mycompare);
            mysort_void_inner(arr, great + 2, right, false, mycompare);

            /*
             * If center part is too large (comprises > 4/7 of the array),
             * swap internal pivot values to ends.
             */
            if (less < e1 && e5 < great) {
                /*
                 * Skip elements, which are equal to pivot values.
                 */
                while (mycompare(arr[less], pivot1)==0) {
                    ++less;
                }

                while (mycompare(arr[great], pivot2)==0) {
                    --great;
                }

                /*
                 * Partitioning:
                 *
                 *   left part         center part                  right part
                 * +----------------------------------------------------------+
                 * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                 * +----------------------------------------------------------+
                 *              ^                        ^       ^
                 *              |                        |       |
                 *             less                      k     great
                 *
                 * Invariants:
                 *
                 *              all in (*,  less) == pivot1
                 *     pivot1 < all in [less,  k)  < pivot2
                 *              all in (great, *) == pivot2
                 *
                 * Pointer k is the first index of ?-part.
                 */
                outer2:
                for (k = less - 1; ++k <= great; ) {
                    void *ak = arr[k];
                    if (mycompare(ak, pivot1)==0){ // Move arr[k] to left part
                        arr[k] = arr[less];
                        arr[less] = ak;
                        ++less;
                    } else if (mycompare(ak, pivot2)==0) { // Move arr[k] to right part
                        while (mycompare(arr[great], pivot2)==0){
                            if (great-- == k) {
                                goto outer2;
                            }
                        }
                        if (mycompare(arr[great], pivot1)==0) { // arr[great] < pivot2
                            arr[k] = arr[less];
                            /*
                             * Even though arr[great] equals to pivot1, the
                             * assignment arr[less] = pivot1 may be incorrect,
                             * if arr[great] and pivot1 are floating-point zeros
                             * of different signs. Therefore in float and
                             * double sorting methods we have to use more
                             * accurate assignment arr[less] = arr[great].
                             */
                            arr[less] = pivot1;
                            ++less;
                        } else { // pivot1 < arr[great] < pivot2
                            arr[k] = arr[great];
                        }
                        arr[great] = ak;
                        --great;
                    }
                }
            }

            // Sort center part recursively
            mysort_void_inner(arr, less, great, false, mycompare);

        } else { // Partitioning with one pivot
            /*
             * Use the third of the five sorted elements as pivot.
             * This value is inexpensive approximation of the median.
             */
            void *pivot = arr[e3];

            /*
             * Partitioning degenerates to the traditional 3-way
             * (or "Dutch National Flag") schema:
             *
             *   left part    center part              right part
             * +-------------------------------------------------+
             * |  < pivot  |   == pivot   |     ?    |  > pivot  |
             * +-------------------------------------------------+
             *              ^              ^        ^
             *              |              |        |
             *             less            k      great
             *
             * Invariants:
             *
             *   all in (left, less)   < pivot
             *   all in [less, k)     == pivot
             *   all in (great, right) > pivot
             *
             * Pointer k is the first index of ?-part.
             */
            for (k = less; k <= great; ++k) {
                if (mycompare(arr[k], pivot)==0) {
                    continue;
                }
                void *ak = arr[k];
                if (mycompare(ak, pivot)<0) { // Move arr[k] to left part
                    arr[k] = arr[less];
                    arr[less] = ak;
                    ++less;
                } else { // arr[k] > pivot - Move arr[k] to right part
                    while (mycompare(arr[great], pivot)>0){
                        --great;
                    }
                    if (mycompare(arr[great], pivot)<0) { // arr[great] <= pivot
                        arr[k] = arr[less];
                        arr[less] = arr[great];
                        ++less;
                    } else { // arr[great] == pivot
                        /*
                         * Even though arr[great] equals to pivot, the
                         * assignment arr[k] = pivot may be incorrect,
                         * if arr[great] and pivot are floating-point
                         * zeros of different signs. Therefore in float
                         * and double sorting methods we have to use
                         * more accurate assignment arr[k] = arr[great].
                         */
                        arr[k] = pivot;
                    }
                    arr[great] = ak;
                    --great;
                }
            }

            /*
             * Sort left and right parts recursively.
             * All elements from center part are equal
             * and, therefore, already sorted.
             */
            mysort_void_inner(arr, left, less - 1, leftmost, mycompare);
            mysort_void_inner(arr, great + 1, right, false, mycompare);
        }
}

//==================== java1.6 sort

void java_sort_int(int *arr, int left, int length){
    java_sort_int_inner(arr, left, length);
}

static void java_sort_int_inner(int *x, int off, int len){
    int i, j;

    // Insertion sort on smallest arrays
    if (len < 7) {
	    for (i=off; i<len+off; i++)
		for (j=i; j>off && x[j-1]>x[j]; j--)
		    java_sort_int_swap(x, j, j-1);
	    return;
	}

    // Choose a partition element, v
	int m = off + (len >> 1);       // Small arrays, middle element
	if (len > 7) {
	    int l = off;
	    int n = off + len - 1;
	    if (len > 40) {        // Big arrays, pseudomedian of 9
		int s = len/8;
		l = java_sort_int_med3(x, l,     l+s, l+2*s);
		m = java_sort_int_med3(x, m-s,   m,   m+s);
		n = java_sort_int_med3(x, n-2*s, n-s, n);
	    }
	    m = java_sort_int_med3(x, l, m, n); // Mid-size, med of 3
	}
	int v = x[m];

    // Establish Invariant: v* (<v)* (>v)* v*
	int a = off, b = a, c = off + len - 1, d = c;
	while(true) {
	    while (b <= c && x[b] <= v) {
		if (x[b] == v)
		    java_sort_int_swap(x, a++, b);
		b++;
	    }
	    while (c >= b && x[c] >= v) {
		if (x[c] == v)
		    java_sort_int_swap(x, c, d--);
		c--;
	    }
	    if (b > c)
		break;
	    java_sort_int_swap(x, b++, c--);
	}

    // Swap partition elements back to middle
	int s, n = off + len;

	int tmp1=a-off;
	int tmp2=b-a;
	s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_int_vecswap(x, off, b-s, s);

    tmp1=d-c;
    tmp2=n-d-1;
    s=tmp1<tmp2 ? tmp1:tmp2;
	java_sort_int_vecswap(x, b,   n-s, s);

	// Recursively sort non-partition-elements
	if ((s = b-a) > 1)
	    java_sort_int_inner(x, off, s);
	if ((s = d-c) > 1)
	    java_sort_int_inner(x, n-s, s);
}

static void java_sort_int_swap(int *x, int a, int b){
    int t = x[a];
	x[a] = x[b];
	x[b] = t;
}

static int java_sort_int_med3(int *x, int a, int b, int c){
    return (x[a] < x[b] ?
		(x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
		(x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

static void java_sort_int_vecswap(int *x, int a, int b, int n){
    int i;
    for (i=0; i<n; i++, a++, b++)
	    java_sort_int_swap(x, a, b);
}

void java_sort_long(int64_t *arr, int left, int length){
    java_sort_long_inner(arr, left, length);
}

static void java_sort_long_inner(int64_t *x, int off, int len){
    int i, j;

    // Insertion sort on smallest arrays
    if (len < 7) {
	    for (i=off; i<len+off; i++)
		for (j=i; j>off && x[j-1]>x[j]; j--)
		    java_sort_long_swap(x, j, j-1);
	    return;
	}

    // Choose a partition element, v
	int m = off + (len >> 1);       // Small arrays, middle element
	if (len > 7) {
	    int l = off;
	    int n = off + len - 1;
	    if (len > 40) {        // Big arrays, pseudomedian of 9
		int s = len/8;
		l = java_sort_long_med3(x, l,     l+s, l+2*s);
		m = java_sort_long_med3(x, m-s,   m,   m+s);
		n = java_sort_long_med3(x, n-2*s, n-s, n);
	    }
	    m = java_sort_long_med3(x, l, m, n); // Mid-size, med of 3
	}
	int64_t v = x[m];

    // Establish Invariant: v* (<v)* (>v)* v*
	int a = off, b = a, c = off + len - 1, d = c;
	while(true) {
	    while (b <= c && x[b] <= v) {
		if (x[b] == v)
		    java_sort_long_swap(x, a++, b);
		b++;
	    }
	    while (c >= b && x[c] >= v) {
		if (x[c] == v)
		    java_sort_long_swap(x, c, d--);
		c--;
	    }
	    if (b > c)
		break;
	    java_sort_long_swap(x, b++, c--);
	}

    // Swap partition elements back to middle
	int s, n = off + len;

	int tmp1=a-off;
	int tmp2=b-a;
	s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_long_vecswap(x, off, b-s, s);

    tmp1=d-c;
    tmp2=n-d-1;
    s=tmp1<tmp2 ? tmp1:tmp2;
	java_sort_long_vecswap(x, b,   n-s, s);

	// Recursively sort non-partition-elements
	if ((s = b-a) > 1)
	    java_sort_long_inner(x, off, s);
	if ((s = d-c) > 1)
	    java_sort_long_inner(x, n-s, s);
}

static void java_sort_long_swap(int64_t *x, int a, int b){
    int64_t t = x[a];
	x[a] = x[b];
	x[b] = t;
}

static int java_sort_long_med3(int64_t *x, int a, int b, int c){
    return (x[a] < x[b] ?
		(x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
		(x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

static void java_sort_long_vecswap(int64_t *x, int a, int b, int n){
    int i;
    for (i=0; i<n; i++, a++, b++)
	    java_sort_long_swap(x, a, b);
}

void java_sort_double(double *arr, int left, int length){
    java_sort_double_inner(arr, left, length);
}

static void java_sort_double_inner(double *x, int off, int len){
    int i, j;

    // Insertion sort on smallest arrays
    if (len < 7) {
	    for (i=off; i<len+off; i++)
		for (j=i; j>off && x[j-1]>x[j]; j--)
		    java_sort_double_swap(x, j, j-1);
	    return;
	}

    // Choose a partition element, v
	int m = off + (len >> 1);       // Small arrays, middle element
	if (len > 7) {
	    int l = off;
	    int n = off + len - 1;
	    if (len > 40) {        // Big arrays, pseudomedian of 9
		int s = len/8;
		l = java_sort_double_med3(x, l,     l+s, l+2*s);
		m = java_sort_double_med3(x, m-s,   m,   m+s);
		n = java_sort_double_med3(x, n-2*s, n-s, n);
	    }
	    m = java_sort_double_med3(x, l, m, n); // Mid-size, med of 3
	}
	double v = x[m];

    // Establish Invariant: v* (<v)* (>v)* v*
	int a = off, b = a, c = off + len - 1, d = c;
	while(true) {
	    while (b <= c && x[b] <= v) {
		if (x[b] == v)
		    java_sort_double_swap(x, a++, b);
		b++;
	    }
	    while (c >= b && x[c] >= v) {
		if (x[c] == v)
		    java_sort_double_swap(x, c, d--);
		c--;
	    }
	    if (b > c)
		break;
	    java_sort_double_swap(x, b++, c--);
	}

    // Swap partition elements back to middle
	int s, n = off + len;

	int tmp1=a-off;
	int tmp2=b-a;
	s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_double_vecswap(x, off, b-s, s);

    tmp1=d-c;
    tmp2=n-d-1;
    s=tmp1<tmp2 ? tmp1:tmp2;
	java_sort_double_vecswap(x, b,   n-s, s);

	// Recursively sort non-partition-elements
	if ((s = b-a) > 1)
	    java_sort_double_inner(x, off, s);
	if ((s = d-c) > 1)
	    java_sort_double_inner(x, n-s, s);
}

static void java_sort_double_swap(double *x, int a, int b){
    double t = x[a];
	x[a] = x[b];
	x[b] = t;
}

static int java_sort_double_med3(double *x, int a, int b, int c){
    return (x[a] < x[b] ?
		(x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
		(x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

static void java_sort_double_vecswap(double *x, int a, int b, int n){
    int i;
    for (i=0; i<n; i++, a++, b++)
	    java_sort_double_swap(x, a, b);
}

void java_sort_void(void **arr, int left, int length, int(*mycompare)(void *a, void *b)){
    void **aux=(void **)my_new2(length*sizeof(void *));
    int i;
    for(i=0;i<length;i++){
        aux[i]=arr[left+i];
    }

    java_sort_void_merge_sort(aux, arr, left, left+length, -left, mycompare);

    free(aux);
}

static void java_sort_void_merge_sort(void **src, void **dest, int low, int high, int off, int(*c)(void *a, void *b)){
    int length=high-low, i, j, p, q;

    if (length < 7) {
	    for (i=low; i<high; i++)
		for (j=i; j>low && c(dest[j-1], dest[j])>0; j--)
		    java_sort_void_swap(dest, j, j-1);
	    return;
	}

    // Recursively sort halves of dest into src
    int destLow  = low;
    int destHigh = high;
    low  += off;
    high += off;
    int mid = (unsigned)(low + high) >> 1;
    java_sort_void_merge_sort(dest, src, low, mid, -off, c);
    java_sort_void_merge_sort(dest, src, mid, high, -off, c);

    // If list is already sorted, just copy from src to dest.  This is an
    // optimization that results in faster sorts for nearly ordered lists.
    if (c(src[mid-1], src[mid]) <= 0) {
        for(i=0;i<length;i++){
            dest[destLow+i]=src[low+i];
        }
        return;
    }

    // Merge sorted halves (now in src) into dest
    for(i = destLow, p = low, q = mid; i < destHigh; i++) {
        if (q >= high || p < mid && c(src[p], src[q]) <= 0)
            dest[i] = src[p++];
        else
            dest[i] = src[q++];
    }
}

static void java_sort_void_swap(void **x, int a, int b){
    void *t=x[a];
    x[a]=x[b];
    x[b]=t;
}

//==================== math

double dotProduct(double *vs1, double *vs2, int len){
    double res=0.0;
    int i;
    for(i=0;i<len;i++){
        res+=(vs1[i]*vs2[i]);
    }
    return res;
}

double norm_L2(double *vs, int len){
    double res=0.0;
    int i;
    for(i=0;i<len;i++){
        res+=(vs[i]*vs[i]);
    }
    return sqrt(res);
}

void normalizeArray(double *vs, int len){
    double total=0.0;
    int i;
    for(i=0;i<len;i++){
        total+=vs[i];
    }
    for(i=0;i<len;i++){
        vs[i]/=total;
    }
}

void avg_unbiased_var(int len, double *arr, double *res_avg, double *res_var){
    if(len<1){
        *(res_avg)=0.0;
        *(res_var)=1.0;
        return;
    }else if(len==1){
        *(res_avg)=arr[0];
        *(res_var)=0.0;
        return;
    }

    int i;
    double avg=0.0, var=0.0;
    for(i=0;i<len;i++) avg+=arr[i];
    avg/=(double)len;
    for(i=0;i<len;i++){
        double d=arr[i]-avg;
        var+=(d*d);
    }
    var=sqrt(var/(len-1));

    *(res_avg)=avg;
    *(res_var)=var;
}

void avg_biased_var(int len, double *arr, double *res_avg, double *res_var){
    if(len<1){
        *(res_avg)=0.0;
        *(res_var)=1.0;
        return;
    }else if(len==1){
        *(res_avg)=arr[0];
        *(res_var)=0.0;
        return;
    }

    int i;
    double avg=0.0, var=0.0;
    for(i=0;i<len;i++) avg+=arr[i];
    avg/=(double)len;
    for(i=0;i<len;i++){
        double d=arr[i]-avg;
        var+=(d*d);
    }
    var=sqrt(var/len);

    *(res_avg)=avg;
    *(res_var)=var;
}

inline double norm_pdf(double x, double mu, double sigma){
    return (1.0/(sqrt(2.0*M_PI)*sigma))*exp(-pow(x-mu, 2.0)/(2.0*sigma*sigma));
}

double norm_cdf(double x, double mu, double sigma){
    x-=mu;
    x/=sigma;
    if(x==0.0) return 0.5;
    else if(x<0.0){
        x=-x;
        double t, sp=x/1000.0, base=1.0/sqrt(2.0*M_PI), value=0.0;
        for(t=sp/2.0;t<x;t+=sp) value+=exp(-t*t/2.0);
        double res=0.5-sp*base*value;
        return res>0.0 ? res:0.0;
    }else{
        double t, sp=x/1000.0, base=1.0/sqrt(2.0*M_PI), value=0.0;
        for(t=sp/2.0;t<x;t+=sp) value+=exp(-t*t/2.0);
        double res=0.5+sp*base*value;
        return res<1.0 ? res:1.0;
    }
}

double gammln_c[6]={76.18009172947148,-86.50532941677,24.01409824083091,-1.231739572450155,0.120865097386617e-2,-0.5395239384953e-5};

double log_gamma(double x){
    int i;
    double t,s;

    if(x<0){printf("incorrect input parameter\n");return(0);}

    s=1.000000000190015;
    for(i=0;i<6;i++) s=s+gammln_c[i]/(x+i);
    t=x+4.5;
    t=t-(x-0.5)*log(t);
    t=log(2.5066282746310005*s)-t;
    return(t);
}

double gamm2(double a,double x,double e1,double e0){
    int n;
    double t,del,gln;
    double an,bn,c,d;

    if((x<0.0)||(a<=0)){printf("x<0.0 or a<=0.0\n");return(0);}
    if(x<e0) return(0.0);

    gln=log_gamma(a);
    if(x<(a+1.0)){
     	del=1.0/a;
        t=1.0/a;
        for(n=1;n<100;n++){
            del=del*x/(a+n);
            t=t+del;
            if(fabs(del)<fabs(t)*e1){
             	t=t*exp(-x+a*log(x)-gln);
                return(t);
            }
        }
        printf("iteration too many times\n");
        return(0);
    }else{
     	bn=x+1.0-a;
        c=1.0/e0;
        d=1.0/bn;
        t=d;
        for(n=1;n<100;n++){
            an=n*(a-n);
            bn=bn+2.0;
            d=an*d+bn;
            c=bn+an/c;
            if(fabs(d)<e0)
                d=e0;
            if(fabs(c)<e0)
                c=e0;
            d=1.0/d;
            del=d*c;
            t=t*del;
            if(fabs(del-1.0)<e1){
             	t=exp(-x+a*log(x)-gln)*t;
                t=1.0-t;
                return(t);
            }

        }
        printf("iteration too many times\n");
        return(0);
    }
}

inline double chi2_pdf(double x, int k){
    return (1.0/(pow(2.0, k/2.0)*exp(log_gamma(k/2.0))))*pow(x, k/2.0-1.0)*exp(-x/2.0);
}

inline double chi2_cdf(double x, int k){
    return x<0.0 ? 0.0:gamm2(k/2.0,x/2.0,1.0e-6,1.0e-30);
}

//================ socket

#ifdef _linux

int is_valid_ip(const char *ip){
    if(!ip) return 0;
    //--
    int section=0;
    int dot=0;
    while(*ip){
        if(*ip == '.'){
            dot++;
            if(dot > 3) return 0;
            if(section<0 || section >255) return 0;
            section=0;
        }else if(*ip >= '0' && *ip <= '9'){
            section = section * 10 + *ip - '0';
        }else{
            return 0;
        }
        ip++;
    }
    if(section<0 || section >255) return 0;
    if(dot!=3) return 0;
    //--
    return 1;
}

char *get_local_ip(){
    char hname[128];
    struct hostent *hent;

    gethostname(hname, sizeof(hname));
    hent = gethostbyname(hname);

    if(!hent || !hent->h_addr_list[0]){
        fprintf(stderr, "cannot obtain local ip, exit!\n");
        exit(0);
    }

    char *res=inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0]));
    return str_copy(res);;
}

int socket_listen(int port){
    int struct_len, numbytes,i;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0, 8);
    struct_len = sizeof(struct sockaddr_in);

    int listen_fd= socket(AF_INET, SOCK_STREAM, 0);
    while(bind(listen_fd, (struct sockaddr *)&server_addr, struct_len) == -1);
    while(listen(listen_fd, 10) == -1);
    mylog("Ready for Accept, listening...");

    return listen_fd;
}

int socket_accept(int listen_id){
    struct sockaddr_in client_addr;
    int struct_len = sizeof(struct sockaddr_in);
    int sock_fd=accept(listen_id, (struct sockaddr *)&client_addr, &struct_len);
    sprintf(loginfo, "%s:%d accept!", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));mylog(loginfo);
    set_socket_buff_size(sock_fd);
    return sock_fd;
}

int socket_connect(char *ip, int port){
    int sockfd,numbytes;
    char buf[BUFSIZ];
    struct sockaddr_in their_addr;

    while((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1);
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    //--
    if(is_valid_ip(ip)) their_addr.sin_addr.s_addr=inet_addr(ip);
    else{
        struct hostent *hent=gethostbyname(ip);
        if(!hent){
            fprintf(stderr, "ip=%s is invalid, exit!\n");
            exit(0);
        }
        char *tmpIp=inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0]));
        their_addr.sin_addr.s_addr=inet_addr(tmpIp);
        free(tmpIp);
    }
    //--
    memset(&(their_addr.sin_zero), 0, 8);
    while(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) == -1);
    sprintf(loginfo, "%s:%d connected!", ip, port);mylog(loginfo);
    set_socket_buff_size(sockfd);

    return sockfd;
}

void set_socket_buff_size(int sock_id){
    unsigned int uiRcvBuf = 0;
    unsigned int uiNewRcvBuf = 0;
    int uiRcvBufLen = sizeof(uiRcvBuf);
    int type=getsockopt(sock_id, SOL_SOCKET, SO_SNDBUF,(char*)&uiRcvBuf, &uiRcvBufLen);
    if(type<0){
        mylog("obtain socket buff size error");
        return;
    }
    uiRcvBuf=socketBuffSize;
    type=setsockopt(sock_id, SOL_SOCKET, SO_SNDBUF,(char*)&uiRcvBuf, uiRcvBufLen);
    if(type<0){
        mylog("set socket buff size error");
        return;
    }
}

void socket_send_data(int socket_id, char *buff, int64_t size){
    int sendSize=send(socket_id, &size, 8, 0);
    if(sendSize<0){
        sprintf(loginfo, "send error1(socket_id=%d)", socket_id);mylog(loginfo);
        exit(0);
    }else if(sendSize==0){
        sprintf(loginfo, "socket disconnected1(socket_id=%d)", socket_id);mylog(loginfo);
        exit(0);
    }
    //--
    char log[1000];
    sprintf(log, "%s sending(%lld)..", getTime(), size);
    while(size){
        int64_t len=size<socketBuffSize ? size:socketBuffSize;
        sendSize=send(socket_id, buff, (int)len, 0);
        if(sendSize<0){
            sprintf(loginfo, "send error2(socket_id=%d)", socket_id);mylog(loginfo);
            exit(0);
        }else if(sendSize==0){
            sprintf(loginfo, "socket disconnected2(socket_id=%d)", socket_id);mylog(loginfo);
            exit(0);
        }
        buff+=sendSize;
        size-=sendSize;
    }
    fprintf(stdout, "%s\n%s sent.\n", log, getTime());
}

void socket_receive_data(int socket_id, char *buff, int64_t *resSize){
    int64_t size;
    int receivedSize=recv(socket_id, &size, 8, 0);
    if(receivedSize<0){
        sprintf(loginfo, "receive error1(socket_id=%d)", socket_id);mylog(loginfo);
        exit(0);
    }else if(receivedSize==0){
        sprintf(loginfo, "socket disconnected1(socket_id=%d)", socket_id);mylog(loginfo);
        exit(0);
    }
    *(resSize)=size;
    //--
    char log[1000];
    sprintf(log, "%s receiving..", getTime());
    while(size){
        int64_t len=size<socketBuffSize ? size:socketBuffSize;
        receivedSize=recv(socket_id, buff, (int)len, 0);
        if(receivedSize<0){
            sprintf(loginfo, "receive error2(socket_id=%d)", socket_id);mylog(loginfo);
            exit(0);
        }else if(receivedSize==0){
            sprintf(loginfo, "socket disconnected2(socket_id=%d)", socket_id);mylog(loginfo);
            exit(0);
        }
        buff+=receivedSize;
        size-=receivedSize;
    }
    fprintf(stdout, "%s\n%s received(%lld).\n", log, getTime(), *(resSize));
}

#endif

//================

