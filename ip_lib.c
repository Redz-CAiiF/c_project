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

    compute_stats(out);

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


/*
* Restituisce il valore in posizione i,j,k *
float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k){
    if(i<a->h && j<a->w &&k<a->k){
        return a->data[i][j][k];
    }else{
        printf("Errore get_val!!!");
        exit(1);
    }
}

* Setta il valore in posizione i,j,k a v *
void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v){
    if(i<a->h && j<a->w &&k<a->k){
        a->data[i][j][k]=v;
    }else{
        printf("Errore set_val!!!");
        exit(1);
    }
}*/

float get_normal_random(float media, float std){

    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float num = cos(2*PI*y2)*sqrt(-2.*log(y1));

    return media + num*std;
}











/**** PARTE 1: TIPO DI DATI ip_mat E MEMORIA ****/

/* Inizializza una ip_mat con dimensioni h w e k. Ogni elemento è inizializzato a v.
 * Inoltre crea un vettore di stats per contenere le statische sui singoli canali.
 * */
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


/* Libera la memoria (data, stat e la struttura)
 *
 * se la variabile "a" è NULL non fa nulla.
 *
 * */
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



/* Calcola il valore minimo, il massimo e la media per ogni canale
 * e li salva dentro la struttura ip_mat stats
 * */
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

/* Inizializza una ip_mat con dimensioni w h e k.
 * Ogni elemento è generato da una gaussiana con media mean e deviazione std */
void ip_mat_init_random(ip_mat * t, float mean, float std){
    int i,j,z;
    compute_stats(t);
    
    for(i=0;i<t->k;i++){
        for(j=0;j<t->h;j++){
            for(z=0;z<t->w;z++){
                /*float gauss = 1/(sqrt(2*PI*pow(std,2)))*exp((-pow((t->stat[i].max-mean), 2))/(2*pow(std,2)));*/
                float gauss = get_normal_random(mean, std);
                set_val(t,j,z,i,gauss);
            }
        }
    }
}

/* Crea una copia di una ip_mat e lo restituisce in output */
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

