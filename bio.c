#include "bio.h"

#define integral_num 10000.0

//====================================

void initial_bio(){
    int i, j;

    l_log_fac=50000001;
    log_fac=calloc(l_log_fac*2, sizeof(double));
    log_fac+=l_log_fac;
    for(i=2;i<l_log_fac;i++) log_fac[i]=log_gamm(i+1.0);
}

double log_gamm(double xx){
    double x,y,tmp, ser;
    static double cof[6]={76.18009172947146,-86.50532032941677,
		24.01409824083091,-1.231739572450155,
		0.1208650973866179e-2,-0.5395239384953e-5};
	int j;

	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
}

inline double log_per(int n, int k){   //if(k<0 || n<k) error;
    return log_fac[n]-log_fac[n-k];
}

inline double log_com(int n, int k){   //if(k<0 || n<k) error;
    return log_fac[n]-log_fac[n-k]-log_fac[k];
}

//====================================

inline double part_1(int i, int m, int n){
    return (i-m)%2==0 ? ((2.0*i-1.0)*exp(log_fac[m+i-2]-log_fac[m-1]+log_fac[n]-log_fac[n-i]-log_fac[m]-log_fac[i-m]-log_fac[n+i-1]+log_fac[n-1])) : ((-2.0*i+1.0)*exp(log_fac[m+i-2]-log_fac[m-1]+log_fac[n]-log_fac[n-i]-log_fac[m]-log_fac[i-m]-log_fac[n+i-1]+log_fac[n-1]));
}

inline double part_2(int i, int k, int n){
    return (i<1 || (n-k)<(i-1)) ? 0.0 : exp(log_fac[n-i-1]-log_fac[n-1]+log_fac[n-k]-log_fac[n-k-i+1]);
}

inline double part_3(int k1, int k2, int m1, int m2){
    return exp(log_fac[m1]-log_fac[m1-k1]-log_fac[k1]+log_fac[m2]-log_fac[m2-k2]-log_fac[k2]+log_fac[m1+m2-k1-k2]+log_fac[k1+k2]-log_fac[m1+m2]);
}

inline double part_4(int k, int i, int n, int m){
    return m>n ? 0.0:(k==0 ? (i==0 ? 1.0:0.0):(k==m ? (i==n ? 1.0:0.0):(k>i ? 0.0:(m<k ? 0.0:(n<i ? 0.0:((n-m)<(i-k) ? 0.0:((k<0||i<0) ? 0.0:((m==0||n==0) ? 0.0:exp(log_fac[n-m]-log_fac[n-m-i+k]-log_fac[i-k]+log_fac[i-1]-log_fac[k-1]+log_fac[n-i-1]-log_fac[m-k-1]-log_fac[n-1]+log_fac[m-1])))))))));
}

//====================================

double cal_integral(double start, double end, double (*growth_model)(double *, double), double *paras){
    if(growth_model==NULL) return (end-start);
    //--
    double t, sp, integral=0.0;
    sp=(end-start)/integral_num;
    for(t=start+sp/2.0;t<end;t+=sp) integral+=sp*growth_model(paras, t);
    return integral;
}

double g_t(int m, int n, double N, double T_integral){
    double value=0.0;
    int i;
    for(i=m;i<=n;i++){
        double a=part_1(i, m, n);
        double b=(-1.0*i*(i-1.0))/(2.0*N);
        b*=T_integral;
        value+=a*exp(b);
    }
    return value;
}

