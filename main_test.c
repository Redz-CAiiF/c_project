#include <stdio.h>
#include <string.h>
#include "ip_lib.h"
#include "bmp.h"


void print_ip_mat(ip_mat *b){
    int i,j,z;
    printf("FULL output:\n");
    for(i=0;i<b->k;i++){
        for(j=0;j<b->h;j++){
            for(z=0;z<b->w;z++)
                printf("%f ",get_val(b,j,z,i));

            printf("\n");
        }
        printf("\nDIM %d:\n",i);
    }
}


void test_fun(){
    ip_mat *p,*b,*d, *f;
    
    
    p = ip_mat_create(5,4,3,5);
    
    printf("\n%d %d %d\n",p->w,p->h,p->k);
    printf("%f\n",p->data[0][0][0]);
    printf("%f\n",p->stat[0].min);
    
    printf("get_val: %f\n",get_val(p,0,0,0));
    set_val(p,0,0,0,4);
    printf("get_val after set: %f\n",get_val(p,0,0,0));
    
    print_ip_mat(p);
    
    
    b = ip_mat_copy(p);
    
    printf("get_val in copy: %f\n",get_val(b,0,0,0));
    
    ip_mat_free(b);
    
    b = ip_mat_subset(p,1,3,1,2);
    
    print_ip_mat(b);
    
    ip_mat_free(b);
    ip_mat_free(p);
    
    p = ip_mat_create(5,4,3,5);
    b = ip_mat_create(5,4,3,3);
    
    printf("SUM:\n");
    d = ip_mat_sum(p,b);
    print_ip_mat(d);
    
    ip_mat_free(d);
    
    printf("SUB:\n");
    d = ip_mat_sub(p,b);
    print_ip_mat(d);
    
    ip_mat_free(d);
    
    printf("MUL S:\n");
    d = ip_mat_mul_scalar(p,5);
    print_ip_mat(d);
    
    ip_mat_free(d);
    
    printf("ADD S:\n");
    d = ip_mat_add_scalar(p,-5);
    print_ip_mat(d);
    
    
    
    ip_mat_free(d);
    
    printf("GRAY SCALE:\n");
    d = ip_mat_to_gray_scale(p);
    print_ip_mat(d);
    
    ip_mat_free(d);
    
    printf("STATS:\n");
    compute_stats(p);
    ip_mat_show_stats(p);
    
    printf("MEDIA MATRICI:\n");
    d = ip_mat_mean(p, b);
    print_ip_mat(d);
    
    ip_mat_free(d);
    
    d = ip_mat_create(3,3,3,0);
    
    printf("INIT RANDOM:\n");
    ip_mat_init_random(d, 3.5, 35);
    print_ip_mat(d);
    
    
    ip_mat_free(b);
    ip_mat_free(p);
    ip_mat_free(d);
    
    p = ip_mat_create(5,4,1,5);
    b = ip_mat_create(5,4,1,3);
    
    printf("CONCAT:\n");
    d = ip_mat_concat(p,b,0);
    
    set_val(p,2,2,0,10.);
    
    print_ip_mat(d);
    print_ip_mat(p);
    
    
    ip_mat_free(b);
    ip_mat_free(p);
    ip_mat_free(d);
    
    
    
    
    
    
    
    
    p = ip_mat_create(5,5,1,0);
    b = ip_mat_create(3,3,1,0);
    
    set_val(p,0,0,0,7);
    set_val(p,0,1,0,2);
    set_val(p,0,2,0,3);
    set_val(p,0,3,0,3);
    set_val(p,0,4,0,8);
    
    set_val(p,1,0,0,4);
    set_val(p,1,1,0,5);
    set_val(p,1,2,0,3);
    set_val(p,1,3,0,8);
    set_val(p,1,4,0,4);
    
    set_val(p,2,0,0,3);
    set_val(p,2,1,0,3);
    set_val(p,2,2,0,2);
    set_val(p,2,3,0,8);
    set_val(p,2,4,0,4);
    
    set_val(p,3,0,0,2);
    set_val(p,3,1,0,8);
    set_val(p,3,2,0,7);
    set_val(p,3,3,0,2);
    set_val(p,3,4,0,7);
    
    set_val(p,4,0,0,5);
    set_val(p,4,1,0,4);
    set_val(p,4,2,0,4);
    set_val(p,4,3,0,5);
    set_val(p,4,4,0,4);
    
    printf("ORIGINAL:\n");
    print_ip_mat(p);
    
    
    set_val(b,0,0,0,1);
    set_val(b,0,1,0,0);
    set_val(b,0,2,0,-1);
    
    set_val(b,1,0,0,1);
    set_val(b,1,1,0,0);
    set_val(b,1,2,0,-1);
    
    set_val(b,2,0,0,1);
    set_val(b,2,1,0,0);
    set_val(b,2,2,0,-1);
    
    printf("KERNEL:\n");
    print_ip_mat(b);
    
    f = ip_mat_padding(p, 1, 1);
    printf("PADDING:\n");
    print_ip_mat(f);
    
    d = ip_mat_convolve(p,b);
    printf("OUTPUT:\n");
    print_ip_mat(d);
    
    ip_mat_free(b);
    ip_mat_free(p);
    ip_mat_free(d);
    ip_mat_free(f);
    
    
    p = ip_mat_create(5,5,3,5);
    b = ip_mat_create(3,3,1,0);
    
    set_val(b,0,0,0,1);
    set_val(b,0,1,0,0);
    set_val(b,0,2,0,-1);
    
    set_val(b,1,0,0,1);
    set_val(b,1,1,0,0);
    set_val(b,1,2,0,-1);
    
    set_val(b,2,0,0,1);
    set_val(b,2,1,0,0);
    set_val(b,2,2,0,-1);
    
    
    d = ip_mat_convolve(p,b);
    printf("\n\nOUTPUT2:\n");
    print_ip_mat(d);
    
    ip_mat_free(b);
    ip_mat_free(p);
    ip_mat_free(d);
    ip_mat_free(f);
    
}


