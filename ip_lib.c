/*
 Created by !Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "ip_lib.h"
#include "bmp.h"


struct stats{
    float min;
    float max;
    float mean;
};

struct ip_mat{
    unsigned int w; /* <- larghezza */
    unsigned int h; /* <- altezza */
    unsigned int k; /* <- canali */
    stats * stat;   /* <- statistiche per canale */
    float *** data; /* <- matrice 3D di valori float */
};


void ip_mat_show(ip_mat * t){
    unsigned int i,l,j;
    printf("Matrix of size %d x %d x %d (hxwxk)\n",t->h,t->w,t->k);
    for (l = 0; l < t->k; l++) {
        printf("Slice %d\n", l);
        for(i=0;i<t->h;i++) {
            for (j = 0; j < t->w; j++) {
                printf("%f ", get_val(t,i,j,l));
            }
            printf("\n");
        }
        printf("\n");
    }
}

void ip_mat_show_stats(ip_mat * t){
    unsigned int k;

    compute_stats(t);

    for(k=0;k<t->k;k++){
        printf("Channel %d:\n", k);
        printf("\t Min: %f\n", t->stat[k].min);
        printf("\t Max: %f\n", t->stat[k].max);
        printf("\t Mean: %f\n", t->stat[k].mean);
    }
}

ip_mat * bitmap_to_ip_mat(Bitmap * img){
    unsigned int i=0,j=0;

    unsigned char R,G,B;

    unsigned int h = img->h;
    unsigned int w = img->w;

    ip_mat * out = ip_mat_create(h, w,3,0);

    for (i = 0; i < h; i++)              /* rows */
    {
        for (j = 0; j < w; j++)          /* columns */
        {
            bm_get_pixel(img, j,i,&R, &G, &B);
            set_val(out,i,j,0,(float) R);
            set_val(out,i,j,1,(float) G);
            set_val(out,i,j,2,(float) B);
        }
    }

    return out;
}

Bitmap * ip_mat_to_bitmap(ip_mat * t){

    Bitmap *b = bm_create(t->w,t->h);

    unsigned int i, j;
    for (i = 0; i < t->h; i++)              /* rows */
    {
        for (j = 0; j < t->w; j++)          /* columns */
        {
            bm_set_pixel(b, j,i, (unsigned char) get_val(t,i,j,0),
                    (unsigned char) get_val(t,i,j,1),
                    (unsigned char) get_val(t,i,j,2));
        }
    }
    return b;
}

/* altezza|cols larghezza|rows canali */
float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k){
    if(i<a->h && j<a->w &&k<a->k){  /* j>=0 and k>=0 and i>=0 is non sense*/
        return a->data[k][i][j];
    }else{
        printf("Errore get_val!!!\n");
        exit(1);
    }
}

/* altezza|cols larghezza|rows canali */
void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v){
    if(i<a->h && j<a->w &&k<a->k){
        a->data[k][i][j]=v;
    }else{
        printf("Errore set_val!!!\n");
        exit(1);
    }
}

float get_normal_random(){
    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    return cos(2*PI*y2)*sqrt(-2.*log(y1));
}

/** PUNTO 1  **/

ip_mat * ip_mat_create(unsigned int h, unsigned int w,unsigned  int k, float v){
    int i,j,z;
	ip_mat *pic; /*indici di scorrimento canali, colonne, righe*/ 
    pic = (ip_mat *) malloc(sizeof(struct ip_mat)); /*creo la nuova matrice*/
    
    pic->w = w;/*righe*/
    pic->h = h;/*colonne*/ 
    pic->k = k;/*canali*/ 
    /*inizializzo stats*/
    pic->stat = (stats *) malloc(sizeof(struct stats)*k);/*array di stats di linghezza k*/
    
    /*inizializzo data*/
    pic->data = (float***) malloc(sizeof(float**)*k);
    for(i=0;i<k;i++){
        pic->data[i] = (float**) malloc(sizeof(float*)*h);
        for(j=0;j<h;j++){
            pic->data[i][j] = (float*) malloc(sizeof(float)*w);
            for(z=0;z<w;z++){
                pic->data[i][j][z] = v;
            }
        }
    }
    
    /* imposta i valori di min max e mean */
    compute_stats(pic);
    
    return pic;
}

