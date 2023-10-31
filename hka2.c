#include "hka2.h"

#define MIN_POISSON_LAMBDA 1e-50
#define MIN_POISSON_PROP 1e-4

int min_for_filter_window=0;
double func_mu=2.5e-8;
double max_mu=10.0;
double min_lambda=1e-4;
double max_lambda=100.0;
double initial_N0=10000.0;
double initial_N0_low=100.0;
double initial_N0_up=1000000.0;
double initial_N1=10000.0;
double initial_N1_low=100.0;
double initial_N1_up=1000000.0;
double initial_N2=10000.0;
double initial_N2_low=100.0;
double initial_N2_up=1000000.0;
double initial_T=200000.0;
double initial_T_low=100.0;
double initial_T_up=10000000.0;
int is_print_s2=0;
int is_output_true=0;

int main(int argc, char **argv){
    load_parameters(argc, argv);

    mylog("starting...");

    initial_bio();

    int i, j, len;
    double like1=0.0, like2=0.0;

    pool=new_thread_pool(thread_num);
    p_locker=my_new(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(p_locker, NULL);

    if(afs_file){
        S2 *afs_s2=read_afs_file();
        fprintf(stdout, "n1=%d\tn2=%d\n", afs_s2->dim1-1, afs_s2->dim2-1);

        mylog("estimating global parameters(N0, N1, N2, T)...");
        int npara=4;
        double *x=my_new(npara, sizeof(double));
        double *low=my_new(npara, sizeof(double));
        double *up=my_new(npara, sizeof(double));
        int *nbd=my_new(npara, sizeof(int));for(i=0;i<npara;i++) nbd[i]=2;
        x[0]=10000;low[0]=100,up[0]=1000000;   //N0
        x[1]=10000;low[1]=100,up[1]=1000000;   //N1
        x[2]=10000;low[2]=100,up[2]=1000000;   //N2
        x[3]=1000;low[3]=100,up[3]=50000;   //T
        fprintf(stdout, "initial: N0=%d\tlow=%d\tup=%d\n", (int)x[0], (int)low[0], (int)up[0]);
        fprintf(stdout, "initial: N1=%d\tlow=%d\tup=%d\n", (int)x[1], (int)low[1], (int)up[1]);
        fprintf(stdout, "initial: N2=%d\tlow=%d\tup=%d\n", (int)x[2], (int)low[2], (int)up[2]);
        fprintf(stdout, "initial:  T=%d\tlow=%d\tup=%d\n", (int)x[3], (int)low[3], (int)up[3]);
        if(optimize_type==0) findmax_bfgs(npara, x, afs_s2, log_likelihood_afs_func, NULL, low, up, nbd, 0);
        else if(optimize_type==1) kmin_hj2(npara, x, afs_s2, log_likelihood_afs_func, low, up);
        like1=log_likelihood_afs_func(x, afs_s2);
        len=sprintf(loginfo, "estimate: like=%.1f\tN0=%.1f\tN1=%.1f\tN2=%.1f\tT=%.1f\n", like1, x[0], x[1], x[2], x[3]);
        fprintf(stdout, "%s", loginfo);

        GzStream *out=gz_stream_open(out_file, "w");
        gz_write(out, loginfo, len);
        gz_stream_destory(out);

        free(x);
        free(low);
        free(up);
        free(nbd);
        goto last;
    }

    all_wins=read_win_size_file(win_size_file);
    all_wins_global=read_win_size_file(win_size_file_for_global);
    all_s2s=read_data();
    initial_cega(func_n1, func_n2);

    for(i=0;i<all_s2s->size;i++) thread_pool_add_worker(pool, complete_S2, all_s2s->elementData[i]);
    thread_pool_invoke_all(pool);
    if(win_size_file_for_global){
        for(i=0;i<all_s2s_global->size;i++) thread_pool_add_worker(pool, complete_S2, all_s2s_global->elementData[i]);
        thread_pool_invoke_all(pool);
    }
    if(is_print_s2){
        for(i=0;i<all_s2s->size;i++){
            Win1 *win=all_s2s_wins->elementData[i];
            S2 *s2=all_s2s->elementData[i];
            fprintf(stderr, "%s-%d-%d\n", win->chr, win->start, win->end);
            print_S2_1(s2);
        }
    }

    AList_i *to_cul_s2s_index=new_alist_i(16);
    for(i=0;i<all_s2s_wins->size;i++){
        S2 *s2=all_s2s->elementData[i];
        double tmp=s2->s1+s2->s2+s2->s12+s2->D;
        if(tmp<(double)min_for_filter_window) continue;
        //--
        alist_i_add(to_cul_s2s_index, i);
    }

    global_L=0.0;
    for(i=0;i<all_s2s_wins_global->size;i++){
        Win1 *win=all_s2s_wins_global->elementData[i];
        global_L+=win->length==0 ? (win->end-win->start+1):(win->length);
    }

    mylog("estimating global parameters(N0, N1, N2, T)...");
    int npara=4;
    double *x=my_new(npara, sizeof(double));
    double *low=my_new(npara, sizeof(double));
    double *up=my_new(npara, sizeof(double));
    int *nbd=my_new(npara, sizeof(int));for(i=0;i<npara;i++) nbd[i]=2;
    x[0]=initial_N0;low[0]=initial_N0_low,up[0]=initial_N0_up;   //N0
    x[1]=initial_N1;low[1]=initial_N1_low,up[1]=initial_N1_up;   //N1
    x[2]=initial_N2;low[2]=initial_N2_low,up[2]=initial_N2_up;   //N2
    x[3]=initial_T;low[3]=initial_T_low,up[3]=initial_T_up;   //T
    //grid_paras(npara, x, low, up, 1000, log_likelihood_func, NULL);
    fprintf(stdout, "initial: N0=%d\tlow=%d\tup=%d\n", (int)x[0], (int)low[0], (int)up[0]);
    fprintf(stdout, "initial: N1=%d\tlow=%d\tup=%d\n", (int)x[1], (int)low[1], (int)up[1]);
    fprintf(stdout, "initial: N2=%d\tlow=%d\tup=%d\n", (int)x[2], (int)low[2], (int)up[2]);
    fprintf(stdout, "initial:  T=%d\tlow=%d\tup=%d\n", (int)x[3], (int)low[3], (int)up[3]);
    //fprintf(stdout, "win_size=%d\tglobal_size=%d\n", win_size, (int)global_L);
    optimize_type=1;
    if(optimize_type==0) findmax_bfgs(npara, x, NULL, log_likelihood_func, NULL, low, up, nbd, 0);
    else if(optimize_type==1) kmin_hj2(npara, x, NULL, log_likelihood_func, low, up);
    optimize_type=0;
    global_N0=x[0];
    global_N1=x[1];
    global_N2=x[2];
    global_T=x[3];
    like1=log_likelihood_func(x, NULL);
    fprintf(stdout, "estimate: like=%.1f\tN0=%.1f\tN1=%.1f\tN2=%.1f\tT=%.1f\n", like1, x[0], x[1], x[2], x[3]);
    //--
    double *xx=my_new(npara, sizeof(double));
    if(is_output_true){
        xx[0]=10000*2.0;
        xx[1]=20000*2.0;
        xx[2]=10000*2.0;
        xx[3]=200000;
        like2=log_likelihood_func(xx, NULL);
        S2 *tmp=calloc(1, sizeof(S2));
        tmp->dim1=global_s2->dim1;
        tmp->dim2=global_s2->dim2;
        tmp->arr=calloc(tmp->dim1*tmp->dim2, sizeof(double));
        estimate_exp_jafs(tmp, tmp->arr, tmp->dim1, tmp->dim2, xx[0], xx[1], xx[2], func_n1, func_n2, xx[3], func_mu, 1.0, win_size, NULL, NULL);
        fprintf(stdout, "true    : like=%.1f\tN0=%.1f\tN1=%.1f\tN2=%.1f\tT=%.1f\ts1=%.2f\ts2=%.2f\ts12=%.2f\tD=[%.2f,%.2f,%.2f]\n", like2, xx[0], xx[1], xx[2], xx[3], tmp->s1, tmp->s2, tmp->s12, tmp->D1, tmp->D2, tmp->D);
        free(tmp->arr);
        free(tmp);
    }

    if(is_calculate_LRT==0){
        mylog("estimating mu lambda...");
        res_2_mu=my_new(all_s2s_wins->size, sizeof(double));
        res_2_lambda1=my_new(all_s2s_wins->size, sizeof(double));
        res_2_lambda2=my_new(all_s2s_wins->size, sizeof(double));
        res_2_like=my_new(all_s2s_wins->size, sizeof(double));
        for(i=0;i<to_cul_s2s_index->size;i++) thread_pool_add_worker(pool, optimize_mu_lambda_for_win, to_cul_s2s_index->elementData[i]);
        thread_pool_invoke_all(pool);
    }else{
        mylog("estimating mu...");
        res_1_mu=my_new(all_s2s_wins->size, sizeof(double));
        res_1_like=my_new(all_s2s_wins->size, sizeof(double));
        for(i=0;i<to_cul_s2s_index->size;i++) thread_pool_add_worker(pool, optimize_mu_for_win, to_cul_s2s_index->elementData[i]);
        thread_pool_invoke_all(pool);

        mylog("estimating mu lambda1...");
        res_2_lambda1=my_new(all_s2s_wins->size, sizeof(double));
        res_2_lambda2=my_new(all_s2s_wins->size, sizeof(double));
        res_2_like_mu_lambda1= my_new(all_s2s_wins->size, sizeof(double));
        for(i=0;i<to_cul_s2s_index->size;i++) thread_pool_add_worker(pool, optimize_mu_lambda1_for_win, to_cul_s2s_index->elementData[i]);
        thread_pool_invoke_all(pool);

        mylog("estimating mu lambda2...");
        res_2_like_mu_lambda2= my_new(all_s2s_wins->size, sizeof(double));
        for(i=0;i<to_cul_s2s_index->size;i++) thread_pool_add_worker(pool, optimize_mu_lambda2_for_win, to_cul_s2s_index->elementData[i]);
        thread_pool_invoke_all(pool);

        res_2_LLR1= my_new(all_s2s_wins->size, sizeof(double));
        res_2_LLR2= my_new(all_s2s_wins->size, sizeof(double));

        double total_LLR1=0.0, total_LLR2=0.0;
        for(i=0;i<to_cul_s2s_index->size;i++){
            double v1=2.0*(res_1_like[i]-res_2_like_mu_lambda1[i]);
            double v2=2.0*(res_1_like[i]-res_2_like_mu_lambda2[i]);
            //fprintf(stderr, "LLR1=%f\tLLR2=%f\tlog_L1_lambda1=%f\tlog_L1_lambda2=%f\tL0=%f\n", v1, v2, res_2_like_mu_lambda2[i], res_2_like_mu_lambda1[i], res_1_like[i]);
            res_2_LLR1[i]=v1;
            res_2_LLR2[i]=v2;
            total_LLR1+=v1;
            total_LLR2+=v2;
        }
        total_LLR1/=all_s2s_wins->size;
        total_LLR2/=all_s2s_wins->size;

        double filter_LLR1=0.0, filter_LLR2=0.0, max1=6.6349*total_LLR1, max2=6.6349*total_LLR2;
        int num1=0, num2=0;
        for(i=0;i<to_cul_s2s_index->size;i++){
            if(res_2_LLR1[i]<max1){
                num1++;
                filter_LLR1+=res_2_LLR1[i];
            }
            if(res_2_LLR2[i]<max2){
                num2++;
                filter_LLR2+=res_2_LLR2[i];
            }
        }
        if(num1>0) filter_LLR1/=num1;
        if(num2>0) filter_LLR2/=num2;

        //fprintf(stderr, "avg_LLR1=%f\tavg_LLR2=%f\tfilter_LLR1=%f\tfilter_LLR2=%f\tmax1=%f\tmax2=%f\n", total_LLR1, total_LLR2, filter_LLR1, filter_LLR2, max1, max2);

        if(filter_LLR1!=0.0){
            for(i=0;i<to_cul_s2s_index->size;i++) res_2_LLR1[i]/=filter_LLR1;
        }
        if(filter_LLR2!=0.0){
            for(i=0;i<to_cul_s2s_index->size;i++) res_2_LLR2[i]/=filter_LLR2;
        }
    }

    double lambda1_avg=0.0, lambda1_var=0.0, lambda2_avg=0.0, lambda2_var=0.0;
    AList_d *L1= new_alist_d(to_cul_s2s_index->size);
    AList_d *L2= new_alist_d(to_cul_s2s_index->size);
    AList_d *nlambda1= new_alist_d(to_cul_s2s_index->size);
    AList_d *nlambda2= new_alist_d(to_cul_s2s_index->size);
    AList_d *n_p1= new_alist_d(to_cul_s2s_index->size);
    AList_d *n_p2= new_alist_d(to_cul_s2s_index->size);
    for(i=0;i<to_cul_s2s_index->size;i++) {
        int index = to_cul_s2s_index->elementData[i];
        alist_d_add(L1, res_2_lambda1[index]);
        alist_d_add(L2, res_2_lambda2[index]);
    }
    get_nlambda_and_P(L1, nlambda1, n_p1);
    get_nlambda_and_P(L2, nlambda2, n_p2);
    avg_biased_var(L1->size, L1->elementData, &lambda1_avg, &lambda1_var);
    avg_biased_var(L2->size, L2->elementData, &lambda2_avg, &lambda2_var);

    len=sprintf(loginfo, "log_lambda1_avg=%f\tlog_lambda1_var=%f\tlog_lambda2_avg=%f\tlog_lambda2_var=%f", lambda1_avg, lambda1_var, lambda2_avg, lambda2_var);mylog(loginfo);

    mylog("writing results into file...");
    GzStream *out=gz_stream_open(out_file, "w");
    //gz_write(out, loginfo, len);gz_write_char(out, '\n');
    len=sprintf(loginfo, "Global parameters:\tN0=%f\tN1=%f\tN2=%f\tT=%f\n", x[0], x[1], x[2], x[3]);
    gz_write(out, loginfo, len);
    if(is_output_true){
        len=sprintf(loginfo, "true:    \tN0=%f\tN1=%f\tN2=%f\tT=%f\tlike=%f\n", xx[0], xx[1], xx[2], xx[3], like2);
        gz_write(out, loginfo, len);
    }
    //len=sprintf(loginfo, "global:s1=%d/%.2f\ts2=%d/%.2f\ts12=%d/[%.2f,%.2f,%.2f]\tD=%d/[%.2f,%.2f,%.2f]\n", (int)(global_s2->s1), global_s2->estimate_s1, (int)(global_s2->s2), global_s2->estimate_s2, (int)(global_s2->s12), global_s2->s12_1, global_s2->s12_2, global_s2->estimate_s12, (int)(global_s2->D), global_s2->D1, global_s2->D2, global_s2->estimate_D);
    //gz_write(out, loginfo, len);
    //--
    for(i=0;i<to_cul_s2s_index->size;i++){
        int index=to_cul_s2s_index->elementData[i];
        Win1 *win=all_s2s_wins->elementData[index];
        int length=win->length==0 ? (win->end-win->start+1):win->length;
        S2 *s2=all_s2s->elementData[index];
        //double span_like=res_1_like[index]-res_2_like[index];
        //double p1=1.0-chi2_cdf(span_like*2.0, 1);
        //double p2_1=norm_cdf(log(res_2_lambda1[index]), lambda1_avg, lambda1_var);
        //double p2_2=norm_cdf(log(res_2_lambda2[index]), lambda2_avg, lambda2_var);
        if(is_calculate_LRT==0){
            len=sprintf(loginfo, "%s:%d-%d\ts1=%d\ts2=%d\ts12=%d\tD=%d\tmu=%f\tlambda1=%f\tlambda2=%f\tnlambda1=%f\tp1=%e\tnlambda2=%f\tp2=%e", win->chr, win->start, win->end, (int)(s2->s1), (int)(s2->s2), (int)(s2->s12), (int)(s2->D), res_2_mu[index]*func_mu*length, res_2_lambda1[index], res_2_lambda2[index], nlambda1->elementData[i], n_p1->elementData[i], nlambda2->elementData[i], n_p2->elementData[i]);
            gz_write(out, loginfo, len);
        }else{
            len=sprintf(loginfo, "%s:%d-%d\ts1=%d\ts2=%d\ts12=%d\tD=%d\tmu=%f\tlambda1=%f\tlambda2=%f", win->chr, win->start, win->end, (int)(s2->s1), (int)(s2->s2), (int)(s2->s12), (int)(s2->D), res_1_mu[index]*func_mu*length, res_2_lambda1[index], res_2_lambda2[index]);
            gz_write(out, loginfo, len);
            double p1_LRT= 1.0-chi2_cdf(res_2_LLR1[i], 1);
            double p2_LRT= 1.0-chi2_cdf(res_2_LLR2[i], 1);
            gz_write(out, loginfo, sprintf(loginfo, "\tLLR1=%f\tLLR2=%f\tp1(LRT)=%e\tp2(LRT)=%e", res_2_LLR1[i], res_2_LLR2[i], p1_LRT, p2_LRT));
        }
        gz_write_char(out, '\n');
    }
    gz_stream_destory(out);

    last:
    free_thread_pool(pool);
    pthread_mutex_destroy(p_locker);
    free(p_locker);

    mylog("finished!");

    return 0;
}

void load_parameters(int argc, char **argv){
    argc--;
    argv++;

    if(argc<4) print_usage();

    pop1_file=NULL;
    pos1_file=NULL;
    pop2_file=NULL;
    pos2_file=NULL;
    afs_file=NULL;
    global_L=0.0;
    thread_num=1;
    optimize_type=0;
    win_size=10000;
	step_size=1000;
    win_size_file=NULL;
    win_size_file_for_global=NULL;
    is_calculate_LRT=0;
    out_file=NULL;

    int i;
    for(i=0;i<argc;i++){
        if(strcmp(argv[i], "-i1")==0){
            i++;
            pop1_file=argv[i];
        }else if(strcmp(argv[i], "-p1")==0){
            i++;
            pos1_file=argv[i];
        }else if(strcmp(argv[i], "-i2")==0){
            i++;
            pop2_file=argv[i];
        }else if(strcmp(argv[i], "-p2")==0){
            i++;
            pos2_file=argv[i];
        }else if(strcmp(argv[i], "-afs")==0){
            i++;
            afs_file=argv[i];
        }else if(strcmp(argv[i], "-t")==0) {
            i++;
            thread_num = atoi(argv[i]);
        }else if(strcmp(argv[i], "-d")==0){
            i++;
            min_for_filter_window=atoi(argv[i]);
        }else if(strcmp(argv[i], "-N0")==0){
            i++;
            initial_N0=atof(argv[i]);
            i++;
            initial_N0_low=atof(argv[i]);
            i++;
            initial_N0_up=atof(argv[i]);
        }else if(strcmp(argv[i], "-N1")==0){
            i++;
            initial_N1=atof(argv[i]);
            i++;
            initial_N1_low=atof(argv[i]);
            i++;
            initial_N1_up=atof(argv[i]);
        }else if(strcmp(argv[i], "-N2")==0){
            i++;
            initial_N2=atof(argv[i]);
            i++;
            initial_N2_low=atof(argv[i]);
            i++;
            initial_N2_up=atof(argv[i]);
        }else if(strcmp(argv[i], "-T")==0){
            i++;
            initial_T=atof(argv[i]);
            i++;
            initial_T_low=atof(argv[i]);
            i++;
            initial_T_up=atof(argv[i]);
        }else if(strcmp(argv[i], "-L")==0){
            i++;
            global_L=atof(argv[i]);
        }else if(strcmp(argv[i], "-mu")==0){
            i++;
            func_mu=atof(argv[i]);
        }else if(strcmp(argv[i], "-op")==0){
            i++;
            optimize_type=atoi(argv[i]);
        }else if(strcmp(argv[i], "-ws")==0){
            i++;
            win_size=atoi(argv[i]);
			i++;
			step_size=atoi(argv[i]);
        }else if(strcmp(argv[i], "-wf")==0){
            i++;
            win_size_file=argv[i];
            win_size=0;
        }else if(strcmp(argv[i], "-wf_g")==0){
            i++;
            win_size_file_for_global=argv[i];
        }else if(strcmp(argv[i], "-LRT")==0){
            i++;
            is_calculate_LRT= atoi(argv[i]);
        }else if(strcmp(argv[i], "-o")==0){
            i++;
            out_file=argv[i];
        }else{
            fprintf(stderr, "%s is not include in parameters, please check!\n", argv[i]);
            exit(0);
        }
    }

    if(thread_num<1) thread_num=1;

    if(afs_file) return;

    if(pop1_file==NULL){
        fprintf(stderr, "-i1 is needed!\n");
        exit(0);
    }else if(!str_ends(pop1_file, ".hap") && !str_ends(pop1_file, ".vcf") && !str_ends(pop1_file, ".vcf.gz") && !str_ends(pop1_file, ".tped")){
        fprintf(stderr, "-i1 is unrecognized format, please check!\n");
        exit(0);
    }else if(str_ends(pop1_file, ".hap") && pos1_file==NULL){
        fprintf(stderr, "-p1 is needed!\n");
        exit(0);
    }

    if(pop2_file==NULL){
        fprintf(stderr, "-i2 is needed!\n");
        exit(0);
    }else if(!str_ends(pop2_file, ".hap") && !str_ends(pop2_file, ".vcf") && !str_ends(pop2_file, ".vcf.gz") && !str_ends(pop2_file, ".tped")){
        fprintf(stderr, "-i2 is unrecognized format, please check!\n");
        exit(0);
    }else if(str_ends(pop2_file, ".hap") && pos2_file==NULL){
        fprintf(stderr, "-p2 is needed!\n");
        exit(0);
    }

    if(optimize_type!=0 && optimize_type!=1){
        fprintf(stderr, "-op value must be '0' or '1', please check!\n");
        exit(0);
    }

    if(out_file==NULL){
        fprintf(stderr, "-o is needed!\n");
        exit(0);
    }
}

void print_usage(){
    printf("==============================\n");
    printf("Author:   ChiLianjiang\n");
    printf("E-mail:   chilianjiang@126.com\n");
    printf("Date:     2023-10-30\n");
    printf("Version:  1.3\n\n");
    printf("Usage: CEGA [arguments]\n");
    printf("  input:\n");
    printf("    -i1         population1 genetic variant file(.hap .vcf .vcf.gz .tped)\n");
    printf("    -p1         population1 position file(format:chr position, split by tab), only for .hap(-i1), other format not need\n");
    printf("    -i2         population2 genetic variant file(.hap .vcf .vcf.gz .tped)\n");
    printf("    -p2         population2 position file(format:chr position, split by tab), only for .hap(-i2), other format not need\n");
    //printf("    -afs        input afs file\n");
    printf("  options:\n");
    printf("    -N0         (double)initial lowbound upbound(defalut:10000.0 100.0 1000000.0)\n");
    printf("    -N1         (double)initial lowbound upbound(defalut:10000.0 100.0 1000000.0)\n");
    printf("    -N2         (double)initial lowbound upbound(defalut:10000.0 100.0 1000000.0)\n");
    printf("    -T          (double)initial lowbound upbound(defalut:10000.0 100.0 10000000.0)\n");
    printf("    -t          (int)thread number(default:1)\n");
    printf("    -d          (int) filtering windows with s1+s2+s12+D < this value (default: 0)\n");
    //printf("    -L          (int)all_chrom_length.(only for -afs)\n");
    printf("    -mu         (double)mutation rate(default:2.5e-8). Unit: per base per generation\n");
    //printf("    -op         (int)0:bfgs, 1:kmin_hj(default:0)\n");
    printf("    -ws         (int)window_size step_size(default: 10000 1000). Unit: bp\n");
    printf("    -wf         (file)window file(format:chr start(1-base,include) end(1-base,include) effective_length, split by tab), if input, '-ws' is disable(default:null)\n");
    printf("    -wf_g       (file)window file to specify neutral genome region for estimating global parameters, format same to '-wf'(default:null)\n");
    printf("    -LRT        (int)1: implement CEGA-LRT (likelihood ratio test), 0: implement CEGA-lambda(default:0)\n");
    printf("  output:\n");
    printf("    -o          output file name\n\n");
    exit(0);
}

//==============================

Win3 *read_win_size_file(char *file){
    if(file==NULL) return NULL;
    sprintf(loginfo, "reading %s...", file);mylog(loginfo);

    Win3 *win3=my_new(1, sizeof(Win3));

    int i, j, len;
    int tabs[100];

    char *chr=NULL;
    Win2 *win2=NULL;

    int64_t all_win_size=0, n_all_win_size=0;

    GzStream *gz1=gz_stream_open(file, "r");
    while(gz_read_util(gz1, '\n', line, maxline, &len)){
        len=chmop_with_len(line, len);
        //--
        int num=str_tab_index(line, '\t', tabs);
        if(num!=4){
            fprintf(stderr, "error format(%s).\n", win_size_file);
            exit(0);
        }
        for(i=0;i<num;i++) line[tabs[i]]='\0';
        int start=atoi(line+tabs[0]+1);
        int end=atoi(line+tabs[1]+1);
        int length=atoi(line+tabs[2]+1);
        all_win_size+=length;
        n_all_win_size++;
        //--
        if(chr==NULL || strcmp(chr, line)!=0){
            chr=str_copy_with_len(line, tabs[0]);
            win2=my_new(1, sizeof(Win2));
            //--
            win3->chrs=my_renew(win3->chrs, (win3->l_chr+1)*sizeof(char *));
            win3->data=my_renew(win3->data, (win3->l_chr+1)*sizeof(Win2 *));
            win3->chrs[win3->l_chr]=chr;
            win3->data[win3->l_chr++]=win2;
        }
        if(win2->l_win==win2->t_win){
            win2->t_win+=1000;
            win2->wins=my_renew(win2->wins, win2->t_win*sizeof(Win1 *));
        }
        Win1 *win1=my_new(1, sizeof(Win1));
        win1->start=start;
        win1->end=end;
        win1->length=length;
        win2->wins[win2->l_win++]=win1;
    }
    gz_stream_destory(gz1);

    if(file==win_size_file) win_size=(int)(all_win_size/n_all_win_size);

    return win3;
}

AList_l *read_data(){
    mylog("reading population data...");

    Pop2 *data1=NULL;
    Pop2 *data2=NULL;

    if(str_ends(pop1_file, ".hap")) data1=read_hap_file(pop1_file, pos1_file);
    else if(str_ends(pop1_file, ".tped")) data1=read_tped_file(pop1_file);
    else data1=read_vcf_file(pop1_file);

    if(str_ends(pop2_file, ".hap")) data2=read_hap_file(pop2_file, pos2_file);
    else if(str_ends(pop2_file, ".tped")) data2=read_tped_file(pop2_file);
    else data2=read_vcf_file(pop2_file);

    func_n1=data1->n_sample;
    func_n2=data2->n_sample;

    AList_l *list=new_alist_l(16);

    int i, j;

    mylog("calculating S2...");

    global_s2=my_new(1, sizeof(S2));
    global_s2->dim1=data1->n_sample+1;
    global_s2->dim2=data2->n_sample+1;
    global_s2->arr=my_new(global_s2->dim1*global_s2->dim2, sizeof(double));

    all_s2s_wins=new_alist_l(16);

    if(all_wins){
        for(i=0;i<all_wins->l_chr;i++){
            Win2 *win2=all_wins->data[i];
            Pop1 *pp1=get_pop1_by_chr(all_wins->chrs[i], data1);
            Pop1 *pp2=get_pop1_by_chr(all_wins->chrs[i], data2);
            if(pp1==NULL || pp2==NULL) continue;
            int *flag1=my_new(1000000000, sizeof(int));
            int *flag2=my_new(1000000000, sizeof(int));
            for(j=0;j<pp1->l_data;j++) flag1[pp1->pos[j]]=pp1->data[j];
            for(j=0;j<pp2->l_data;j++) flag2[pp2->pos[j]]=pp2->data[j];
            for(j=0;j<win2->l_win;j++){
                Win1 *win1=win2->wins[j];
                alist_l_add(list, get_S2(flag1, flag2, win1->start, win1->end, data1->n_sample+1, data2->n_sample+1));
                Win1 *tmp= my_new(1, sizeof(Win1));
                tmp->chr=all_wins->chrs[i];
                tmp->start=win1->start;
                tmp->end=win1->end;
                tmp->length=win1->length;
                alist_l_add(all_s2s_wins, tmp);
            }
            free(flag1);
            free(flag2);
        }
    }else{
        for(i=0;i<data1->l_chr;i++){
            Pop1 *pp1=data1->pops[i];
            Pop1 *pp2=get_pop1_by_chr(data1->chrs[i], data2);
            if(pp1==NULL || pp2==NULL) continue;
            int *flag1=my_new(1000000000, sizeof(int));
            int *flag2=my_new(1000000000, sizeof(int));
            for(j=0;j<pp1->l_data;j++) flag1[pp1->pos[j]]=pp1->data[j];
            for(j=0;j<pp2->l_data;j++) flag2[pp2->pos[j]]=pp2->data[j];
            //--
            int min_p1=pp1->pos[0];
            int min_p2=pp2->pos[0];
            int min_p=min_p1<min_p2 ? min_p1:min_p2;
            int max_p1=pp1->pos[pp1->l_data-1];
            int max_p2=pp2->pos[pp2->l_data-1];
            int max_p=max_p1>max_p2 ? max_p1:max_p2;
            int start, end;
            for(start=min_p;start<=max_p;start+=step_size){
                end=start+win_size-1;
                if(end>max_p) end=max_p;
                alist_l_add(list, get_S2(flag1, flag2, start, end, data1->n_sample+1, data2->n_sample+1));
                Win1 *tmp=my_new(1, sizeof(Win1));
                tmp->chr= str_copy(data1->chrs[i]);
                tmp->start=start;
                tmp->end=end;
                tmp->length=end-start+1;
                alist_l_add(all_s2s_wins, tmp);
                if(end==max_p) break;
            }
            //--
            free(flag1);
            free(flag2);
        }
    }

    //-- for global
    all_s2s_wins_global=new_alist_l(16);
    all_s2s_global=new_alist_l(16);
    memset(global_s2->arr, 0, global_s2->dim1*global_s2->dim2*sizeof(double));
    if(win_size_file_for_global){
        for(i=0;i<all_wins_global->l_chr;i++){
            Win2 *win2=all_wins_global->data[i];
            Pop1 *pp1=get_pop1_by_chr(all_wins_global->chrs[i], data1);
            Pop1 *pp2=get_pop1_by_chr(all_wins_global->chrs[i], data2);
            if(pp1==NULL || pp2==NULL) continue;
            int *flag1=my_new(1000000000, sizeof(int));
            int *flag2=my_new(1000000000, sizeof(int));
            for(j=0;j<pp1->l_data;j++) flag1[pp1->pos[j]]=pp1->data[j];
            for(j=0;j<pp2->l_data;j++) flag2[pp2->pos[j]]=pp2->data[j];
            for(j=0;j<win2->l_win;j++){
                Win1 *win1=win2->wins[j];
                alist_l_add(all_s2s_global, get_S2(flag1, flag2, win1->start, win1->end, data1->n_sample+1, data2->n_sample+1));
                Win1 *tmp= my_new(1, sizeof(Win1));
                tmp->chr=all_wins_global->chrs[i];
                tmp->start=win1->start;
                tmp->end=win1->end;
                tmp->length=win1->length;
                alist_l_add(all_s2s_wins_global, tmp);
            }
            free(flag1);
            free(flag2);
        }
    }else{
        for(i=0;i<data1->l_chr;i++){
            Pop1 *pp1=data1->pops[i];
            Pop1 *pp2=get_pop1_by_chr(data1->chrs[i], data2);
            if(pp1==NULL || pp2==NULL) continue;
            int *flag1=my_new(1000000000, sizeof(int));
            int *flag2=my_new(1000000000, sizeof(int));
            for(j=0;j<pp1->l_data;j++) flag1[pp1->pos[j]]=pp1->data[j];
            for(j=0;j<pp2->l_data;j++) flag2[pp2->pos[j]]=pp2->data[j];
            //--
            int min_p1=pp1->pos[0];
            int min_p2=pp2->pos[0];
            int min_p=min_p1<min_p2 ? min_p1:min_p2;
            int max_p1=pp1->pos[pp1->l_data-1];
            int max_p2=pp2->pos[pp2->l_data-1];
            int max_p=max_p1>max_p2 ? max_p1:max_p2;
            int start=min_p;
            int end=max_p;
            //--
            alist_l_add(all_s2s_global, get_S2(flag1, flag2, start, end, data1->n_sample+1, data2->n_sample+1));
            Win1 *tmp=my_new(1, sizeof(Win1));
            tmp->chr= str_copy(data1->chrs[i]);
            tmp->start=start;
            tmp->end=end;
            tmp->length=end-start+1;
            alist_l_add(all_s2s_wins_global, tmp);
            //--
            free(flag1);
            free(flag2);
        }
    }

    complete_S2(global_s2);
    if(is_print_s2){
        fprintf(stderr, "global S2:\n");
        print_S2_1(global_s2);
    }

    free_Pop2(data1);
    free_Pop2(data2);

    mylog("read finished!");

    return list;
}

void free_Pop1(Pop1 *p){
    if(!p) return;
    if(p->pos) free(p->pos);
    if(p->data) free(p->data);
    free(p);
}

void free_Pop2(Pop2 *p){
    if(!p) return;
    int i;
    for(i=0;i<p->l_chr;i++){
        free(p->chrs[i]);
        free_Pop1(p->pops[i]);
    }
    free(p->chrs);
    free(p->pops);
    free(p);
}

Pop1 *get_pop1_by_chr(char *chr, Pop2 *data){
    int i;
    for(i=0;i<data->l_chr;i++){
        if(strcmp(chr, data->chrs[i])==0) return data->pops[i];
    }
    return NULL;
}

S2 *get_S2(int *flag1, int *flag2, int start, int end, int dim1, int dim2){
    S2 *s2=my_new(1, sizeof(S2));
    s2->dim1=dim1;
    s2->dim2=dim2;
    s2->arr=my_new(dim1*dim2, sizeof(double));

    int pos;
    for(pos=start;pos<=end;pos++){
        int n1=flag1[pos];
        int n2=flag2[pos];
        if(n1>=dim1 || n2>=dim2){
            fprintf(stderr, "error: n1=%d/dim1=%d\tn2=%d/dim2=%d\n", n1, dim1, n2, dim2);
            exit(0);
        }
        s2->arr[n1*dim2+n2]++;
        global_s2->arr[n1*dim2+n2]++;
    }

    complete_S2(s2);
    free(s2->arr);
    s2->arr=NULL;

    return s2;
}

void complete_S2(S2 *s2){
    if(s2->arr==NULL) return;

    int i, j, dim1=s2->dim1, dim2=s2->dim2;

    s2->s1=0;
    s2->s2=0;
    s2->s12=0;
    s2->D=s2->arr[(dim1-1)*dim2]+s2->arr[dim2-1];

    for(i=0;i<dim1;i++){
        for(j=0;j<dim2;j++){
            double v=s2->arr[i*dim2+j];
            if(i>0 && i<(dim1-1) && (j==0 || j==(dim2-1))) s2->s1+=v;
            else if((i==0 || i==(dim1-1)) && j>0 && j<(dim2-1)) s2->s2+=v;
            else if(i>0 && i<(dim1-1) && j>0 && j<(dim2-1)) s2->s12+=v;
        }
    }
}

Pop2 *read_hap_file(char *hap_file, char *pos_file){
    sprintf(loginfo, "reading %s...", hap_file);mylog(loginfo);

    int i, j, len;

    int row=0, col=0;
    SBuilder *data=new_s_builder(1<<16);

    int *tabs=my_new(1000000000, sizeof(int));
    GzStream *gz1=gz_stream_open(hap_file, "r");
    while(gz_read_util(gz1, '\n', line, maxline, &len)){
        len=chmop_with_len(line, len);
        if(len==0) continue;
        row++;
        int num=str_tab_index(line, ' ', tabs);
        if(col==0) col=num;
        else if(col!=num){
            fprintf(stderr, "error format: line's length not equal(%s)\n", hap_file);
            exit(0);
        }
        for(i=0;i<num;i++){
            j=i==0 ? 0:tabs[i-1]+1;
            s_builder_add_char(data, line[j]);
        }
    }
    gz_stream_destory(gz1);

    Pop2 *pop2=my_new(1, sizeof(Pop2));
    pop2->n_sample=row;

    char *chr=NULL;
    Pop1 *pop1=NULL;
    j=0;

    gz1=gz_stream_open(pos_file, "r");
    while(gz_read_util(gz1, '\n', line, maxline, &len)){
        len=chmop_with_len(line, len);
        if(len==0) continue;
        //--
        char *arr=data->str;
        int num=0, missing_num=0;
        for(i=0;i<row;i++){
            char c=arr[i*col+j];
            if(c!='0' && c!='1') missing_num++;
            else if(c=='1') num++;
        }
        if(num>0 && (num+missing_num)==row) num=row;
        //--
        j++;
        if(j>col){
            fprintf(stderr, "the row number of(%s) is not equal to the column number of(%s)!\n", pos_file, hap_file);
            exit(0);
        }
        //--
        if(str_tab_index(line, '\t', tabs)!=2){
            fprintf(stderr, "error format(%s)\n", pos_file);
            exit(0);
        }
        i=0;
        while(line[++i]!='\t');
        line[i]='\0';
        int pos=atoi(line+i+1);
        //--
        if(chr==NULL || strcmp(chr, line)!=0){
            chr=str_copy_with_len(line, i);
            pop1=my_new(1, sizeof(Pop1));
            //--
            pop2->chrs=my_renew(pop2->chrs, (pop2->l_chr+1)*sizeof(char *));
            pop2->pops=my_renew(pop2->pops, (pop2->l_chr+1)*sizeof(Pop1 *));
            pop2->chrs[pop2->l_chr]=chr;
            pop2->pops[pop2->l_chr++]=pop1;
        }
        //--
        if(pop1->l_data==pop1->t_data){
            pop1->t_data+=1000;
            pop1->data=my_renew(pop1->data, pop1->t_data*sizeof(int));
            pop1->pos=my_renew(pop1->pos, pop1->t_data*sizeof(int));
        }
        pop1->data[pop1->l_data]=num;
        pop1->pos[pop1->l_data++]=pos;
    }
    gz_stream_destory(gz1);
    free_s_builder(data);

    free(tabs);

    if(j!=col){
        fprintf(stderr, "the row number of(%s) is not equal to the column number of(%s)!\n", pos_file, hap_file);
        exit(0);
    }

    return pop2;
}

Pop2 *read_tped_file(char *tped_file){
    sprintf(loginfo, "reading %s...", tped_file);mylog(loginfo);

    int i, len;
    int tabs[10000];

    Pop2 *pop2=my_new(1, sizeof(Pop2));

    char *chr=NULL;
    Pop1 *pop1=NULL;

    GzStream *gz1=gz_stream_open(tped_file, "r");
    while(gz_read_util(gz1, '\n', line, maxline, &len)){
        len=chmop_with_len(line, len);
        if(len==0) continue;
        if(line[0]=='#') continue;
        //--
        int n_tabs=str_tab_index(line, '\t', tabs);
        for(i=0;i<n_tabs;i++) line[tabs[i]]='\0';
        int pos=atoi(line+tabs[2]+1);
        if(pop2->n_sample==0) pop2->n_sample=n_tabs-4;
        int num=0, missing_num=0;
        for(i=4;i<n_tabs;i++){
            char *str=line+tabs[i-1]+1;
            if(str[0]!='0' && str[0]!='1') missing_num++;
            else if(str[0]=='1') num++;
        }
        if(num>0 && (num+missing_num)==(n_tabs-4)) num=n_tabs-4;
        //--
        if(chr==NULL || strcmp(chr, line)!=0){
            chr=str_copy_with_len(line, i);
            pop1=my_new(1, sizeof(Pop1));
            //--
            pop2->chrs=my_renew(pop2->chrs, (pop2->l_chr+1)*sizeof(char *));
            pop2->pops=my_renew(pop2->pops, (pop2->l_chr+1)*sizeof(Pop1 *));
            pop2->chrs[pop2->l_chr]=chr;
            pop2->pops[pop2->l_chr++]=pop1;
        }
        //--
        if(pop1->l_data==pop1->t_data){
            pop1->t_data+=1000;
            pop1->data=my_renew(pop1->data, pop1->t_data*sizeof(int));
            pop1->pos=my_renew(pop1->pos, pop1->t_data*sizeof(int));
        }
        pop1->data[pop1->l_data]=num;
        pop1->pos[pop1->l_data++]=pos;
    }
    gz_stream_destory(gz1);

    return pop2;
}

Pop2 *read_vcf_file(char *vcf_file){
    sprintf(loginfo, "reading %s...", vcf_file);mylog(loginfo);

    int i, len;
    int tabs[10000];

    Pop2 *pop2=my_new(1, sizeof(Pop2));

    char *chr=NULL;
    Pop1 *pop1=NULL;

    GzStream *gz1=gz_stream_open(vcf_file, "r");
    while(gz_read_util(gz1, '\n', line, maxline, &len)){
        len=chmop_with_len(line, len);
        if(len==0) continue;
        if(line[0]=='#') continue;
        //--
        int n_tabs=str_tab_index(line, '\t', tabs);
        for(i=0;i<n_tabs;i++) line[tabs[i]]='\0';
        int pos=atoi(line+tabs[0]+1);
        if(pop2->n_sample==0) pop2->n_sample=(n_tabs-9)*2;
        int num=0, missing_num=0;
        for(i=9;i<n_tabs;i++){
            char *str=line+tabs[i-1]+1;
            if(str[0]=='.') missing_num++;
            else if(str[0]!='0') num++;
            if(str[2]=='.') missing_num++;
            else if(str[2]!='0') num++;
        }
        if(num>0 && (num+missing_num)==(2*(n_tabs-9))) num=2*(n_tabs-9);
        //--
        if(chr==NULL || strcmp(chr, line)!=0){
            chr=str_copy_with_len(line, i);
            pop1=my_new(1, sizeof(Pop1));
            //--
            pop2->chrs=my_renew(pop2->chrs, (pop2->l_chr+1)*sizeof(char *));
            pop2->pops=my_renew(pop2->pops, (pop2->l_chr+1)*sizeof(Pop1 *));
            pop2->chrs[pop2->l_chr]=chr;
            pop2->pops[pop2->l_chr++]=pop1;
        }
        //--
        if(pop1->l_data==pop1->t_data){
            pop1->t_data+=1000;
            pop1->data=my_renew(pop1->data, pop1->t_data*sizeof(int));
            pop1->pos=my_renew(pop1->pos, pop1->t_data*sizeof(int));
        }
        pop1->data[pop1->l_data]=num;
        pop1->pos[pop1->l_data++]=pos;
    }
    gz_stream_destory(gz1);

    return pop2;
}

S2 *read_afs_file(){
    sprintf(loginfo, "reading afs file(%s)...", afs_file);mylog(loginfo);

    int tabs[100000], i, j, len;

    int row=0, col=0;
    AList_d *list=new_alist_d(1000000);

    GzStream *gz1=gz_stream_open(afs_file, "r");
    while(gz_read_util(gz1, '\n', line, maxline, &len)){
        len=chmop_with_len(line, len);
        if(len==0) continue;
        if(line[0]=='#') continue;
        //--
        row++;
        int num=str_tab_index(line, '\t', tabs);
        if(col==0) col=num;
        else if(col!=num){
            fprintf(stderr, "error line's elements number: pre_num=%d\tnum=%d\n", col, num);
            exit(0);
        }
        for(i=0;i<num;i++) line[tabs[i]]='\0';
        alist_d_add(list, atof(line));
        for(i=0;i<num-1;i++) alist_d_add(list, atof(line+tabs[i]+1));
    }
    gz_stream_destory(gz1);

    S2 *s2=my_new(1, sizeof(S2));
    s2->dim1=row;
    s2->dim2=col;
    s2->arr=list->elementData;
    complete_S2(s2);

    free(list);

    return s2;
}

//==============================

void grid_paras(int len, double *x, double *low, double *up, int max, double (*func)(double*,void *), void *data){
    int i, j, k, n=(int)exp(log(max)/len);
    if(n<3) n=3;
    int half=(n-1)/2;

    AList_d **all_paras=my_new(len, sizeof(AList_d *));
    for(i=0;i<len;i++){
        AList_d *list=new_alist_d(n);
        alist_d_add(list, x[i]);
        for(k=1;k<=half;k++){
            double v=x[i]*pow(0.5, k);
            if(v<low[i]) break;
            alist_d_add(list, v);
        }
        for(k=1;k<=half;k++){
            double v=x[i]*pow(2.0, k);
            if(v>up[i]) break;
            alist_d_add(list, v);
        }
        all_paras[i]=list;
    }

    AList_l *list=new_alist_l(max);
    double *stack=my_new(len, sizeof(double));
    grid_paras_perm(len, 0, all_paras, stack, list);
    for(i=0;i<len;i++) free_alist_d(all_paras[i]);
    free(all_paras);
    free(stack);

    double min_value=func(x, NULL);
    for(i=0;i<list->size;i++){
        double *tmp=list->elementData[i];
        double v=func(tmp, data);
        if(v<min_value){
            min_value=v;
            memcpy(x, tmp, len*sizeof(double));
        }
        free(tmp);
    }
    free_alist_l(list);
}

void grid_paras_perm(int len, int index, AList_d **all_paras, double *stack, AList_l *list){
    if(index==len){
        double *tmp=my_new(len, sizeof(double));
        memcpy(tmp, stack, len*sizeof(double));
        alist_l_add(list, tmp);
        return;
    }
    AList_d *tmp=all_paras[index];
    int i;
    for(i=0;i<tmp->size;i++){
        stack[index]=tmp->elementData[i];
        grid_paras_perm(len, index+1, all_paras, stack, list);
    }
}

//==============================

int is_print=0;

void estimate_exp_jafs(S2 *exp_s2, double *arr, int row, int col, double N0, double N1, double N2, int n1, int n2, double T, double mu, double lambda2, double L, double (*growth_model)(double *, double), double *paras){
    int i;

    mu*=L;
    s2_exp_s1_s2_s12_D(N0, N1, N2, n1, n2, (int)T, mu, growth_model, paras, &(exp_s2->s1), &(exp_s2->s2), &(exp_s2->s12), &(exp_s2->D));
    //fprintf(stderr, "s1=%f\ts2=%f\ts12=%f\tD=%f\n", exp_s2->s1, exp_s2->s2, exp_s2->s12, exp_s2->D);
    //--
    double v1=0.0, v2=0.0;
    for(i=2;i<=n1;i++) v1+=1.0/(i-1);
    for(i=2;i<=n2;i++) v2+=1.0/(i-1);
    v1*=2.0*N1*mu;
    v2*=2.0*N2*mu;
    exp_s2->s12_1=exp_s2->s12;
    exp_s2->s12_2=(v1*v2)/L;
    exp_s2->s12+=exp_s2->s12_2;
}

double log_likelihood_afs_func(double *x, void *data){// N0,N1,N2,T
    S2 *tmp=(S2 *)data;
    int dim1=tmp->dim1;
    int dim2=tmp->dim2;
    double *arr=my_new(dim1*dim2, sizeof(double));
    S2 *s2=my_new(1, sizeof(S2));
    s2->dim1=dim1;
    s2->dim2=dim2;
    s2->arr=arr;
    estimate_exp_jafs(s2, arr, dim1, dim2, x[0], x[1], x[2], dim1-1, dim2-1, x[3], func_mu, 1.0, global_L, NULL, NULL);

    double value=0.0;
    value+=get_log_poisson_probability((int)tmp->s1, s2->s1);
    value+=get_log_poisson_probability((int)tmp->s2, s2->s2);
    value+=get_log_poisson_probability((int)tmp->s12, s2->s12);
    value+=get_log_poisson_probability((int)tmp->D, s2->D);

    free(arr);
    free(s2);

    //fprintf(stdout, "func=%f\tN0=%f\tN1=%f\tN2=%f\tT=%f\n", -value, x[0], x[1], x[2], x[3]);
    //fflush(stdout);

    return -value;
}

double log_likelihood_func(double *x, void *data){// N0,N1,N2,T
    int dim1=func_n1+1;
    int dim2=func_n2+1;
    double *arr=my_new(dim1*dim2, sizeof(double));
    S2 *s2=my_new(1, sizeof(S2));
    s2->dim1=dim1;
    s2->dim2=dim2;
    s2->arr=arr;

    int i;
    double value=0.0;

    if(0){
        estimate_exp_jafs(s2, arr, dim1, dim2, x[0], x[1], x[2], func_n1, func_n2, x[3], func_mu, 1.0, win_size, NULL, NULL);
        for(i=0;i<all_s2s_global->size;i++){
            S2 *tmp=all_s2s_global->elementData[i];
            Win1 *win=all_s2s_wins_global->elementData[i];
            int length=win->length==0 ? (win->end-win->start+1):win->length;
            double rate=(double)length/(double)win_size;
            value+=get_log_poisson_probability((int)tmp->s1, s2->s1*rate);
            value+=get_log_poisson_probability((int)tmp->s2, s2->s2*rate);
            value+=get_log_poisson_probability((int)tmp->s12, s2->s12*rate);
            value+=get_log_poisson_probability((int)tmp->D, s2->D*rate);
        }
    }else{
        estimate_exp_jafs(s2, arr, dim1, dim2, x[0], x[1], x[2], func_n1, func_n2, x[3], func_mu, 1.0, global_L, NULL, NULL);
        value+=get_log_poisson_probability((int)global_s2->s1, s2->s1);
        value+=get_log_poisson_probability((int)global_s2->s2, s2->s2);
        value+=get_log_poisson_probability((int)global_s2->s12, s2->s12);
        value+=get_log_poisson_probability((int)global_s2->D, s2->D);
    }

    global_s2->estimate_s1=s2->s1;
    global_s2->estimate_s2=s2->s2;
    global_s2->estimate_s12=s2->s12;
    global_s2->estimate_D=s2->D;
    global_s2->D1=s2->D1;
    global_s2->D2=s2->D2;
    global_s2->s12_1=s2->s12_1;
    global_s2->s12_2=s2->s12_2;

    free(arr);
    free(s2);

    //fprintf(stdout, "func=%f\tN0=%f\tN1=%f\tN2=%f\tT=%f\n", -value, x[0], x[1], x[2], x[3]);

    return -value;
}

double log_likelihood_mu_func(double *x, void *data){//mu
    S2 *s2=all_s2s->elementData[(int)data];
    Win1 *win=all_s2s_wins->elementData[(int)data];
    int length=win->length==0 ? (win->end-win->start+1):win->length;

    S2 *tmp=my_new(1, sizeof(S2));
    tmp->dim1=s2->dim1;
    tmp->dim2=s2->dim2;
    tmp->arr=my_new(s2->dim1*s2->dim2, sizeof(double));
    estimate_exp_jafs(tmp, tmp->arr, tmp->dim1, tmp->dim2, global_N0, global_N1, global_N2, func_n1, func_n2, global_T, func_mu*x[0], 1.0, (double)length, NULL, NULL);

    double value=0.0;
    value+=get_log_poisson_probability((int)s2->s1, tmp->s1);
    value+=get_log_poisson_probability((int)s2->s2, tmp->s2);
    value+=get_log_poisson_probability((int)s2->s12, tmp->s12);
    value+=get_log_poisson_probability((int)s2->D, tmp->D);
    value=-value;

    s2->estimate_s1=tmp->s1;
    s2->estimate_s2=tmp->s2;
    s2->estimate_s12=tmp->s12;
    s2->estimate_D=tmp->D;
    s2->D1=tmp->D1;
    s2->D2=tmp->D2;
    s2->s12_1=tmp->s12_1;
    s2->s12_2=tmp->s12_2;

    free(tmp->arr);
    free(tmp);

    return value;
}

double log_likelihood_mu_lambda_func(double *x, void *data){//mu lambda1 lambda2
    S2 *s2=all_s2s->elementData[(int)data];
    Win1 *win=all_s2s_wins->elementData[(int)data];
    int length=win->length==0 ? (win->end-win->start+1):win->length;

    S2 *tmp=my_new(1, sizeof(S2));
    tmp->dim1=s2->dim1;
    tmp->dim2=s2->dim2;
    tmp->arr=my_new(s2->dim1*s2->dim2, sizeof(double));
    estimate_exp_jafs(tmp, tmp->arr, tmp->dim1, tmp->dim2, global_N0, global_N1*x[1], global_N2*x[2], func_n1, func_n2, global_T, func_mu*x[0], x[2], (double)length, NULL, NULL);

    double value=0.0;
    value+=get_log_poisson_probability((int)s2->s1, tmp->s1);
    value+=get_log_poisson_probability((int)s2->s2, tmp->s2);
    value+=get_log_poisson_probability((int)s2->s12, tmp->s12);
    value+=get_log_poisson_probability((int)s2->D, tmp->D);
    value=-value;

    s2->estimate_s1=tmp->s1;
    s2->estimate_s2=tmp->s2;
    s2->estimate_s12=tmp->s12;
    s2->estimate_D=tmp->D;
    s2->D1=tmp->D1;
    s2->D2=tmp->D2;
    s2->s12_1=tmp->s12_1;
    s2->s12_2=tmp->s12_2;

    free(tmp->arr);
    free(tmp);

    return value;
}

double log_likelihood_mu_lambda1_func(double *x, void *data){//mu lambda1
    S2 *s2=all_s2s->elementData[(int)data];
    Win1 *win=all_s2s_wins->elementData[(int)data];
    int length=win->length==0 ? (win->end-win->start+1):win->length;

    S2 *tmp=my_new(1, sizeof(S2));
    tmp->dim1=s2->dim1;
    tmp->dim2=s2->dim2;
    tmp->arr=my_new(s2->dim1*s2->dim2, sizeof(double));
    estimate_exp_jafs(tmp, tmp->arr, tmp->dim1, tmp->dim2, global_N0, global_N1*x[1], global_N2*1.0, func_n1, func_n2, global_T, func_mu*x[0], 1.0, (double)length, NULL, NULL);

    double value=0.0;
    value+=get_log_poisson_probability((int)s2->s1, tmp->s1);
    value+=get_log_poisson_probability((int)s2->s2, tmp->s2);
    value+=get_log_poisson_probability((int)s2->s12, tmp->s12);
    value+=get_log_poisson_probability((int)s2->D, tmp->D);
    value=-value;

    s2->estimate_s1=tmp->s1;
    s2->estimate_s2=tmp->s2;
    s2->estimate_s12=tmp->s12;
    s2->estimate_D=tmp->D;
    s2->D1=tmp->D1;
    s2->D2=tmp->D2;
    s2->s12_1=tmp->s12_1;
    s2->s12_2=tmp->s12_2;

    free(tmp->arr);
    free(tmp);

    return value;
}

double log_likelihood_mu_lambda2_func(double *x, void *data){//mu lambda2
    S2 *s2=all_s2s->elementData[(int)data];
    Win1 *win=all_s2s_wins->elementData[(int)data];
    int length=win->length==0 ? (win->end-win->start+1):win->length;

    S2 *tmp=my_new(1, sizeof(S2));
    tmp->dim1=s2->dim1;
    tmp->dim2=s2->dim2;
    tmp->arr=my_new(s2->dim1*s2->dim2, sizeof(double));
    estimate_exp_jafs(tmp, tmp->arr, tmp->dim1, tmp->dim2, global_N0, global_N1*1.0, global_N2*x[1], func_n1, func_n2, global_T, func_mu*x[0], x[1], (double)length, NULL, NULL);

    double value=0.0;
    value+=get_log_poisson_probability((int)s2->s1, tmp->s1);
    value+=get_log_poisson_probability((int)s2->s2, tmp->s2);
    value+=get_log_poisson_probability((int)s2->s12, tmp->s12);
    value+=get_log_poisson_probability((int)s2->D, tmp->D);
    value=-value;

    s2->estimate_s1=tmp->s1;
    s2->estimate_s2=tmp->s2;
    s2->estimate_s12=tmp->s12;
    s2->estimate_D=tmp->D;
    s2->D1=tmp->D1;
    s2->D2=tmp->D2;
    s2->s12_1=tmp->s12_1;
    s2->s12_2=tmp->s12_2;

    free(tmp->arr);
    free(tmp);

    return value;
}

void optimize_mu_for_win(int index){
    int npar=1;
    double *x=my_new(npar, sizeof(double));
    double *low=my_new(npar, sizeof(double));
    double *up=my_new(npar, sizeof(double));
    int *nbd=my_new(npar, sizeof(int));
    x[0]=1.0;
    low[0]=0.1;
    up[0]=max_mu;
    nbd[0]=2;

    if(optimize_type==0) findmax_bfgs(npar, x, index, log_likelihood_mu_func, NULL, low, up, nbd, 0);
    else if(optimize_type==1) kmin_hj2(npar, x, index, log_likelihood_mu_func, low, up);

    res_1_mu[index]=x[0];
    res_1_like[index]=log_likelihood_mu_func(x, index);

    free(x);
    free(low);
    free(up);
    free(nbd);

    Win1 *win=all_s2s_wins->elementData[index];
    int length=win->length==0 ? (win->end-win->start+1):win->length;
    fprintf(stdout, "estimate_mu: %s-%d-%d\tmu=%f\tlike=%f\n", win->chr, win->start, win->end, res_1_mu[index]*func_mu*length, res_1_like[index]);
}

void optimize_mu_lambda_for_win(int index){
    int npar=3;
    double *x=my_new(npar, sizeof(double));
    double *low=my_new(npar, sizeof(double));
    double *up=my_new(npar, sizeof(double));
    int *nbd=my_new(npar, sizeof(int));
    x[0]=1.0;
    low[0]=0.1;
    up[0]=max_mu;
    nbd[0]=2;
    x[1]=1.0;
    low[1]=min_lambda;
    up[1]=max_lambda;
    nbd[1]=2;
    x[2]=1.0;
    low[2]=min_lambda;
    up[2]=max_lambda;
    nbd[2]=2;

    if(optimize_type==0) findmax_bfgs(npar, x, index, log_likelihood_mu_lambda_func, NULL, low, up, nbd, 0);
    else if(optimize_type==1) kmin_hj2(npar, x, index, log_likelihood_mu_lambda_func, low, up);

    res_2_mu[index]=x[0];
    res_2_lambda1[index]=x[1];
    res_2_lambda2[index]=x[2];
    res_2_like[index]=log_likelihood_mu_lambda_func(x, index);

    free(x);
    free(low);
    free(up);
    free(nbd);

    Win1 *win=all_s2s_wins->elementData[index];
    int length=win->length==0 ? (win->end-win->start+1):win->length;
    fprintf(stdout, "estimate_mu_lambda: %s-%d-%d\tmu=%f\tlambda1=%f\tlambda2=%f\tlike=%f\n", win->chr, win->start, win->end, res_2_mu[index]*func_mu*length, res_2_lambda1[index], res_2_lambda2[index], res_2_like[index]);
}

void optimize_mu_lambda1_for_win(int index){
    int npar=2;
    double *x=my_new(npar, sizeof(double));
    double *low=my_new(npar, sizeof(double));
    double *up=my_new(npar, sizeof(double));
    int *nbd=my_new(npar, sizeof(int));
    x[0]=1.0;
    low[0]=0.1;
    up[0]=max_mu;
    nbd[0]=2;
    x[1]=1.0;
    low[1]=min_lambda;
    up[1]=max_lambda;
    nbd[1]=2;

    if(optimize_type==0) findmax_bfgs(npar, x, index, log_likelihood_mu_lambda1_func, NULL, low, up, nbd, 0);
    else if(optimize_type==1) kmin_hj2(npar, x, index, log_likelihood_mu_lambda1_func, low, up);

    res_2_lambda1[index]=x[1];
    res_2_like_mu_lambda1[index]=log_likelihood_mu_lambda1_func(x, index);

    Win1 *win=all_s2s_wins->elementData[index];
    int length=win->length==0 ? (win->end-win->start+1):win->length;
    fprintf(stdout, "estimate_mu_lambda1: %s-%d-%d\tmu=%f\tlambda1=%f\tlike=%f\n", win->chr, win->start, win->end, x[0]*func_mu*length, x[1], res_2_like_mu_lambda1[index]);

    free(x);
    free(low);
    free(up);
    free(nbd);
}

void optimize_mu_lambda2_for_win(int index){
    int npar=2;
    double *x=my_new(npar, sizeof(double));
    double *low=my_new(npar, sizeof(double));
    double *up=my_new(npar, sizeof(double));
    int *nbd=my_new(npar, sizeof(int));
    x[0]=1.0;
    low[0]=0.1;
    up[0]=max_mu;
    nbd[0]=2;
    x[1]=1.0;
    low[1]=min_lambda;
    up[1]=max_lambda;
    nbd[1]=2;

    if(optimize_type==0) findmax_bfgs(npar, x, index, log_likelihood_mu_lambda2_func, NULL, low, up, nbd, 0);
    else if(optimize_type==1) kmin_hj2(npar, x, index, log_likelihood_mu_lambda2_func, low, up);

    res_2_lambda2[index]=x[1];
    res_2_like_mu_lambda2[index]=log_likelihood_mu_lambda2_func(x, index);

    Win1 *win=all_s2s_wins->elementData[index];
    int length=win->length==0 ? (win->end-win->start+1):win->length;
    fprintf(stdout, "estimate_mu_lambda2: %s-%d-%d\tmu=%f\tlambda2=%f\tlike=%f\n", win->chr, win->start, win->end, x[0]*func_mu*length, x[1], res_2_like_mu_lambda2[index]);

    free(x);
    free(low);
    free(up);
    free(nbd);
}

inline double get_log_poisson_probability(int k, double lambda){
    return lambda<MIN_POISSON_LAMBDA ? (k==0 ? (0):(log(MIN_POISSON_PROP))):(k*log(lambda)-log_fac[k]-lambda);
}

//==============================

void get_nlambda_and_P(AList_d *L, AList_d *nlambda, AList_d *p){
    int i;
    double par=0.0, avg, var;

    AList_d *bL= box_cox(L, &par);
    avg_unbiased_var(bL->size, bL->elementData, &avg, &var);
    double L_min=avg-3.0*var;
    double L_max=avg+3.0*var;

    AList_d *L_filter= new_alist_d(L->size);
    for(i=0;i<L->size;i++){
        double d1=L->elementData[i];
        double d2=bL->elementData[i];
        if(d2>=L_min && d2<=L_max) alist_d_add(L_filter, d1);
    }

    AList_d *bL_filter= box_cox(L_filter, &par);
    AList_d *L_par= bcTransform(par, L);
    avg_unbiased_var(bL_filter->size, bL_filter->elementData, &avg, &var);
    free_alist_d(L_filter);
    free_alist_d(bL_filter);

    for(i=0;i<L_par->size;i++){
        double d1=L_par->elementData[i];
        double d2=(d1-avg)/var;
        double d3= norm_cdf(d2, 0.0, 1.0);
        alist_d_add(nlambda, d2);
        alist_d_add(p, d3);
    }
    free_alist_d(L_par);
    free_alist_d(bL);
}

AList_d *bcTransform(double par, AList_d *L){
    AList_d *res= new_alist_d(L->size);

    int i;

    double tmp=par>=0.0 ? par:-par;
    if(tmp<1e-10){
        for(i=0;i<L->size;i++){
            alist_d_add(res, log(L->elementData[i]));
        }
    }else{
        for(i=0;i<L->size;i++){
            alist_d_add(res, (pow(L->elementData[i], par)-1.0)/par);
        }
    }

    return res;
}

AList_d *box_cox(AList_d *L, double *res_par){
    res_par[0]=0.0;
    kmin_hj2(1, res_par, L, func_for_optimize_box_cox, NULL, NULL);
    return bcTransform(res_par[0], L);
}

double func_for_optimize_box_cox(double *x, void *data){
    double par=x[0];
    AList_d *L=(AList_d *)data;

    int i;

    double total_log_L=0.0;
    for(i=0;i<L->size;i++) total_log_L+=log(L->elementData[i]);

    double avg=0.0, var=0.0;
    AList_d *Lhat=bcTransform(par, L);
    avg_biased_var(Lhat->size, Lhat->elementData, &avg, &var);
    free_alist_d(Lhat);

    double llf=-(L->size/2.0)*log(var*var)+(par-1.0)*total_log_L;

    return -llf;
}

//==============================

void print_S2_1(S2 *s2){
    fprintf(stderr, "S2:\ts1=%d\ts2=%d\ts12=%d\tD=%d\n", (int)s2->s1, (int)s2->s2, (int)s2->s12, (int)s2->D);
    int i, j;
    for(i=0;i<s2->dim1;i++){
        for(j=0;j<s2->dim2;j++) fprintf(stderr, "%d ", (int)s2->arr[i*s2->dim2+j]);
        fprintf(stderr, "\n");
    }
}

void print_S2(S2 *s2){
    fprintf(stderr, "S2:\ts1=%f\ts2=%f\ts12=%f\tD=%f\n", s2->s1, s2->s2, s2->s12, s2->D);
    int i, j;
    for(i=0;i<s2->dim1;i++){
        for(j=0;j<s2->dim2;j++) fprintf(stderr, "%.2f ", s2->arr[i*s2->dim2+j]);
        fprintf(stderr, "\n");
    }
}

void write_pop2(Pop2 *p2, char *file){
    int i, j, len;

    GzStream *out=gz_stream_open(file, "w");

    len=sprintf(loginfo, "n=%d\n", p2->n_sample);
    gz_write(out, loginfo, len);

    for(i=0;i<p2->l_chr;i++){
        char *chr=p2->chrs[i];
        Pop1 *p1=p2->pops[i];
        for(j=0;j<p1->l_data;j++){
            len=sprintf(loginfo, "%s\t%d\t%d\n", chr, p1->pos[j], p1->data[j]);
            gz_write(out, loginfo, len);
        }
    }

    gz_stream_destory(out);
}

//==============================




