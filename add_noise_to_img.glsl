#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image;
layout(rgba32f, binding = 1) uniform image2D noise;
layout(rgba32f, binding = 2) uniform image2D output_n;



void synchronize()
{

    memoryBarrierShared();
    barrier();

}



void main()
{
   ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 w_id = ivec2(gl_LocalInvocationID.xy);

    vec4 img_p = imageLoad(image, ivec2(th_id));
    vec4 noise_p = imageLoad(noise, ivec2(th_id)) / 30.0;

    vec4 sum = img_p + noise_p;
        
    imageStore(output_n, ivec2(th_id), sum);


   


}