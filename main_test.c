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



void test2(){
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
}

int main (int argc, char * argv[]) {

    ip_mat *p,*b,*d;
    
    /*p = ip_mat_create(5,5,1,1);
    b = ip_mat_create(3,5,1,3);
    
    set_val(p,3,2,0,50);
    
    printf("OUTPUT 1:\n");
    print_ip_mat(p);
    printf("OUTPUT 2:\n");
    print_ip_mat(b);
    
    compute_stats(p);
    
    ip_mat_show_stats(p);
    
    ip_mat_free(b);
    ip_mat_free(p);
    
    p = ip_mat_create(5,5,1,0);
    ip_mat_init_random(p, 50, 10);
    printf("OUTPUT 3:\n");
    print_ip_mat(p);
    
    b = ip_mat_copy(p);
    set_val(p,3,2,0,0);
    
    printf("OUTPUT 4:\n");
    print_ip_mat(p);
    printf("OUTPUT 5:\n");
    print_ip_mat(b);
    
    d = ip_mat_subset(p,1,3,2,3);
    printf("OUTPUT 6:\n");
    print_ip_mat(d);*/
    
    /*p = ip_mat_create(5,5,2,1);
    b = ip_mat_create(5,2,2,3);
    
    d = ip_mat_concat(p,b,1);
    
    printf("OUTPUT 7:\n");
    print_ip_mat(d);*/
    
    
    /*p = ip_mat_create(5,5,1,1);
    b = ip_mat_create(5,5,1,3);
    
    d = ip_mat_sum(p,b);
    
    printf("OUTPUT 8:\n");
    print_ip_mat(d);
    
    d = ip_mat_sub(p,b);
    
    printf("OUTPUT 9:\n");
    print_ip_mat(d);
    
    d = ip_mat_mul_scalar(p,10);
    
    printf("OUTPUT 10:\n");
    print_ip_mat(d);
    
    d = ip_mat_add_scalar(p,9);
    
    printf("OUTPUT 11:\n");
    print_ip_mat(d);
    
    d = ip_mat_mean(p,b);
    
    printf("OUTPUT 12:\n");
    print_ip_mat(d);*/
    
    
    
    Bitmap *imagine_input1,*imagine_input2, *imagine_output;
    ip_mat *ii_c1, *ii_c2, *output;
    
    /*imagine_input1 = bm_load("flower.bmp");
    ii_c1 = bitmap_to_ip_mat(imagine_input1);
    
    output = ip_mat_brighten(ii_c1,50);
    
    imagine_output = ip_mat_to_bitmap(output);
    bm_save(imagine_output,"filter_test_br.bmp");*/
    
    
    /*imagine_input1 = bm_load("flower2.bmp");
    imagine_input2 = bm_load("mongolfiere.bmp");
    ii_c1 = bitmap_to_ip_mat(imagine_input1);
    ii_c2 = bitmap_to_ip_mat(imagine_input2);
    
    output = ip_mat_blend(ii_c1,ii_c2,.5);
    
    imagine_output = ip_mat_to_bitmap(output);
    bm_save(imagine_output,"filter_test_blend.bmp");*/
    
    
    /*imagine_input1 = bm_load("flower2.bmp");
    ii_c1 = bitmap_to_ip_mat(imagine_input1);
    
    output = ip_mat_corrupt(ii_c1,100);
    clamp(output, 0, 255);
    
    imagine_output = ip_mat_to_bitmap(output);
    bm_save(imagine_output,"filter_test_corrupt.bmp");*/
    

    /*imagine_input1 = bm_load("flower2.bmp");
    ii_c1 = bitmap_to_ip_mat(imagine_input1);
    
    output = ip_mat_brighten(ii_c1,50);
    output = ip_mat_padding(output, 40,40);
    clamp(output, 0, 255);
    
    imagine_output = ip_mat_to_bitmap(output);
    bm_save(imagine_output,"filter_test_bi_padding.bmp");*/
    
    
    /*p = create_sharpen_filter();
    printf("OUTPUT 13:\n");
    print_ip_mat(p);
    p = create_edge_filter();
    printf("OUTPUT 14:\n");
    print_ip_mat(p);
    p = create_emboss_filter();
    printf("OUTPUT 15:\n");
    print_ip_mat(p);
    p = create_average_filter(3,3,3);
    printf("OUTPUT 16:\n");
    print_ip_mat(p);*/
    p = create_gaussian_filter(3,3,3,.10);
    printf("OUTPUT 17:\n");
    print_ip_mat(p);
    
    
    /*imagine_input1 = bm_load("flower.bmp");
    ii_c1 = bitmap_to_ip_mat(imagine_input1);
    
    output = ip_mat_convolve(ii_c1,create_edge_filter());
    clamp(output, 0, 255);
    
    imagine_output = ip_mat_to_bitmap(output);
    bm_save(imagine_output,"adv_filter_test.bmp");*/
    
    
    return 0;
}

/*rescale(ip_map_imm_p_elab,255);*/
/*clamp(output, 0, 255);*/

























