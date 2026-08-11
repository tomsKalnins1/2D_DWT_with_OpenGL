#version 460 core

layout(local_size_x = NUM_INV, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D subband;
layout(rgba32f, binding = 1) uniform image2D subband_buffer;




const float h0[4] = { 0.4829629131, 0.8365163037, 0.224143868, -0.1294095226 };
const float h1[4] = { 0.1294095226, 0.224143868, -0.8365163037, 0.4829629131 };

const float g0[4] = {
    -0.1294095226,
     0.224143868,
     0.8365163037,
     0.4829629131
};

const float g1[4] = {
     0.4829629131,
    -0.8365163037,
     0.224143868,
     0.1294095226
};

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

   int i_0 = ((th_id.x * 2) + 2) & MASK;
   int i_1 = ((th_id.x * 2) + 1) & MASK;
   int i_2 = ((th_id.x * 2) + 0) & MASK;
   int i_3 = ((th_id.x * 2) - 1) & MASK;

   int i_0_p = ((th_id.x * 2 + 1) + 2) & MASK;
   int i_1_p = ((th_id.x * 2 + 1) + 1) & MASK;
   int i_2_p = ((th_id.x * 2 + 1) + 0) & MASK;
   int i_3_p = ((th_id.x * 2 + 1) - 1) & MASK;

   float pix_low = 0.0;
   float pix_low_p = 0.0;
   float pix_high = 0.0;

   vec4 pix_low_0 = vec4(0.0);

   vec4 pix_low_1 = vec4(0.0);
    

        pix_low += imageLoad(subband, ivec2(i_0, th_id.y)).x * FILTER[0];
        synchronize();
        pix_low += imageLoad(subband, ivec2(i_1, th_id.y)).x * FILTER[1];
        synchronize();
        pix_low += imageLoad(subband, ivec2(i_2, th_id.y)).x * FILTER[2];
        synchronize();
        pix_low += imageLoad(subband, ivec2(i_3, th_id.y)).x * FILTER[3];

        synchronize();
   
        pix_low_p += imageLoad(subband, ivec2(i_0_p, th_id.y)).x * FILTER[0];
        synchronize();
        pix_low_p += imageLoad(subband, ivec2(i_1_p, th_id.y)).x * FILTER[1];
        synchronize();
        pix_low_p += imageLoad(subband, ivec2(i_2_p, th_id.y)).x * FILTER[2];
        synchronize();
        pix_low_p += imageLoad(subband, ivec2(i_3_p, th_id.y)).x * FILTER[3];
        synchronize();

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