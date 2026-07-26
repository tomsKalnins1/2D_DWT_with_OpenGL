#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D image_T;

shared float input_b[256];

void synchronize(){

    memoryBarrierShared();
    barrier();

}
void store_all_from_img_to_arr(){
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    int th_ind_l = th_id.x;
    int th_ind_r = th_id.x + 128;

    vec4 pix_l = imageLoad(image_O, ivec2(th_id));
    vec4 pix_r = imageLoad(image_O, ivec2(th_id.x + 128, th_id.y));

    input_b[th_ind_l] = pix_l.x;
    input_b[th_ind_r] = pix_r.x;
}

void store_all_from_arr_to_img(){
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    int th_ind_l = th_id.x;
    int th_ind_r = th_id.x + 128;

    float pix_l_val = input_b[th_ind_l];
    float pix_r_val = input_b[th_ind_r];

    vec4 pix_l = vec4(pix_l_val, pix_l_val, pix_l_val, 1.0);
    vec4 pix_r = vec4(pix_r_val, pix_r_val, pix_r_val, 1.0);
    
    imageStore(image_T, ivec2(th_id), pix_l);
    imageStore(image_T, ivec2(th_id.x + 128, th_id.y), pix_r);

}

void swap_val(int left, int right){
    float temp = input_b[left];
    input_b[left] = input_b[right];
    input_b[right] = temp;
}
/*

void bitonic_one_dir(int start, int end, int dir){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    int th_ind = th_id.x;

    vec4 pix_l = imageLoad(image_O, ivec2(th_id));
    vec4 pix_r = vec4(0.0, 0.0, 0.0, 1.0);

    int counter = 0;
    int s = end - start;

    for(int j = s/2; j > 0; j /= 2 ){
        
        int block = ( th_ind + j ) / (j * 2);
        int offset = (th_ind + j) % (j * 2);
        int left = block * (j * 2) + offset;
        int right = left + j;

        if(input_b[left] > input_b[right] && dir == 1){
            
            swap_val(left, right);
        
        }
        if(input_b[left] < input_b[right] && dir == 0){
            swap_val(left, right);
        }
        
        synchronize();
    
    }
    */

    void bitonic_sort_1(int start, int end){
        ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
        int th_ind = th_id.x;
        int s = end - start;

        for(int k = 2; k <= s; k *= 2){
            
            for(int j = k/2; j > 0; j /= 2){
                
                int num_blocks = s / (j * 2);

                int block = (th_ind * num_blocks) / (s/2);
                int offset = (th_ind) % j;
                int left = block * (j * 2) + offset;
                int right = left ^ j;
                //odd up_down has 1 at the end and even has 0 at the end ANDing then with one is better 
                //as one and operation is simpler that modulus
                int up_down = (left/k) & 1;

                if(up_down == 1){
                
                    if(input_b[left] < input_b[right]){
                        
                        swap_val(left, right);
                    
                    }
                }else{
                    
                    if(input_b[left] > input_b[right]){
                        swap_val(left, right);
                    }
                
                }
                synchronize();
         
            }
        
        }

        


    

}
void put_from_img_to_arr(){

     ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
     
     for(int i = 0; i < 256; i++){
        
        vec4 pix = imageLoad(image_O, ivec2(i, th_id.y));
        float pix_val = pix.x;

        input_b[i] = pix_val;
     
     }

}

void put_from_arr_to_img(){

     ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
     
     for(int i = 0; i < 256; i++){
        
        float pix_val = input_b[i];
        vec4 pix = vec4(pix_val, pix_val, pix_val, 1.0);
        imageStore(image_T, ivec2(i, th_id.y), pix);
        

        input_b[i] = pix_val;
     
     }

}

void bitonic_seq(int start, int end, int dir){

    int s = end - start + 1;

    if(dir == 1){

        int counter = 0;

        for(int j = s/2; j > 0; j /= 2){

            counter = 0;

            for(int i = start; i + j <= end; i++){

                if(counter < j){

                    if(input_b[i] > input_b[i + j]){
                        swap_val(i, i + j);
                        counter++;
                    }

                }else{

                    counter = 0;
                    i = i + j - 1;

                }
            }
        }

    }else{

        int counter = 0;

        for(int j = s/2; j > 0; j /= 2){

            counter = 0;

            for(int i = start; i + j <= end; i++){

                if(counter < j){

                    if(input_b[i] < input_b[i + j]){
                        swap_val(i, i + j);
                    }

                }else{

                    counter = 0;
                    i = i + j - 1;

                }
            }
        }
    }
}

void bitonic_sort(int start, int end){
    
    int s = end - start;

    for(int k =2; k < s; k *= 2 ){
        
        for(int i = 0; i < s; i = i + k){
            
            if( ( ( i/k ) % 2 ) == 0){
                
                bitonic_seq(i, i + k - 1, 1);

            }else{
                
                bitonic_seq(i, i + k - 1, 0);
            
            }
        
        }
    
    }

}








void main()
{
put_from_img_to_arr();
bitonic_sort_1(0, 256);
//bitonic_seq(0, 255, 1);
synchronize();
put_from_arr_to_img();



        
 
   


}