void E_T_not_gt(double *ET, int m, int n, double N, double mu, double T, double gt){
    memset(ET, 0, (n+1)*sizeof(double));

    int j, k, l;

    if(m==n){
        if(gt<1e-10) ET[m]=0.0;
        else ET[m]=T*gt;
    }else{
        double base=2.0*N;
        for(j=m;j<=n;j++){
            double v2=0.0;
            if(j==m){
                for(k=m+1;k<=n;k++){
                    double c=part_1(k, m, n);
                    //--
                    double d1=exp((-1.0*m*(m-1)*T)/(2.0*N));
                    double d2=exp((-1.0*k*(k-1)*T)/(2.0*N));
                    double e=((d1-d2)/((k-m)*(k+m-1)))-((T*d1)/(2.0*N));
                    //--
                    v2+=c*e;
                }
            }else if(j==n){
                for(k=m;k<=(n-1);k++){
                    double c=part_1(k, m, n);
                    //--
                    double d1=exp((-1.0*k*(k-1)*T)/(2.0*N));
                    double d2=exp((-1.0*n*(n-1)*T)/(2.0*N));
                    double e=((d1-d2)/((n-k)*(n+k-1)))-((T*d2)/(2.0*N));
                    //--
                    v2+=c*e;
                }
            }else{
                for(k=j+1;k<=n;k++){
                    double c=part_1(k, j, n);
                    double v3=0.0;
                    for(l=m;l<=j-1;l++){
                        double e=part_1(l, m, j);
                        //--
                        double d1=exp((-1.0*j*(j-1)*T)/(2.0*N));
                        double d2=exp((-1.0*k*(k-1)*T)/(2.0*N));
                        double d3=exp((-1.0*l*(l-1)*T)/(2.0*N));
                        double c1=T/(2.0*N);
                        double kj=(k-j)*(k+j-1);
                        double lj=(l-j)*(l+j-1);
                        double lk=(l-k)*(l+k-1);
                        double c2=(kj+lj)/(kj*lj);
                        double c3=lj/(kj*lk);
                        double c4=kj/(lk*lj);
                        //--
                        v3+=e*(c1*d1-c2*d1+c3*d2-c4*d3);
                    }
                    v2+=c*v3;
                }
            }
            ET[j]=v2*base;
        }
    }
}

double E_S_n(int i, int n, double N, double mu, double T, double *gt){
    int j, k, l, m;

    double base=2.0*N*mu;

    double value=0.0;
    for(m=1;m<=n;m++){
        for(j=m;j<=n;j++){
            double b=j*(j-1)*part_2(i, j, n);
            double v2=0.0;
            if(m==n){
                if(gt[m]<1e-10) v2=0.0;
                else v2=T*gt[m]/(2.0*N);
            }else if(j==m){
                for(k=m+1;k<=n;k++){
                    double c=part_1(k, m, n);
                    //--
                    double d1=exp((-1.0*m*(m-1)*T)/(2.0*N));
                    double d2=exp((-1.0*k*(k-1)*T)/(2.0*N));
                    double e=((d1-d2)/((k-m)*(k+m-1)))-((T*d1)/(2.0*N));
                    //--
                    v2+=c*e;
                }
            }else if(j==n){
                for(k=m;k<=(n-1);k++){
                    double c=part_1(k, m, n);
                    //--
                    double d1=exp((-1.0*k*(k-1)*T)/(2.0*N));
                    double d2=exp((-1.0*n*(n-1)*T)/(2.0*N));
                    double e=((d1-d2)/((n-k)*(n+k-1)))-((T*d2)/(2.0*N));
                    //--
                    v2+=c*e;
                }
            }else{
                for(k=j+1;k<=n;k++){
                    double c=part_1(k, j, n);
                    double v3=0.0;
                    for(l=m;l<=j-1;l++){
                        double e=part_1(l, m, j);
                        //--
                        double d1=exp((-1.0*j*(j-1)*T)/(2.0*N));
                        double d2=exp((-1.0*k*(k-1)*T)/(2.0*N));
                        double d3=exp((-1.0*l*(l-1)*T)/(2.0*N));
                        double c1=T/(2.0*N);
                        double kj=(k-j)*(k+j-1);
                        double lj=(l-j)*(l+j-1);
                        double lk=(l-k)*(l+k-1);
                        double c2=(kj+lj)/(kj*lj);
                        double c3=lj/(kj*lk);
                        double c4=kj/(lk*lj);
                        //--
                        v3+=e*(c1*d1-c2*d1+c3*d2-c4*d3);
                    }
                    v2+=c*v3;
                }
            }
            value+=(b*v2);
            if(i==n && m==1) return v2*base;
        }
    }

    if(value<0.0) value=0.0;
    return base*value;
}

double E_S_a(int m, int n, double N, double mu){
    int k;

    double base=2.0*N*mu;

    double v=0.0;
    for(k=2;k<=n;k++) v+=part_2(m, k, n);

    return base*v;
}

