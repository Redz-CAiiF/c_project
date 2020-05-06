/*
 Created by Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include <string.h> /*only memcpy*/
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
	ip_mat *pic;
    pic = (ip_mat *) malloc(sizeof(struct ip_mat));
    
    pic->w = w;
    pic->h = h;
    pic->k = k;
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
    /*switch (dimensione){
        case 0: 
        if(a->w == b->w && a->k == b->k){
            int i,j,z;
            ip_mat *out;
            out = ip_mat_create(a->h+b->h,a->w,a->k,0);
            
            
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
            
            for(i=0;i<out->k;i++){
                for(j=0;j<out->h;j++){
                    /*puntatore ad array di float*
                    memcpy(out->data[j], a->data[j], sizeof(float)*a->w);
                    memcpy(out->data[j]+a->w, a->data[j], sizeof(float)*a->w);
                    
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
            
            
            
            return out;
        }else{
            printf("Dimensions not correct\n");
            exit(1);
        }
        break;
    }*/
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
    if(alpha>=0 && alpha<=1 && a->w==b->w && a->h==b->h && a->k==b->k){
        int i,j,z;
        ip_mat *out;
        out = ip_mat_create(a->h,a->w,a->k,0);
        
        for(i=0;i<out->k;i++){
            for(j=0;j<out->h;j++){
                for(z=0;z<out->w;z++){
                    int blend = alpha * get_val(a,j,z,i) + (1-alpha)* get_val(b,j,z,i);
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

