void ip_mat_free(ip_mat *a){
    int i,j;
    
    /* free stats */
    free(a->stat);
    
    /* free data */
    for(i=0;i<a->k;i++){
        for(j=0;j<a->h;j++){
            free(a->data[i][j]);
        }
        free(a->data[i]);
    }
    free(a->data);
    
    free(a);
    
}

ip_mat * ip_mat_copy(ip_mat * in){
    int i,j,z;
	ip_mat *out;
    out = ip_mat_create(in->h,in->w,in->k,0);
    
    out->w = in->w;
    out->h = in->h;
    out->k = in->k;
    
    /*copio stats*/
    for(i=0;i<out->k;i++){
        /*copio tutte le celle di stats*/
        out->stat[i] = in->stat[i];
    }
    
    /*copio data*/
    for(i=0;i<out->k;i++)
        for(j=0;j<out->h;j++)
            for(z=0;z<out->w;z++)
                set_val(out,j,z,i,get_val(in,j,z,i));
    
    return out;
}

/* devo sapere se row_end e col_end sono inclusi oppure no
 * */
ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end){
    int i,j,z;
    ip_mat *out;
    out = ip_mat_create(col_end-col_start,row_end-row_start,t->k,0);
    
    for(i=0;i<t->k;i++){
        for(j=col_start;j<col_end;j++){
            for(z=row_start;z<row_end;z++){
                set_val(out,j-col_start,z-row_start,i,get_val(t,j,z,i)); /*devo ridurre l'input in set val*/
            }
        }
    }
    
    return out;
}

/* Inizializza una ip_mat con dimensioni w h e k.
 * Ogni elemento è generato da una gaussiana con media mean e varianza var
 * 
 * Genera dei numeri casuali con distribuzione Normale
 */
void ip_mat_init_random(ip_mat * t, float mean, float var){
    int i,j,z;
    compute_stats(t);
    
    for(i=0;i<t->k;i++){
        for(j=0;j<t->h;j++){
            for(z=0;z<t->w;z++){
                /*float gauss = 1/(sqrt(2*PI*pow(var,2)))*exp((-pow((t->stat[i].max-mean), 2))/(2*pow(var,2)));*/
                float gauss = get_normal_random()*var+mean;
                set_val(t,j,z,i,gauss);
            }
        }
    }
}

/**** PARTE 1: OPERAZIONI MATEMATICHE FRA IP_MAT ****/
ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b){
    if(a->w==b->w && a->h==b->h && a->k==b->k){
        int i,j,z;
        ip_mat *out;
        out = ip_mat_create(a->h,a->w,a->k,0);
        
        for(i=0;i<out->k;i++)
            for(j=0;j<out->h;j++)
                for(z=0;z<out->w;z++)
                    set_val(out,j,z,i,(get_val(a,j,z,i)+get_val(b,j,z,i)));
        
       return out;
    }else{
        printf("Dimensions not equals\n");
        exit(1);
    }
}

ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b){
    if(a->w==b->w && a->h==b->h && a->k==b->k){
        int i,j,z;
        ip_mat *out;
        out = ip_mat_create(a->h,a->w,a->k,0);
        
        for(i=0;i<out->k;i++)
            for(j=0;j<out->h;j++)
                for(z=0;z<out->w;z++)
                    set_val(out,j,z,i,(get_val(a,j,z,i)-get_val(b,j,z,i)));
        
       return out;
    }else{
        printf("Dimensions not equals\n");
        exit(1);
    }
}

ip_mat * ip_mat_mul_scalar(ip_mat *a, float c){
    int i,j,z;
    ip_mat *out;
    out = ip_mat_create(a->h,a->w,a->k,0);
    
    for(i=0;i<out->k;i++)
        for(j=0;j<out->h;j++)
            for(z=0;z<out->w;z++)
                set_val(out,j,z,i,(get_val(a,j,z,i)*c));
    return out;
}

