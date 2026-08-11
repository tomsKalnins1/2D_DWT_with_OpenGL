#version 460 core

layout(local_size_x = NUM_INV, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D DWT_coeffs;
layout(rgba32f, binding = 1) uniform image2D sorted_HH;



shared float scale_parameter = 1.0; // this is at level 2 decomposition


shared float local_medians[NUM_INV];
shared float local_deviation_noise[NUM_INV];

shared float local_diviation_signal[NUM_INV];
shared float local_Tn[NUM_INV];

shared float local_means[NUM_INV];

const float scale_parameter_beta = SCALE_PARAM; // this is at level 2 decomposition



void synchronize(){

    memoryBarrierShared();
    barrier();

}

void get_local_medians(int L_H_0, int L_H_1){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    vec4 pix_l = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_r = vec4(0.0, 0.0, 0.0, 1.0);

    //HH subband
    if(L_H_0 == 1 && L_H_1 == 1 ){

       pix_l = imageLoad(sorted_HH, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + ( PARTITION_SIZE / 2 ) - 1, th_id.y + (SUBBAND_SIZE / 2)));
       pix_r = imageLoad(sorted_HH, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + ( PARTITION_SIZE / 2 ), th_id.y + (SUBBAND_SIZE / 2)));
       float median = (pix_l.x + pix_r.x) / 2.0;
       local_medians[th_id.x] = median;

    }

    
    //LH subband
    if(L_H_0 == 0 && L_H_1 == 1 ){

       pix_l = imageLoad(sorted_HH, ivec2(th_id.x * PARTITION_SIZE + ( PARTITION_SIZE / 2 ) - 1, th_id.y + (SUBBAND_SIZE / 2)));
       pix_r = imageLoad(sorted_HH, ivec2(th_id.x * PARTITION_SIZE + ( PARTITION_SIZE / 2 ), th_id.y + (SUBBAND_SIZE / 2)));
       float median = (pix_l.x + pix_r.x) / 2.0;
       local_medians[th_id.x] = median;

    }

    //HL subband
    if(L_H_0 == 1 && L_H_1 == 0 ){
    
  

       pix_l = imageLoad(sorted_HH, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + ( PARTITION_SIZE / 2 ) - 1, th_id.y));
       pix_r = imageLoad(sorted_HH, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + ( PARTITION_SIZE / 2 ), th_id.y));
       float median = (pix_l.x + pix_r.x) / 2.0;
       local_medians[th_id.x] = median;

    }

}
//VARIANCE NOT DEVIATION!!!!
void store_local_deviation_noise(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    local_deviation_noise[th_id.x] = pow(( local_medians[th_id.x] / 0.6745 ), 2.0);

}

void set_local_means(int L_H_0, int L_H_1){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    //HH subband
    if(L_H_0 == 1 && L_H_1 == 1){
        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
        float avg = 0.0;

        //!!!!!!!! reduce 8 to 4 for lvl 2 ??
        
        for(int i = 0; i < PARTITION_SIZE; i++){
            pix = imageLoad(DWT_coeffs, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + i, th_id.y + (SUBBAND_SIZE / 2)));
            avg += pix.x;
        }
        avg /= float( PARTITION_SIZE );

        local_means[th_id.x] = avg;

    }

    //LH subband
    if(L_H_0 == 0 && L_H_1 == 1 ){
        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
        float avg = 0.0;

        for(int i = 0; i < PARTITION_SIZE; i++){
            pix = imageLoad(DWT_coeffs, ivec2(th_id.x * PARTITION_SIZE + i, th_id.y + (SUBBAND_SIZE / 2)));
            avg += pix.x;
        }
        avg /= float( PARTITION_SIZE );

        local_means[th_id.x] = avg;

    }

    //HL subband
    if(L_H_0 == 1 && L_H_1 == 0 ){
        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
        float avg = 0.0;

        for(int i = 0; i < PARTITION_SIZE; i++){
            pix = imageLoad(DWT_coeffs, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + i, th_id.y));
            avg += pix.x;
        }

         avg /= float( PARTITION_SIZE );

        local_means[th_id.x] = avg;

    }
   
}

void set_local_diviation_signal(int L_H_0, int L_H_1){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    //HH subband
    if(L_H_0 == 1 && L_H_1 == 1 ){

        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
        float sum = 0.0;

        for(int i = 0; i < PARTITION_SIZE; i++){
            pix = imageLoad(DWT_coeffs, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + i, th_id.y + (SUBBAND_SIZE / 2)));
            sum += pow((pix.x - local_means[th_id.x]), 2.0);
        }

        sum /= float( PARTITION_SIZE );

        local_diviation_signal[th_id.x] = sqrt(sum);

    }

    //LH subband
    if(L_H_0 == 0 && L_H_1 == 1 ){

        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
        float sum = 0.0;

        for(int i = 0; i < PARTITION_SIZE; i++){
            pix = imageLoad(DWT_coeffs, ivec2(th_id.x * PARTITION_SIZE + i, th_id.y + (SUBBAND_SIZE / 2)));
            sum += pow((pix.x - local_means[th_id.x]), 2.0);
        }

        sum /=  float( PARTITION_SIZE );

        local_diviation_signal[th_id.x] = sqrt(sum);

    }

    //HL subband
    if(L_H_0 == 1 && L_H_1 == 0 ){

        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
        float sum = 0.0;

        for(int i = 0; i < PARTITION_SIZE; i++){
            pix = imageLoad(DWT_coeffs, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + i, th_id.y));
            sum += pow((pix.x - local_means[th_id.x]), 2.0);
        }

        sum /=  float( PARTITION_SIZE );

        local_diviation_signal[th_id.x] = sqrt(sum);

    }

}