void afs_m_growth(double *arr, int n, int m, double T, double T1, double N1, double N2, double mu, double *gt1, double (*growth_model)(double *, double), double *paras){
    memset(arr, 0, (n+1)*sizeof(double));

    double T2=T-T1;
    double T2_integral=cal_integral(0, T2, growth_model, paras);

    int a, b, k, internal;

    double *sb1=calloc(n+1, sizeof(double));
    double *sb2=calloc(n+1, sizeof(double));
    double *ET1=calloc(n+1, sizeof(double));
    double *ET2=calloc(n+1, sizeof(double));

    for(internal=m;internal<=n;internal++){
        double gnm1=gt1[internal];
        double gnm2=g_t(m, internal, N2, T2_integral);
        E_T_not_gt(ET1, internal, n, N1, mu, T1, gnm1);
        E_T_not_gt(ET2, m, internal, N2, mu, T2, gnm2);
        for(b=1;b<=internal;b++){
            double value=0.0;
            for(k=m;k<=internal;k++) value+=k*(k-1.0)*part_2(b, k, internal)*ET2[k]*mu;
            sb2[b]=value;
        }
        for(b=1;b<=n;b++){
            double value=0.0;
            for(k=m;k<=n;k++) value+=k*(k-1.0)*part_2(b, k, n)*ET1[k]*mu;
            sb1[b]=value;
        }
        for(b=1;b<=n;b++){
            arr[b]+=sb1[b]*gnm2;
            for(a=1;a<=internal;a++) arr[b]+=sb2[a]*gnm1*part_4(a, b, n, internal);
        }
    }

    free(sb1);
    free(sb2);
    free(ET1);
    free(ET2);
}

void afs_growth(double *arr, int n, double T, double T1, double N1, double N2, double mu, double (*growth_model)(double *, double), double *paras){
    memset(arr, 0, (n+1)*sizeof(double));

    int i, j;

    double *sb1=calloc(n+1, sizeof(double));

    double *gt1=calloc(n+1, sizeof(double));
    for(i=1;i<=n;i++) gt1[i]=g_t(i, n, N1, cal_integral(0, T1, growth_model, paras));

    for(i=1;i<=n;i++){
        afs_m_growth(sb1, n, i, T, T1, N1, N2, mu, gt1, growth_model, paras);
        for(j=1;j<=n;j++) arr[j]+=sb1[j];
    }

    free(sb1);
    free(gt1);
}

//====================================

void s2_exp_jafs(double *arr, int row, int col, double N0, double N1, double N2, int n1, int n2, int T, double mu, double (*growth_model)(double *, double), double *paras){
    memset(arr, 0, row*col*sizeof(double));

    int i, j, k, k1, k2, m, m1, m2, n;

    double T_integral=cal_integral(0, T, growth_model, paras);

    double *gt1=calloc(n1+1, sizeof(double));
    double *gt2=calloc(n2+1, sizeof(double));
    for(m=1;m<=n1;m++) gt1[m]=g_t(m, n1, N1, T_integral);
    for(m=1;m<=n2;m++) gt2[m]=g_t(m, n2, N2, T_integral);

    int64_t l_esa=n1+n2+1;
    double *esa=calloc(l_esa*l_esa, sizeof(double));
    for(n=1;n<l_esa;n++){
        for(m=0;m<=n;m++) esa[n*l_esa+m]=E_S_a(m, n, N0, mu);
    }

    double *p_vec1=calloc(n1+1, sizeof(double));
    double *p_vec2=calloc(n2+1, sizeof(double));

    for(i=0;i<=n1;i++){
        for(j=0;j<=n2;j++){
            if(i==0 && j==0) continue;
            double value=0.0;
            for(m1=1;m1<=n1;m1++){
                for(m2=1;m2<=n2;m2++){
                    double v=0.0;
                    for(k1=0;k1<=m1&&k1<=i;k1++) p_vec1[k1]=part_4(k1, i, n1, m1);
                    for(k2=0;k2<=m2&&k2<=j;k2++) p_vec2[k2]=part_4(k2, j, n2, m2);
                    for(k1=0;k1<=m1&&k1<=i;k1++){
                        for(k2=0;k2<=m2&&k2<=j;k2++){
                            double a=part_3(k1, k2, m1, m2);
                            double b=esa[(m1+m2)*l_esa+(k1+k2)];
                            v+=(p_vec1[k1]*p_vec2[k2]*a*b);
                        }
                    }
                    value+=(gt1[m1]*gt2[m2]*v);
                }
            }
            arr[i*col+j]=value;
        }
    }

    for(i=1,j=0;i<=n1;i++) arr[i*col+j]+=E_S_n(i, n1, N1, mu, T, gt1);
    for(i=0,j=1;j<=n2;j++) arr[i*col+j]+=E_S_n(j, n2, N2, mu, T, gt2);

    free(gt1);
    free(gt2);
    free(esa);
    free(p_vec1);
    free(p_vec2);
}