ip_mat * ip_mat_add_scalar(ip_mat *a, float c){
    int i,j,z;
    ip_mat *out;
    out = ip_mat_create(a->h,a->w,a->k,0);
    
    for(i=0;i<out->k;i++)
        for(j=0;j<out->h;j++)
            for(z=0;z<out->w;z++)
                set_val(out,j,z,i,(get_val(a,j,z,i)+c));
    
    return out;
}

/* NON SONO SICURO SU QUESTA */
ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b){
    /* ogni elem è la media tra a e b */
    int i,j,z;
    ip_mat *out;
    out = ip_mat_create(a->h,a->w,a->k,0);
    
    for(i=0;i<out->k;i++)
        for(j=0;j<out->h;j++)
            for(z=0;z<out->w;z++)
                set_val(out,j,z,i,(get_val(a,j,z,i)+get_val(b,j,z,i))/2);
    
    return out;
}





/* Concatena due ip_mat su una certa dimensione.
 * Ad esempio:
 * ip_mat_concat(ip_mat * a, ip_mat * b, 0);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h + b.h
 *      out.w = a.w = b.w
 *      out.k = a.k = b.k
 *
 * ip_mat_concat(ip_mat * a, ip_mat * b, 1);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h = b.h
 *      out.w = a.w + b.w
 *      out.k = a.k = b.k
 *
 * ip_mat_concat(ip_mat * a, ip_mat * b, 2);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h = b.h
 *      out.w = a.w = b.w
 *      out.k = a.k + b.k
 * */
ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione){
    switch (dimensione){
        case 0: 
        if(a->w == b->w && a->k == b->k){
            int i,j,z;
            ip_mat *out;
            out = ip_mat_create(a->h+b->h,a->w,a->k,0);
            for(i=0;i<a->k;i++){
                for(j=0;j<a->h;j++){
                    for(z=0;z<a->w;z++){
                        float tmp = get_val(a,j,z,i);
                        set_val(out,j,z,i,tmp);
                    }
                }
            }
            for(i=0;i<b->k;i++){
                for(j=0;j<b->h;j++){
                    for(z=0;z<b->w;z++){
                        float tmp = get_val(b,j,z,i);
                        set_val(out,(j+(a->h)),z,i,tmp);
                    }
                }
            }
            
            return out;
        }else{
            printf("Dimensions not correct\n");
            exit(1);
        }
        break;
        case 1:
        if(a->h == b->h && a->k == b->k){
            int i,j,z;
            ip_mat *out;
            out = ip_mat_create(a->h,a->w+b->w,a->k,0);
            for(i=0;i<a->k;i++){
                for(j=0;j<a->h;j++){
                    for(z=0;z<a->w;z++){
                        set_val(out,j,z,i,get_val(a,j,z,i));
                    }
                }
            }
            for(i=0;i<b->k;i++){
                for(j=0;j<b->h;j++){
                    for(z=0;z<b->w;z++){
                        set_val(out,j,z+(a->w),i,get_val(b,j,z,i));
                    }
                }
            }
            return out;
        }else{
            printf("Dimensions not correct\n");
            exit(1);
        }
        break;
        case 2:
        if(a->h == b->h && a->w == b->w){
            int i,j,z;
            ip_mat *out;
            out = ip_mat_create(a->h,a->w,a->k+b->k,0);
            for(i=0;i<a->k;i++){
                for(j=0;j<a->h;j++){
                    for(z=0;z<a->w;z++){
                        set_val(out,j,z,i,get_val(a,j,z,i));
                    }
                }
            }
            for(i=0;i<b->k;i++){
                for(j=0;j<b->h;j++){
                    for(z=0;z<b->w;z++){
                        set_val(out,j,z,i+(a->k),get_val(b,j,z,i));
                    }
                }
            }
            
            return out;
        }else{
            printf("Dimensions not correct\n");
            exit(1);
        }
        break;
        default:
            printf("General error\n");
            exit(1);
    }
}

