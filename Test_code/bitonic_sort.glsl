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



void main()
{
store_all_from_img_to_arr();
bitonic_sort_1(0, 256);
//bitonic_seq(0, 255, 1);
synchronize();
store_all_from_arr_to_img();



        
 
   


}