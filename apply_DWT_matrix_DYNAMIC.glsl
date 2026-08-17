#version 460 core

layout(local_size_x = NUM_INV, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D image_T;
//h0[0.049817, -0.12083, -0.19093, 0.65037, 1.1411, 0.47047]
//h1[0.47047, 1.1411, 0.65037, -0.19093, -0.12083, 0.049817]

//const float h0[4] = { 0.4829629131, 0.8365163037, 0.224143868, -0.1294095226 };
//const float h1[4] = { 0.1294095226, 0.224143868, -0.8365163037, 0.4829629131 }

const float h0[ SIZE ] = { COEFFS_H0 };
const float h1[ SIZE ] = { COEFFS_H1 };



const float g0[6] = {
     0.3326705530, 0.8068915093, 0.4598775021,
    -0.1350110200, -0.0854412739, 0.0352262919
};

const float g1[6] = {
     0.0352262919, 0.0854412739, -0.1350110200,
    -0.4598775021, 0.8068915093, -0.3326705530
};

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

   float pix_low = 0.0;
   float pix_high = 0.0;

   for(int i = 0; i < SIZE; i++){
     int i_0 = (th_id.x * 2 - i + 2) & (WIDTH_IMG - 1);
     pix_low += imageLoad(image_O, ivec2(i_0, th_id.y)).x * h0[i];
     pix_high += imageLoad(image_O, ivec2(i_0, th_id.y)).x * h1[i];
   }

   vec4 pix_low_vec = vec4(pix_low, pix_low, pix_low, 1.0);
   vec4 pix_high_vec = vec4(pix_high, pix_high, pix_high, 1.0);

   imageStore(image_T, ivec2(th_id.x, th_id.y), pix_low_vec);
   imageStore(image_T, ivec2(th_id.x + NUM_INV, th_id.y), pix_high_vec);

   synchronize();

    store_back_to_input();
    synchronize();


}