#version 460 core

layout(local_size_x = NUM_INV, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D dwt;
layout(rgba32f, binding = 2) uniform image2D image_T;



void synchronize()
{

    memoryBarrierShared();
    barrier();

}



void main()
{
   ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 w_id = ivec2(gl_LocalInvocationID.xy);


    float low_c = 0.0;
    float high_c = 0.0;

    for(int i = 0; i < WIDTH_IMG; i++){

         vec4 img = imageLoad(image_O, ivec2(i,th_id.y));
         vec4 low = imageLoad(dwt, ivec2(i, w_id.x));
         low_c += img.x * low.x;
         vec4 high = imageLoad(dwt, ivec2(i, NUM_INV + w_id.x));
         high_c += img.x * high.x;
    }
    vec4 output_apprx = vec4(low_c, low_c, low_c, 1.0);
    vec4 output_det = vec4(high_c, high_c, high_c, 1.0);
    vec4 test_vec = vec4(1.0, 0.0, 0.0, 1.0);
    imageStore(image_T, ivec2(w_id.x, th_id.y), output_apprx);
    imageStore(image_T, ivec2(w_id.x + NUM_INV, th_id.y), output_det);

}