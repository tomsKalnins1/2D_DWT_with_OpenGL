#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;


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

void upsample(int L_H_0, int L_H_1){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);

    //LL
    if(L_H_0 == 0 && L_H_1 == 0 && th_id.y <= 127){
        
        pix = imageLoad(subband, ivec2(th_id.x, th_id.y));
            synchronize();
        imageStore(subband_buffer, ivec2(th_id.x * 2, (th_id.y) * 2), pix);
        synchronize();
     //   imageStore(subband_buffer, ivec2(th_id.x * 2 + 1, (th_id.y) * 2), vec4(0.0, 0.0, 0.0, 1.0));
    //    synchronize();
    
    }
}

void put_back_to_image_O(){
       
       ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

 

}

void convolve_subband(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

   int i_0 = ((th_id.x * 2) + 1) & 255;
   int i_1 = ((th_id.x * 2) + 0) & 255;
   int i_2 = ((th_id.x * 2) - 1) & 255;
   int i_3 = ((th_id.x * 2) - 2) & 255;

   int i_0_p = ((th_id.x * 2 + 1) + 1) & 255;
   int i_1_p = ((th_id.x * 2 + 1) + 0) & 255;
   int i_2_p = ((th_id.x * 2 + 1) - 1) & 255;
   int i_3_p = ((th_id.x * 2 + 1) - 2) & 255;

   float pix_low = 0.0;
   float pix_low_p = 0.0;
   float pix_high = 0.0;

   vec4 pix_low_0 = vec4(0.0);

   vec4 pix_low_1 = vec4(0.0);
    

        pix_low += imageLoad(subband, ivec2(i_0, th_id.y)).x * g0[0];
        synchronize();
        pix_low += imageLoad(subband, ivec2(i_1, th_id.y)).x * g0[1];
        synchronize();
        pix_low += imageLoad(subband, ivec2(i_2, th_id.y)).x * g0[2];
        synchronize();
        pix_low += imageLoad(subband, ivec2(i_3, th_id.y)).x * g0[3];

        synchronize();
   
        pix_low_p += imageLoad(subband, ivec2(i_0_p, th_id.y)).x * g0[0];
        synchronize();
        pix_low_p += imageLoad(subband, ivec2(i_1_p, th_id.y)).x * g0[1];
        synchronize();
        pix_low_p += imageLoad(subband, ivec2(i_2_p, th_id.y)).x * g0[2];
        synchronize();
        pix_low_p += imageLoad(subband, ivec2(i_3_p, th_id.y)).x * g0[3];
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


void main()
{

    convolve_subband();

   /*
   ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
   ivec2 w_id = ivec2(gl_LocalInvocationID.xy);

   int i_0 = ((th_id.x) * 2 + 1) & 127;
   int i_1 = ((th_id.x) * 2 + 0) & 127;
   int i_2 = ((th_id.x) * 2 - 1) & 127;
   int i_3 = ((th_id.x) * 2 - 2) & 127;

   int i_0_h = (((th_id.x + 128 ) * 2 + 1) & 127 ) + 128;
   int i_1_h = (((th_id.x + 128 ) * 2 + 0) & 127 ) + 128;
   int i_2_h = (((th_id.x + 128 ) * 2 - 1) & 127 ) + 128;
   int i_3_h = (((th_id.x + 128 ) * 2 - 2) & 127 ) + 128;

   float pix_low = 0.0;
   float pix_high = 0.0;

   pix_low += imageLoad(image_O, ivec2(i_0, th_id.y)).x * g0[0];
   pix_low += imageLoad(image_O, ivec2(i_1, th_id.y)).x * g0[1];
   pix_low += imageLoad(image_O, ivec2(i_2, th_id.y)).x * g0[2];
   pix_low += imageLoad(image_O, ivec2(i_3, th_id.y)).x * g0[3];

   pix_high += imageLoad(image_O, ivec2(i_0_h, th_id.y)).x * g1[0];
   pix_high += imageLoad(image_O, ivec2(i_1_h, th_id.y)).x * g1[1];
   pix_high += imageLoad(image_O, ivec2(i_2_h, th_id.y)).x * g1[2];
   pix_high += imageLoad(image_O, ivec2(i_3_h, th_id.y)).x * g1[3];

   float synth = pix_high + pix_low;

   vec4 pix_synth = vec4(synth, synth, synth, 1.0);


      imageStore(image_T, ivec2(th_id.x, th_id.y), pix_synth);
*/
   /*

   vec4 pix_low_vec = vec4(pix_low, pix_low, pix_low, 1.0);
   vec4 pix_high_vec = vec4(pix_high, pix_high, pix_high, 1.0);

   imageStore(image_T, ivec2(th_id.x, th_id.y), pix_low_vec);
   imageStore(image_T, ivec2(th_id.x + 128, th_id.y), pix_high_vec);
   */
   /*
   synchronize();

    store_back_to_input();
    synchronize();
    */
    /*
    float low_c = 0.0;
    float high_c = 0.0;

    for(int i = 0; i < WIDTH_IMG; i++){

         vec4 img = imageLoad(image_O, ivec2(i,th_id.y));
         vec4 low = imageLoad(dwt, ivec2(i, th_id.x));
         low_c += img.x * low.x;
         vec4 high = imageLoad(dwt, ivec2(i, NUM_INV + th_id.x));
         high_c += img.x * high.x;
   
    }
    synchronize();
    vec4 output_apprx = vec4(low_c, low_c, low_c, 1.0);
    vec4 output_det = vec4(high_c, high_c, high_c, 1.0);
    vec4 test_vec = vec4(1.0, 0.0, 0.0, 1.0);
    imageStore(image_T, ivec2(th_id.x, th_id.y), output_apprx);

    imageStore(image_T, ivec2(th_id.x + NUM_INV, th_id.y), output_det);

    store_back_to_input();
    synchronize();
*/


}