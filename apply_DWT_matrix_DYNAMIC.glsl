#version 460 core

layout(local_size_x = NUM_INV, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D image_T;

const float h0[4] = { 0.4829629131, 0.8365163037, 0.224143868, -0.1294095226 };
const float h1[4] = { 0.1294095226, 0.224143868, -0.8365163037, 0.4829629131 };

void synchronize()
{

    memoryBarrierImage();
    barrier();

}

void put_back_to_image_O(){
       
       ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

 

}

void store_back_to_input(){

     ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
        
     vec4 pix_l = imageLoad(image_T, ivec2(th_id.x, th_id.y));
     vec4 pix_r = imageLoad(image_T, ivec2(th_id.x + NUM_INV, th_id.y));

     imageStore(image_O, ivec2(th_id.x, th_id.y), pix_l);
     imageStore(image_O, ivec2(th_id.x + NUM_INV, th_id.y), pix_r);
     
       
}



void main()
{
   ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
   ivec2 w_id = ivec2(gl_LocalInvocationID.xy);

   int i_0 = (th_id.x * 2 + 0) & (WIDTH_IMG - 1);
   int i_1 = (th_id.x * 2 - 1) & (WIDTH_IMG - 1);
   int i_2 = (th_id.x * 2 - 2) & (WIDTH_IMG - 1);
   int i_3 = (th_id.x * 2 - 3) & (WIDTH_IMG - 1);

   float pix_low = 0.0;
   float pix_high = 0.0;

   pix_low += imageLoad(image_O, ivec2(i_0, th_id.y)).x * h0[0];
   pix_low += imageLoad(image_O, ivec2(i_1, th_id.y)).x * h0[1];
   pix_low += imageLoad(image_O, ivec2(i_2, th_id.y)).x * h0[2];
   pix_low += imageLoad(image_O, ivec2(i_3, th_id.y)).x * h0[3];

   pix_high += imageLoad(image_O, ivec2(i_0, th_id.y)).x * h1[0];
   pix_high += imageLoad(image_O, ivec2(i_1, th_id.y)).x * h1[1];
   pix_high += imageLoad(image_O, ivec2(i_2, th_id.y)).x * h1[2];
   pix_high += imageLoad(image_O, ivec2(i_3, th_id.y)).x * h1[3];

   vec4 pix_low_vec = vec4(pix_low, pix_low, pix_low, 1.0);
   vec4 pix_high_vec = vec4(pix_high, pix_high, pix_high, 1.0);

   imageStore(image_T, ivec2(th_id.x, th_id.y), pix_low_vec);
   imageStore(image_T, ivec2(th_id.x + NUM_INV, th_id.y), pix_high_vec);

   synchronize();

    store_back_to_input();
    synchronize();
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