void s2_exp_jafs_v2(double *arr, int row, int col, double N1, double N2, int n1, int n2, int T1, int T2, double mu, int l_esa, double *esa, double (*growth_model)(double *, double), double *paras){
    memset(arr, 0, row*col*sizeof(double));

    int i, j, k, k1, k2, m, m1, m2, n;

    double T1_integral=cal_integral(0, T1, growth_model, paras);
    double T2_integral=cal_integral(0, T2, growth_model, paras);

    double *gt1=calloc(n1+1, sizeof(double));
    double *gt2=calloc(n2+1, sizeof(double));
    for(m=1;m<=n1;m++) gt1[m]=g_t(m, n1, N1, T1_integral);
    for(m=1;m<=n2;m++) gt2[m]=g_t(m, n2, N2, T2_integral);

    double *p_vec1=calloc(n1+1, sizeof(double));
    double *p_vec2=calloc(n2+1, sizeof(double));

    for(i=0;i<=n1;i++){
        for(j=0;j<=n2;j++){
            if(i==0 && j==0) continue;
            double value=0.0;
            for(m1=1;m1<=n1;m1++){
                for(m2=1;m2<=n2;m2++){
                    double v=0.0;
                    for(k1=0;k1<=m1&&k1<=i;k1++) p_vec1[k1]=part_4(k1, i, n1, m1);
                    for(k2=0;k2<=m2&&k2<=j;k2++) p_vec2[k2]=part_4(k2, j, n2, m2);
                    for(k1=0;k1<=m1&&k1<=i;k1++){
                        for(k2=0;k2<=m2&&k2<=j;k2++){
                            double a=part_3(k1, k2, m1, m2);
                            double b=esa[(m1+m2)*l_esa+(k1+k2)];
                            v+=(p_vec1[k1]*p_vec2[k2]*a*b);
                        }
                    }
                    value+=(gt1[m1]*gt2[m2]*v);
                }
            }
            arr[i*col+j]=value;
        }
    }

    for(i=1,j=0;i<=n1;i++) arr[i*col+j]+=E_S_n(i, n1, N1, mu, T1, gt1);
    for(i=0,j=1;j<=n2;j++) arr[i*col+j]+=E_S_n(j, n2, N2, mu, T2, gt2);

    free(gt1);
    free(gt2);
    free(p_vec1);
    free(p_vec2);
}

void s2_exp_s1_s2_s12_D(double N0, double N1, double N2, int n1, int n2, int T, double mu, double (*growth_model)(double *, double), double *paras, double *res_s1, double *res_s2, double *res_s12, double *res_D){
    int i, j, k, k1, k2, m, m1, m2, n;

    double T_integral=cal_integral(0, T, growth_model, paras);

    double *gt1=calloc(n1+1, sizeof(double));
    double *gt2=calloc(n2+1, sizeof(double));
    for(m=1;m<=n1;m++) gt1[m]=g_t(m, n1, N1, T_integral);
    for(m=1;m<=n2;m++) gt2[m]=g_t(m, n2, N2, T_integral);

    double s1=0.0, s2=0.0, s12=0.0, D=0.0;
    int flag1=n1<50 ? 1:0;
    int flag2=n2<50 ? 1:0;

    //-- s1
    for(i=1;i<=n1;i++){
        for(j=1;j<=n2;j++){
            double v=0.0;
            for(k=1;k<=(i+j-1);k++) v+=1.0/k;
            for(k=1;k<=(j-1);k++) v-=1.0/k;
            v-=(1.0/i+1.0/j)/exp(log_fac[i+j]-log_fac[i]-log_fac[j]);
            v*=2.0*N0*mu;
            s1+=v*gt1[i]*gt2[j];
        }
    }
    if(!flag1) s1+=2.0*N1*mu*(log(2.0*N1+(n1*T))-log(2.0*N1));

    //-- s2
    for(i=1;i<=n1;i++){
        for(j=1;j<=n2;j++){
            double v=0.0;
            for(k=1;k<=(i+j-1);k++) v+=1.0/k;
            for(k=1;k<=(i-1);k++) v-=1.0/k;
            v-=(1.0/i+1.0/j)/exp(log_fac[i+j]-log_fac[i]-log_fac[j]);
            v*=2.0*N0*mu;
            s2+=v*gt1[i]*gt2[j];
        }
    }
    if(!flag2) s2+=2.0*N2*mu*(log(2.0*N2+(n2*T))-log(2.0*N2));

    //-- s12
    for(i=1;i<=n1;i++){
        for(j=1;j<=n2;j++){
            double v=0.0;
            for(k=1;k<=(i-1);k++) v+=1.0/k;
            for(k=1;k<=(j-1);k++) v+=1.0/k;
            for(k=1;k<=(i+j-1);k++) v-=1.0/k;
            v+=(1.0/i+1.0/j)/exp(log_fac[i+j]-log_fac[i]-log_fac[j]);
            v*=2.0*N0*mu;
            s12+=v*gt1[i]*gt2[j];
        }
    }

    //-- D
    for(i=1;i<=n1;i++){
        for(j=1;j<=n2;j++){
            double a=2.0*N0*mu*(1.0/i+1.0/j);
            double tmp=exp(log_fac[i+j]-log_fac[i]-log_fac[j]);
            D+=(a/tmp)*gt1[i]*gt2[j];
        }
    }

    for(i=1;i<n1&&flag1;i++) s1+=E_S_n(i, n1, N1, mu, T, gt1);
    for(j=1;j<n2&&flag2;j++) s2+=E_S_n(j, n2, N2, mu, T, gt2);
    D+=E_S_n(n1, n1, N1, mu, T, gt1)+E_S_n(n2, n2, N2, mu, T, gt2);

    *(res_s1)=s1;
    *(res_s2)=s2;
    *(res_s12)=s12;
    *(res_D)=D;

    free(gt1);
    free(gt2);
}