int test2(){
    
}

int main (int argc, char * argv[]) {

    ip_mat *p,*b,*d;
    
    p = ip_mat_create(5,5,1,0);
    b = ip_mat_create(3,3,1,0);
    
    set_val(p,0,0,0,7);
    set_val(p,0,1,0,2);
    set_val(p,0,2,0,3);
    set_val(p,0,3,0,3);
    set_val(p,0,4,0,8);
    
    set_val(p,1,0,0,4);
    set_val(p,1,1,0,5);
    set_val(p,1,2,0,3);
    set_val(p,1,3,0,8);
    set_val(p,1,4,0,4);
    
    set_val(p,2,0,0,3);
    set_val(p,2,1,0,3);
    set_val(p,2,2,0,2);
    set_val(p,2,3,0,8);
    set_val(p,2,4,0,4);
    
    set_val(p,3,0,0,2);
    set_val(p,3,1,0,8);
    set_val(p,3,2,0,7);
    set_val(p,3,3,0,2);
    set_val(p,3,4,0,7);
    
    set_val(p,4,0,0,5);
    set_val(p,4,1,0,4);
    set_val(p,4,2,0,4);
    set_val(p,4,3,0,5);
    set_val(p,4,4,0,4);
    
    printf("ORIGINAL:\n");
    print_ip_mat(p);
    
    
    set_val(b,0,0,0,1);
    set_val(b,0,1,0,0);
    set_val(b,0,2,0,-1);
    
    set_val(b,1,0,0,1);
    set_val(b,1,1,0,0);
    set_val(b,1,2,0,-1);
    
    set_val(b,2,0,0,1);
    set_val(b,2,1,0,0);
    set_val(b,2,2,0,-1);
    
    printf("ORIGINAL:\n");
    print_ip_mat(b);
    
    
    d = ip_mat_convolve(p,b);
    printf("OUTPUT:\n");
    print_ip_mat(d);
    
    
    /* immage generating area */
    Bitmap *temp_bit, *temp_bit2, *immagine_prova;
    ip_mat *map_ip,*map_ip2,*gray_temp_bit, *ip_map_imm_p, *ip_map_imm_p_elab;
    temp_bit = bm_load("flower2.bmp");
    temp_bit2 = bm_load("mongolfiere.bmp");
    map_ip = bitmap_to_ip_mat(temp_bit);
    map_ip2 = bitmap_to_ip_mat(temp_bit2);

    gray_temp_bit = ip_mat_corrupt(map_ip, 100.);
    
    clamp(gray_temp_bit, 0, 255);
    
    temp_bit = ip_mat_to_bitmap(gray_temp_bit);
    
    bm_save(temp_bit,"grey_scale.bmp");
    
    
    
    
    immagine_prova = bm_load("flower.bmp");
    
    ip_map_imm_p = bitmap_to_ip_mat(immagine_prova);
    
    ip_map_imm_p_elab = ip_mat_convolve(ip_map_imm_p, create_edge_filter());

    /*create_gaussian_filter(7,7,1,.5)*/
    rescale(ip_map_imm_p_elab,255);
    
    /*clamp(ip_map_imm_p_elab, 0, 255);*/
    
    immagine_prova = ip_mat_to_bitmap(ip_map_imm_p_elab);

    bm_save(immagine_prova,"filter_test.bmp");
    
    ip_mat_free(b);
    ip_mat_free(p);
    ip_mat_free(d);
    ip_mat_free(map_ip);
    ip_mat_free(map_ip2);
    ip_mat_free(gray_temp_bit);
    ip_mat_free(ip_map_imm_p);
    ip_mat_free(ip_map_imm_p_elab);
    
    return 0;
}



























