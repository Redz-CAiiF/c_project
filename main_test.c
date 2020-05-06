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
    

int main (int argc, char * argv[]) {
    ip_mat *p,*b,*d;
    
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
    
    /*printf("CONCAT:\n");
    d = ip_mat_concat(p,b,1);
    print_ip_mat(d);
    
    ip_mat_free(d);*/
    
    printf("STATS:\n");
    compute_stats(p);
    ip_mat_show_stats(p);
    
    printf("MEDIA MATRICI:\n");
    d = ip_mat_mean(p, b);
    print_ip_mat(d);
    
    
    ip_mat_free(b);
    ip_mat_free(p);
    ip_mat_free(d);
     
    return 0;
}