void compute_stats(ip_mat * t){
    /*per ogni canale
    
        scorro le colonne
            scorro le righe
                sommo a tot ogni valore
                count +1
                se è il min setto min
                se è il max setto max
        calcolo tot/count
        
        inserisco i dati in stats di i
    */
    int i,j,z;
    
    for(i=0;i<t->k;i++){
        float tot, count, max, min;
        tot = count = 0.0;
        max = INT_MIN;
        min = INT_MAX;
        
        for(j=0;j<t->h;j++){
            for(z=0;z<t->w;z++){
                float tmp = get_val(t,j,z,i);
                tot += tmp;
                count++;
                if(tmp<min) min = tmp;
                if(tmp>max) max = tmp;
            }
        }
        
        t->stat[i].min = min;
        t->stat[i].max = max;
        t->stat[i].mean = tot/count; /* avg */
    }

}



/**** PARTE 2: SEMPLICI OPERAZIONI SU IMMAGINI ****/

ip_mat * ip_mat_to_gray_scale(ip_mat * in){
    int i,j,z;
    ip_mat *out;
    out = ip_mat_create(in->h,in->w,in->k,0);
    
    for(j=0;j<out->h;j++){
        for(z=0;z<out->w;z++){
            float tot,count;
            tot = count = 0.0;
            for(i=0;i<out->k;i++){
                tot += get_val(in,j,z,i);
                count ++;
            }
            for(i=0;i<out->k;i++){
                set_val(out,j,z,i,tot/count);
            }
        }
    }
    
    return out;
}

/* Effettua la fusione (combinazione convessa) di due immagini */
/* DA testare */
ip_mat * ip_mat_blend(ip_mat * a, ip_mat * b, float alpha){
    if(alpha>=0.0 && alpha<=1.0 && a->w==b->w && a->h==b->h && a->k==b->k){
        int i,j,z;
        ip_mat *out;
        out = ip_mat_create(a->h,a->w,a->k,0);
        
        for(i=0;i<out->k;i++){
            for(j=0;j<out->h;j++){
                for(z=0;z<out->w;z++){
                    float blend = alpha * get_val(a,j,z,i) + (1-alpha)* get_val(b,j,z,i);
                    set_val(out,j,z,i,blend);
                }
            }
        }
        
        return out;
    }else{
        printf("Alpha not in range or dimensions not equals\n");
        exit(1);
    }
}

ip_mat * ip_mat_brighten(ip_mat * a, float bright){
    return ip_mat_add_scalar(a, bright);
}

ip_mat * ip_mat_corrupt(ip_mat * a, float amount){
    ip_mat *out,*tmp;
    out = ip_mat_create(a->h,a->w,a->k,0);
    ip_mat_init_random(out,0,0.25);
    tmp=ip_mat_mul_scalar(out,amount);
    out=ip_mat_sum(a,tmp);
    free(tmp);
    return out;
}


/**** PARTE 3: CONVOLUZIONE E FILTRI *****/

ip_mat * ip_mat_convolve(ip_mat * a, ip_mat * f){
    int c,r;
    ip_mat *out, *new_a;
    out = ip_mat_create(a->h,a->w,a->k,0);
    new_a = ip_mat_padding(a,(f->h-1)/2,(f->w-1)/2);
    
    for(c=0;c<=new_a->h-f->h;c++){
        for(r=0;r<=new_a->w-f->w;r++){
            int ii,ic,ir;
            ip_mat *sub_new_a;
            sub_new_a = ip_mat_subset(new_a, r, r+f->w, c, c+f->h);
            
            for(ii=0;ii<sub_new_a->k;ii++){
                float tot = 0.0;
                for(ic=0;ic<sub_new_a->h;ic++){
                    for(ir=0;ir<sub_new_a->h;ir++){
                        float t1, t2;
                        t1 = get_val(sub_new_a,ic,ir,ii);
                        t2 = get_val(f,ic,ir,0); /* 0 perche il filtro ha sempre solo una dimensione */
                        tot += t1*t2;
                    }
                }
                set_val(out,c,r,ii,tot);
            }
            
            ip_mat_free(sub_new_a);
            
        }
    }
    ip_mat_free(new_a);
    return out;
}

