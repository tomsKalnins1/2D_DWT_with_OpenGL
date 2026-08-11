#version 460 core

layout(local_size_x = NUM_INV, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D dwt;
layout(rgba32f, binding = 1) uniform image2D LL;
layout(rgba32f, binding = 2) uniform image2D LH;
layout(rgba32f, binding = 3) uniform image2D HL;
layout(rgba32f, binding = 4) uniform image2D HH;



void synchronize()
{

    memoryBarrierShared();
    barrier();

}



void main()
{
   ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 w_id = ivec2(gl_LocalInvocationID.xy);

    vec4 pix_sum_l = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_sum_r = vec4(0.0, 0.0, 0.0, 1.0);

    pix_sum_l += imageLoad(LL, ivec2(th_id.x, th_id.y));
    pix_sum_l += imageLoad(LH, ivec2(th_id.x, th_id.y));
    pix_sum_l += imageLoad(HL, ivec2(th_id.x, th_id.y));
    pix_sum_l += imageLoad(HH, ivec2(th_id.x, th_id.y));

    pix_sum_r += imageLoad(LL, ivec2(th_id.x + NUM_INV, th_id.y));
    pix_sum_r += imageLoad(LH, ivec2(th_id.x + NUM_INV, th_id.y));
    pix_sum_r += imageLoad(HL, ivec2(th_id.x + NUM_INV, th_id.y));
    pix_sum_r += imageLoad(HH, ivec2(th_id.x + NUM_INV, th_id.y));

    imageStore(dwt, ivec2(th_id.x, th_id.y), pix_sum_l);
    imageStore(dwt, ivec2(th_id.x + NUM_INV, th_id.y), pix_sum_r);

}