/* Restituisce una sotto-matrice, ovvero la porzione individuata da:
 * t->data[row_start...row_end][col_start...col_end][0...k]
 * La terza dimensione la riportiamo per intero, stiamo in sostanza prendendo un sottoinsieme
 * delle righe e delle colonne.
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 * */
/* devo sapere se row_end e col_end sono inclusi oppure no
 * */
ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end){
    int i,j,z;
    ip_mat *out;
    out = ip_mat_create(row_end-row_start,col_end-col_start,t->k,0);
    
    for(i=0;i<t->k;i++){
        for(j=col_start;j<col_end;j++){
            for(z=row_start;z<row_end;z++){
                set_val(out,z-row_start,j-col_start,i,get_val(t,z,j,i)); /*devo ridurre l'input in set val*/
            }
        }
    }
    
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
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
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


/**** PARTE 1: OPERAZIONI MATEMATICHE FRA IP_MAT ****/
/* Esegue la somma di due ip_mat (tutte le dimensioni devono essere identiche).
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
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

/* Esegue la sottrazione di due ip_mat (tutte le dimensioni devono essere identiche)
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
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


/* Moltiplica un ip_mat per uno scalare c. Si moltiplica c per tutti gli elementi di "a"
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
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

/* Aggiunge ad un ip_mat uno scalare c e lo restituisce in un nuovo tensore in output.
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
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

/* Calcola la media di due ip_mat a e b. La media si calcola per coppie delle due matrici aventi gli stessi indici
 * C[i][j][k]= (A[i][j][k]+B[i]j[k])/2
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
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


/**** PARTE 2: SEMPLICI OPERAZIONI SU IMMAGINI ****/
/* Converte un'immagine RGB ad una immagine a scala di grigio.
 * Quest'operazione viene fatta calcolando la media per ogni pixel sui 3 canali
 * e creando una nuova immagine avente per valore di un pixel su ogni canale la media appena calcolata.
 * Avremo quindi che tutti i canali saranno uguali.
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 * */
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

/* Effettua la fusione (combinazione convessa) di due immagini.
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
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

/* Operazione di brightening: aumenta la luminosità dell'immagine
 * aggiunge ad ogni pixel un certo valore
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
ip_mat * ip_mat_brighten(ip_mat * a, float bright){
    return ip_mat_add_scalar(a, bright);
}

/* Operazione di corruzione con rumore gaussiano:
 * Aggiunge del rumore gaussiano all'immagine, il rumore viene enfatizzato
 * per mezzo della variabile amount.
 *
 * out = a + gauss_noise*amount
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 * */
ip_mat * ip_mat_corrupt(ip_mat * a, float amount){
    ip_mat *out,*tmp;
    tmp = ip_mat_create(a->h,a->w,a->k,0);
    
    ip_mat_init_random(tmp,0,amount/2.0);
    out=ip_mat_sum(a,tmp);
    
    free(tmp);
    return out;
}

/**** PARTE 3: CONVOLUZIONE E FILTRI *****/

/* Effettua la convoluzione di un ip_mat "a" con un ip_mat "f".
 * La funzione restituisce un ip_mat delle stesse dimensioni di "a".
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
ip_mat * ip_mat_convolve(ip_mat * a, ip_mat * f){
    int c,r;
    ip_mat *out, *new_a;
    out = ip_mat_create(a->h,a->w,a->k,0);
    new_a = ip_mat_padding(a,(f->h-1)/2,(f->w-1)/2);
    
    for(c=0;c<=new_a->h-f->h;c++){
        for(r=0;r<=new_a->w-f->w;r++){
            int ii,ic,ir;
            ip_mat *sub_new_a;
            sub_new_a = ip_mat_subset(new_a, c, c+f->h, r, r+f->w);
            
            for(ii=0;ii<sub_new_a->k;ii++){
                float tot = 0.0;
                for(ic=0;ic<sub_new_a->h;ic++){
                    for(ir=0;ir<sub_new_a->h;ir++){
                        float t1, t2;
                        t1 = get_val(sub_new_a,ic,ir,ii);
                        t2 = get_val(f,ic,ir,ii);
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

/* Aggiunge un padding all'immagine. Il padding verticale è pad_h mentre quello
 * orizzontale è pad_w.
 * L'output sarà un'immagine di dimensioni:
 *      out.h = a.h + 2*pad_h;
 *      out.w = a.w + 2*pad_w;
 *      out.k = a.k
 * con valori nulli sui bordi corrispondenti al padding e l'immagine "a" riportata
 * nel centro.
 *
 * I parametri della funzione non subiscono modiche, il risultato viene salvato e restituito in output
 * all'interno di una nuova ip_mat.
 */
ip_mat * ip_mat_padding(ip_mat * a, unsigned int pad_h, unsigned int pad_w){
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

/* The input is a 2d matrix containing all the values of the filter */
ip_mat * create_filter_from_data(float filter[3][3]){
    int k,c,r;
    unsigned int cols,rows;
    ip_mat *out;
    cols = rows = 3;
    out = ip_mat_create(rows,cols,3,0);
    
    for(k=0;k<3;k++){
        for(c=0;c<cols;c++){
            for(r=0;r<rows;r++){
                set_val(out, c, r, k, filter[c][r]);
            }
        }
    }
    
    return out;
}

/* Crea un filtro di sharpening */
ip_mat * create_sharpen_filter(){
    float filter[3][3] = {{0, -1, 0},{-1, 5, -1},{0, -1, 0}};
    
    return create_filter_from_data(filter);
}

/* Crea un filtro per rilevare i bordi */
ip_mat * create_edge_filter(){
    float filter[3][3] = {{-1, -1, -1},{-1, 8, -1},{-1, -1, -1}};
    
    return create_filter_from_data(filter);
}

/* Crea un filtro per aggiungere profondità */
ip_mat * create_emboss_filter(){
    float filter[3][3] = {{-2, -1, 0},{-1, 1, 1},{0, 1, 2}};
    
    return create_filter_from_data(filter);
}

/* Crea un filtro medio per la rimozione del rumore */
ip_mat * create_average_filter(unsigned int h, unsigned int w, unsigned int k){
    int i,j;
    float filter[3][3];
    float c=1./(w*h);
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            filter[i][j] = c;
    
    return create_filter_from_data(filter);
    
}

/* Crea un filtro gaussiano per la rimozione del rumore */
ip_mat * create_gaussian_filter(unsigned int h, unsigned int w, unsigned int k, float sigma){

	float somma=0;
	int i,j,z;
    ip_mat *out;
    out = ip_mat_create(h,w,k,0);
	
	for (i=0;i<h;i++){
		for(j=0;j<w;j++){
			float x= i-(h-1)/2.;
			float y= j-(w-1)/2.;
			float ris=1.*exp(((pow(x,2)+pow(y,2))/((2*pow(sigma,2))))*(-1));
			somma+=ris;
            set_val(out,i,j,0,ris);  
		}
	}
	for (i=0;i<h;i++){
		for(j=0;j<w;j++){
            float ris=get_val(out,i,j,0)/somma;
            for(z=0;z<k;z++){
                set_val(out,i,j,z,ris);  
            }    
		}
	}

    return out;
}

/* Effettua una riscalatura dei dati tale che i valori siano in [0,new_max].
 * Utilizzate il metodo compute_stat per ricavarvi il min, max per ogni canale.
 *
 * I valori sono scalati tramite la formula (valore-min)/(max - min)
 *
 * Si considera ogni indice della terza dimensione indipendente, quindi l'operazione
 * di scalatura va ripetuta per ogni "fetta" della matrice 3D.
 * Successivamente moltiplichiamo per new_max gli elementi della matrice in modo da ottenere un range
 * di valori in [0,new_max].
 *
 * Il risultato dell'operazione si salva in t
 * */
void rescale(ip_mat * t, float new_max){
    int i,j,z;
    compute_stats(t);
    for(i=0;i<t->k;i++){
        for(j=0;j<t->h;j++){
            for(z=0;z<t->w;z++){
                float c;
                c=(get_val(t,j,z,i)-(t->stat[i].min))/((t->stat[i].max)-(t->stat[i].min));
                set_val(t,j,z,i,c*new_max);
            }
        }
    }
}

/* Nell'operazione di clamping i valori <low si convertono in low e i valori >high in high.
 *
 * Il risultato dell'operazione si salva in t
 * */
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





