ip_mat * ip_mat_padding(ip_mat * a, int pad_h, int pad_w){
    int i,c,r;
    ip_mat *out;
    out = ip_mat_create(a->h+(2*pad_h),a->w+(2*pad_w),a->k,0);
    
    for(i=0;i<a->k;i++){
        for(c=0;c<a->h;c++){
            for(r=0;r<a->w;r++){
                set_val(out, c+pad_h, r+pad_w, i, get_val(a,c,r,i));
            }
        }
    }
    return out;
}


ip_mat * create_sharpen_filter(){
    ip_mat *out;
    out = ip_mat_create(3,3,1,0);
    set_val(out,0,0,0,0);
    set_val(out,0,1,0,-1);
    set_val(out,0,2,0,0);
    set_val(out,1,0,0,-1);
    set_val(out,1,1,0,5);
    set_val(out,1,2,0,-1);
    set_val(out,2,0,0,0);
    set_val(out,2,1,0,-1);
    set_val(out,2,2,0,0);
    return out;
}    

ip_mat * create_edge_filter(){
    ip_mat *out;
    out = ip_mat_create(3,3,1,0);
    set_val(out,0,0,0,-1);
    set_val(out,0,1,0,-1);
    set_val(out,0,2,0,-1);
    set_val(out,1,0,0,-1);
    set_val(out,1,1,0,8);
    set_val(out,1,2,0,-1);
    set_val(out,2,0,0,-1);
    set_val(out,2,1,0,-1);
    set_val(out,2,2,0,-1);
    return out;
} 

ip_mat * create_emboss_filter(){
    ip_mat *out;
    out = ip_mat_create(3,3,1,0);
    set_val(out,0,0,0,-2);
    set_val(out,0,1,0,-1);
    set_val(out,0,2,0,0);
    set_val(out,1,0,0,-1);
    set_val(out,1,1,0,1);
    set_val(out,1,2,0,1);
    set_val(out,2,0,0,0);
    set_val(out,2,1,0,1);
    set_val(out,2,2,0,2);
    return out;
} 

ip_mat * create_average_filter(int w, int h, int k){
    int i,j,z;
    float c=1./(w*h);
    ip_mat *out;
    out = ip_mat_create(h,w,k,0);
    
    for(i=0;i<out->k;i++){
        for(j=0;j<out->h;j++){
            for(z=0;z<out->w;z++){
                   set_val(out,j,z,i,c); 
            }
        }
    }
    return out;
}

ip_mat * create_gaussian_filter(int w, int h, int k, float sigma){
    ip_mat *out;
    int i,j;
    float x, y, gauss, sum;
    
    sum = 0.0;
    out = ip_mat_create(h,w,1,0);
    
    for(i=0;i<out->h;i++){
        for(j=0;j<out->w;j++){
            x= i-(h-1)/2.0;
            y= j-(w-1)/2.0;
            gauss=1.* exp((pow(x,2.0)+(pow(y,2.0))/((2.0*pow(sigma,2.0))))*(-1.0));
            set_val(out,i,j,0,gauss);
            sum=sum+gauss;
        }
    }
    for(i=0;i<out->h;i++){
        for(j=0;j<out->w;j++){
            set_val(out,i,j,0,out->data[0][i][j]/sum);
        }
    }
    return out;
}
void rescale(ip_mat * t, float new_max){
    int i,j,z;
    compute_stats(t);
    for(i=0;i<t->k;i++){
        for(j=0;j<t->h;j++){
            for(z=0;z<t->w;z++){
                float c;
                c=(get_val(t,j,z,i)-(t->stat[i].min))/((t->stat[i].max)-(t->stat[i].min));
                printf("%f ",c);
                set_val(t,j,z,i,c*new_max);
            }
            printf("\n");
        }
        printf("\n\n\naltro strato:: \n");
    }
}

/* Nell'operazione di clamping i valori <low si convertono in low e i valori >high in high.*/
void clamp(ip_mat * t, float low, float high){
    int i,j,z;
    for(i=0;i<t->k;i++){
        for(j=0;j<t->h;j++){
            for(z=0;z<t->w;z++){
                float val = get_val(t,j,z,i);
                if(val<low){
                    set_val(t,j,z,i,low);
                }else if(val>high){
                    set_val(t,j,z,i,high);
                }
            }
        }
    }
}