void set_local_Tn(){

   ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

   local_Tn[th_id.x] = ( (scale_parameter_beta / 1.0) * local_deviation_noise[th_id.x] ) /  local_diviation_signal[th_id.x];

}

void apply_Tn(int L_H_0, int L_H_1){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    //HH subband
    if(L_H_0 == 1 && L_H_1 == 1){
        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);

        for(int i = 0; i < PARTITION_SIZE; i++){
            
            pix = imageLoad(DWT_coeffs, ivec2((SUBBAND_SIZE / 2) + th_id.x * PARTITION_SIZE + i, th_id.y + SUBBAND_SIZE / 2));
            float sign = 1.0;
            if(pix.x < 0){
                sign = -1.0;
            }

            float new_val = sign * max(0.0, ( abs(pix.x) - local_Tn[th_id.x]));

    



            vec4 pix_Tn = vec4(new_val, new_val, new_val, 1.0);
            if(local_deviation_noise[th_id.x] > 1.0){
                pix_Tn = vec4(1.0, 0.0, 0.0, 1.0);
            }

            imageStore(DWT_coeffs, ivec2(SUBBAND_SIZE / 2 + th_id.x * PARTITION_SIZE + i, th_id.y + SUBBAND_SIZE / 2), pix_Tn);
 
        }
        
    }

    //LH subband
    if(L_H_0 == 0 && L_H_1 == 1){
        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);

        for(int i = 0; i < PARTITION_SIZE; i++){
            
            pix = imageLoad(DWT_coeffs, ivec2(th_id.x * PARTITION_SIZE + i, th_id.y + SUBBAND_SIZE / 2));
            float sign = 1.0;
            if(pix.x < 0){
                sign = -1.0;
            }

            float new_val = sign * max(0.0, ( abs(pix.x) - local_Tn[th_id.x]));

    



            vec4 pix_Tn = vec4(new_val, new_val, new_val, 1.0);
            if(local_deviation_noise[th_id.x] > 1.0){
                pix_Tn = vec4(1.0, 0.0, 0.0, 1.0);
            }

            imageStore(DWT_coeffs, ivec2(th_id.x * PARTITION_SIZE + i, th_id.y + SUBBAND_SIZE / 2), pix_Tn);
 
        }
        
    }

    //HL subband
    if(L_H_0 == 1 && L_H_1 == 0){
        vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);

        for(int i = 0; i < PARTITION_SIZE; i++){
            
            pix = imageLoad(DWT_coeffs, ivec2(SUBBAND_SIZE / 2 + th_id.x * PARTITION_SIZE + i, th_id.y));
            float sign = 1.0;
            if(pix.x < 0){
                sign = -1.0;
            }

            float new_val = sign * max(0.0, ( abs(pix.x) - local_Tn[th_id.x]));

    



            vec4 pix_Tn = vec4(new_val, new_val, new_val, 1.0);
     

            imageStore(DWT_coeffs, ivec2(SUBBAND_SIZE / 2 + th_id.x * PARTITION_SIZE + i, th_id.y), pix_Tn);
 
        }
        
    }


}

/*
void draw_Tn(int th_x, int th_y){

     ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

     if(th_id.x == th_x && th_id.y == th_y ){
        
        float Tn = local_deviation_noise[th_id.x];
        uint Tn_bits =  floatBitsToInt(Tn);

        uint pix_val = 0; 

        for(int i = 0; i < 32; i++){
            pix_val = ( Tn_bits >> i ) & 1;
            float pix_val_fl = float(pix_val);
            vec4 pix = vec4(pix_val_fl, pix_val_fl, pix_val_fl, 1.0);
            if(pix_val == 0){
                
                pix = vec4(0.0, 0.0, 1.0, 1.0);        

            }

            

            imageStore(Tn_value, ivec2(i, 255), pix);
        }
     
     }

}
*/

void main(){

    get_local_medians(1, 1);
    synchronize();
    store_local_deviation_noise();
    synchronize();
    set_local_means(1, 1);
    synchronize();
    set_local_diviation_signal(1, 1);
    synchronize();
    set_local_Tn();
    synchronize();

    apply_Tn(1, 1);
    synchronize();
    /*
    apply_Tn(0, 1);
    synchronize();

  

    apply_Tn(1, 0);
    synchronize();
    */
    
   // apply_Tn(1, 1);
   // synchronize();




}