//====================================

void s3_exp_2_to_3Is(double *arr, int dim1, int dim2, int dim3, int n1, int n2, int n3, double N1, double N2, double N3, double T1, double T2, double mu, double *s_0, double (*growth_model)(double *, double), double *paras){
    memset(arr, 0, dim1*dim2*dim3*sizeof(double));

    int i, i3, j, k, k1, k2, m, m1, m2, n;

    double T1_integral=cal_integral(0, T1, growth_model, paras);
    double T2_integral=cal_integral(0, T2, growth_model, paras);

    double *gt1=calloc(n1+1, sizeof(double));
    double *gt2=calloc(n2+1, sizeof(double));
    for(m=1;m<=n1;m++) gt1[m]=g_t(m, n1, N1, T1_integral);
    for(m=1;m<=n2;m++) gt2[m]=g_t(m, n2, N2, T2_integral);

    double *p_vec1=calloc(n1+1, sizeof(double));
    double *p_vec2=calloc(n2+1, sizeof(double));

    for(i3=0;i3<=n3;i3++){
        for(m1=1;m1<=n1;m1++){
            for(m2=1;m2<=n2;m2++){
                arr[i3]+=gt1[m1]*gt2[m2]*s_0[(m1+m2)*dim2*dim3+(0)*dim3+i3];
            }
        }
    }

    for(i3=0;i3<=n3;i3++){
        for(i=0;i<=n1;i++){
            for(j=0;j<=n2;j++){
                double value=0.0;
                for(m1=1;m1<=n1;m1++){
                    for(m2=1;m2<=n2;m2++){
                        double v=0.0;
                        for(k1=0;k1<=m1&&k1<=i;k1++) p_vec1[k1]=part_4(k1, i, n1, m1);
                        for(k2=0;k2<=m2&&k2<=j;k2++) p_vec2[k2]=part_4(k2, j, n2, m2);
                        for(k1=0;k1<=m1&&k1<=i;k1++){
                            for(k2=0;k2<=m2&&k2<=j;k2++){
                                double a=s_0[(m1+m2)*dim2*dim3+(k1+k2)*dim3+i3];
                                double b=part_3(k1, k2, m1, m2)*p_vec1[k1]*p_vec2[k2];
                                v+=(a*b);
                            }
                        }
                        value+=(gt1[m1]*gt2[m2]*v);
                    }
                }
                arr[i*dim2*dim3+j*dim3+i3]+=value;
            }
        }
    }

    for(i=1,j=0,i3=0;i<=n1;i++) arr[i*dim2*dim3+j*dim3+i3]+=E_S_n(i, n1, N1, mu, T1, gt1);
    for(i=0,j=1,i3=0;j<=n2;j++) arr[i*dim2*dim3+j*dim3+i3]+=E_S_n(j, n2, N2, mu, T2, gt2);

    free(gt1);
    free(gt2);
    free(p_vec1);
    free(p_vec2);
}

