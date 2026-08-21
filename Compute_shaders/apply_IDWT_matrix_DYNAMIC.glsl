#version 460 core

layout(local_size_x = NUM_INV, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D subband;
layout(rgba32f, binding = 1) uniform image2D subband_buffer;



const float FILTER[SIZE] = { COEFFS };


void synchronize(){

    memoryBarrierImage();
    barrier();

}

void store_back_to_input(){

     ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
        
     vec4 pix_l = imageLoad(subband_buffer, ivec2(th_id.x, th_id.y));
     vec4 pix_r = imageLoad(subband_buffer, ivec2(th_id.x + NUM_INV, th_id.y));

     imageStore(subband, ivec2(th_id.x, th_id.y), pix_l);
     imageStore(subband, ivec2(th_id.x + NUM_INV, th_id.y), pix_r);
     
       
}

void convolve_subband(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

   float pix_low = 0.0;
   float pix_low_p = 0.0;
   float pix_high = 0.0;

   vec4 pix_low_0 = vec4(0.0);

   vec4 pix_low_1 = vec4(0.0);

        for(int i = SIZE - 1; i >= 0; i--){
        /*
        if it is db2 with 4 coefcients then each index of i added to the pixel index has shift of 3 which seems consistent with
        the correction phase shift with z^(-3) for the db2 filter, so I think the additional shift added to the index just starts the convolution
        at a different place, but has no effect on the actual output    int i_0 = ((th_id.x * 2) + i + some_conv_offset) & MASK; (?)
        */
           int i_0 = ((th_id.x * 2) + i) & MASK;
           int i_0_p = ((th_id.x * 2 + 1) + i) & MASK;

           pix_low += imageLoad(subband, ivec2(i_0, th_id.y)).x * FILTER[( SIZE - 1) - i];
           pix_low_p += imageLoad(subband, ivec2(i_0_p, th_id.y)).x * FILTER[(SIZE - 1 ) - i];

        }

   pix_low_0 = vec4(pix_low, pix_low, pix_low, 1.0);
       synchronize();
   pix_low_1 = vec4(pix_low_p, pix_low_p, pix_low_p, 1.0);
       synchronize();
   imageStore(subband_buffer, ivec2(th_id.x * 2, th_id.y), pix_low_0 );
    synchronize();
   imageStore(subband_buffer, ivec2(th_id.x * 2 + 1, th_id.y), pix_low_1);
    synchronize();

}


void main(){
    convolve_subband();
    store_back_to_input();

}