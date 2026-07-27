#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D DWT_coeffs;
layout(rgba32f, binding = 1) uniform image2D sorted_HH;

shared float medians_of_rows[128];
shared float median_of_HH = 0.0;
shared float variance_of_noise = 0.0;
shared float scale_parameter = 3.741; // this is at level 1 decomposition
shared float mean_vals_of_rows[128];
shared float global_mean_val = 0.0;


shared float radicands_for_full_std[128];
shared float Tn_threshold = 0.0;

shared float standard_dev_HH = 0.0;


void synchronize(){

    memoryBarrierShared();
    barrier();

}


void swap_val(int left, int right){
    float temp = medians_of_rows[left];
    medians_of_rows[left] = medians_of_rows[right];
    medians_of_rows[right] = temp;
}


void store_medians(){

        ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
        vec4 pix_l = imageLoad(sorted_HH, ivec2(th_id.x + 128 + (128/2 - 1), th_id.x + 128));
        vec4 pix_r = imageLoad(sorted_HH, ivec2(th_id.x + 128 + (128/2), th_id.x + 128));
        
        float avg_pix_l_pix_r = (pix_l.x + pix_r.x) / 2.0;

        medians_of_rows[th_id.x] = avg_pix_l_pix_r;
        
}

void bitonic_sort(int start, int end){
        ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
        if(th_id.x < 64){
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
                
                    if(medians_of_rows[left] < medians_of_rows[right]){
                        
                        swap_val(left, right);
                    
                    }
                }else{
                    
                    if(medians_of_rows[left] > medians_of_rows[right]){
                        swap_val(left, right);
                    }
                
                }
                synchronize();
         
            }
        
            }
        }

}

void set_subband_median(){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    if(th_id.x == 0){
        
        float mid_left = medians_of_rows[128 / 2 - 1];
        float mid_right = medians_of_rows[128 / 2];

        median_of_HH = (mid_left + mid_right) / 2.0;

    }   

}

void set_variance_of_noise(){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    if(th_id.x == 0){
            
      //variance_of_noise =  pow(( median_of_HH / 0.6745 ), 2.0 );
        variance_of_noise =  pow(( median_of_HH / 0.6745 ), 2.0 );

    }  

}


void set_mean_val_row(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    float sum = 0.0;

    for(int i = 0; i < 128; i++){
        
        vec4 pix = imageLoad(sorted_HH, ivec2(th_id.x + 128 + i, th_id.x + 128));
        sum += pix.x;
    }

    mean_vals_of_rows[th_id.x] = sum / 128.0;
    
}

void set_global_mean(){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    float sum = 0.0;

    if(th_id.x == 0){
        for(int i = 0; i < 128; i++){
            sum += mean_vals_of_rows[i];
        }
        global_mean_val = sum / 128.0;

    }

}

void set_radicands_for_full_std(){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    float sum = 0.0;
    for(int i = 0; i < 128; i++){
        
        vec4 pix = imageLoad(DWT_coeffs, ivec2(th_id.x + 128 + i, th_id.x + 128));
        sum += pow(pix.x - global_mean_val, 2.0);
    
    }

    radicands_for_full_std[th_id.x] = sum / 128.0;

}


void set_standard_dev_HH(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    if(th_id.x == 0){
        float sum = 0.0;

        for(int i = 0; i < 128; i++){

            sum +=  radicands_for_full_std[i];    
        
        }

        //standard_dev_HH = sqrt(sum);
        standard_dev_HH = 1.0;
    }
}

void set_Tn_threshold(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    
    if(th_id.x == 0){
        
        Tn_threshold = scale_parameter * variance_of_noise / (standard_dev_HH);
       // Tn_threshold = scale_parameter * 500.0 / (standard_dev_HH);
    
    }

}

void apply_threshold(){

   ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

   for(int i = 0; i < 128; i++){
    
        vec4 pix = imageLoad(DWT_coeffs, ivec2(th_id.x + 128 + i, th_id.x + 128));
        float pix_x = pix.x;
        float new_pix = 0.0;
        float sign = 0.0;

        if(pix_x < 0){
            sign = -1.0;
        }else{
            sign = 1.0;
        }
        
        new_pix = sign * max(0.0, abs(pix_x) - Tn_threshold);
        vec4 soft_th = vec4(new_pix, new_pix, new_pix, 1.0);
        vec4 test = vec4(pix_x, pix_x, pix_x, 1.0);
        if(abs(pix_x) < 0.5){
            test = vec4(0.0, 0.0, 0.0, 1.0);
        } 
        imageStore(DWT_coeffs, ivec2(th_id.x + 128 + i, th_id.x + 128), soft_th);

   }
    
}

void main(){

    store_medians();
    synchronize();
    bitonic_sort(0, 128);
    synchronize();
    
    set_subband_median();
    synchronize();
    set_variance_of_noise();
    synchronize();
    set_mean_val_row();
    synchronize();
    set_global_mean();
    synchronize();
    set_radicands_for_full_std();
    synchronize();
    set_standard_dev_HH();
    synchronize();
    set_Tn_threshold();
    synchronize();
    apply_threshold();
    synchronize();
}