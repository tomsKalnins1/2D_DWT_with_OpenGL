#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D dwt_mat;
layout(rgba32f, binding = 2) uniform image2D image_T;



void synchronize()
{

    memoryBarrierShared();
    barrier();

}

void transform()
{

    ivec2 texC_g = ivec2(gl_GlobalInvocationID.xy);
    ivec2 w_id = ivec2(gl_WorkGroupID.xy);


    float res_low = 0.0;
    float res_high = 0.0;
    vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_wt_low = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_wt_high = vec4(0.0, 0.0, 0.0, 1.0);


    for (int k = 0; k < 4; k++)
    {
        pix = imageLoad(image_O, ivec2((w_id.x * 2 + k) % 256, 255 - texC_g.y));
        pix_wt_low = imageLoad(dwt_mat, ivec2((w_id.x * 2 + k) % 256, 255 - w_id.x));
        res_low += pix_wt_low.x * pix.x;
    }
    synchronize();
    for (int k = 0; k < 4; k++)
    {
        pix = imageLoad(image_O, ivec2((w_id.x * 2 + k) % 256, 255 - texC_g.y));
        pix_wt_high = imageLoad(dwt_mat, ivec2((w_id.x * 2 + k) % 256, 127 - w_id.x));
        res_high += pix_wt_high.x * pix.x;
    }
    synchronize();
    vec4 t_0 = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 t_1 = vec4(0.0, 1.0, 0.0, 1.0);
 //   imageStore(image_T, ivec2(w_id.x, 255 - texC_g.y), t_1);
  //  imageStore(image_T, ivec2(w_id.x + 128, 255 - texC_g.y), t_0);
        imageStore(image_T, ivec2(w_id.x, 255 - texC_g.y), vec4(res_low, res_low, res_low, 1.0));
        imageStore(image_T, ivec2(w_id.x + 128, 255 - texC_g.y), vec4(res_high, res_high, res_high, 1.0));
      
    synchronize();

    //  imageStore(image_T, ivec2(texC_g.x, texC_g.y), vec4(res, res, res, 1.0));


}

void main()
{
    ivec2 texC_g = ivec2(gl_GlobalInvocationID.xy);
    transform();
    ivec2 x = ivec2(gl_LocalInvocationID.xy);
    ivec2 w_id = ivec2(gl_WorkGroupID.xy);

}