void s3_exp_3Is(double *arr, int dim1, int dim2, int dim3, int n1, int n2 ,int n3, double N1, double N2, double N3,double N4,double N5,double N6, double TT1, double TT2, double TT3, double TT0, double mu, double (*growth_model)(double *, double), double *paras){
    memset(arr, 0, dim1*dim2*dim3*sizeof(double));

    int i, j, k;

    double T1=TT1;
    double T2=TT2-TT1;
    double T3=TT3-TT2;
    double T0=TT0-TT2;

    int n4=n1+n2;
    int n0=n1+n2+n3;

    double *s_0=calloc(dim1*dim2*dim3, sizeof(double));

    int l_esa=n0+1;
    double *esa=calloc(l_esa*l_esa, sizeof(double));
    for(i=1;i<=n0;i++) afs_growth(esa+i*l_esa, i, T0, T3, N5, N6, mu, growth_model, paras);
    myerror("growth!");
    for(i=1;i<=n4;i++) s2_exp_jafs_v2(s_0+i*dim2*dim3, dim2, dim3, N4, N3, i, n3, T2, TT2, mu, l_esa, esa, growth_model, paras);
    free(esa);

    for(i=0;i<=n1;i++){
        for(j=0;j<=n2;j++){
            for(k=0;k<=n3;k++){
                double d=s_0[i*dim2*dim3+j*dim3+k];
                //if(d!=0) fprintf(stdout, "%d,%d,%d\t%f\n", i, j, k, d);
            }
        }
    }

    s3_exp_2_to_3Is(arr, dim1, dim2, dim3, n1, n2, n3, N1, N2, N3, T1, T1, mu, s_0, growth_model, paras);
    free(s_0);

    /*
    double *sb=calloc(n3+1, sizeof(double));
    nAFS(n3,T3,N3,sb,0.0,mu);
    for(i=0;i<=n3;i++) fprintf(stdout, "%d\t%f\n", i, sb[i]);
    free(sb);
    */
}

//====================================

void Asy_AFS_Gazave_CG(int n,int m,double *N,double *T,double r,double *S,double mu){
  int i,j;
  double *ET=calloc(n+1, sizeof(double));
  double *EW=calloc(n+1, sizeof(double));

  Asy_ET_Gazave_CG(n,m,N,T,r,ET);
  ET_2_EW_CG(n,ET,EW);
  AFS_fromEW(n,S,EW,mu);

  free(ET);
  free(EW);
}

double Asy_ET_Gazave_CG(int n,int m,double *N,double *T,double g,double *ET){
  //*ST is not finished yet. should be filled in. 6/10/2014.
  int i,j,k,indicator;
  double uj,tempValue;
  double *indV=calloc(m+1, sizeof(double));
  double *ETa=calloc(m+1, sizeof(double));

  for(i=0;i<n;i++) ET[i]=0.0;

  if(fabs(g)>0.0001) indV[1]=(exp(g*T[1])-1.0)/(N[1]*g);
  else indV[1]=T[1]/N[1];

  for(i=2;i<m;i++) indV[i]=T[i]/N[i]-T[i-1]/N[i]+indV[i-1];

  for(i=1;i<n;i++){
      uj=2.0*(1.0/i-1.0/n);
      if(fabs(g)>0.0001) ETa[1]=log(N[1]*uj*g+1.0)/g;
      else ETa[1]=N[1]*(uj);
      ETa[2]=N[2]*(uj-indV[1])+T[1];
      for(j=3;j<=m;j++) ETa[j]=N[j]*(ETa[j-1]-T[j-1])/N[j-1]+T[j-1];
      indicator=m;
      for(k=1;k<m;k++){
        if(uj<=indV[k]){
            indicator=k;
            break;
	    }
      }
      ET[i]=ETa[indicator];
  }
  free(indV);
  free(ETa);
}

void ET_2_EW_CG(int n,double *ET,double *EW){
  int i;
  for(i=2;i<=n;i++) EW[i]=ET[i-1]-ET[i];
  EW[n]=ET[n-1];
}

/*note the different notationhere, W-interC, T-coalT */
double AFS_fromEW(int n,double *S,double *EW,double mu){
  int j,k;
  for(j=1;j<=n;j++){
      S[j]=0.0;
      for(k=2;k<=n;k++) S[j]+=mu*EW[k]*k*descendDist(n,j,k);
  }
}

inline double descendDist(int n,int j,int k){
    return (n==0 || n<k || n-j-k+1<0) ? 0.0 :exp((log_fac[n-j-1]-log_fac[k-2]-log_fac[n-j-k+1])-(log_fac[n-1]-log_fac[k-1]-log_fac[n-k]));
}






//====================================
