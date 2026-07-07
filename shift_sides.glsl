#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D image_O; 
layout(rgba32f, binding = 1) uniform image2D image_T;

shared vec4 transpose_a[256] ;
shared vec4 real_imag_buffer[256] ;

uint num_samples = 256;





//-------------------------------------------------------------------------SYNCHRONIZE DATA ACCESS
void synchronize()
{

    barrier();
    memoryBarrierShared();
}


void main()
{
     ivec2 t_id = ivec2(gl_GlobalInvocationID.xy);
     ivec2 w = ivec2(gl_LocalInvocationID.xy);

     vec4 pix_l = imageLoad(image_O, ivec2(w.x,255 -  t_id.y));
     vec4 pix_r = imageLoad(image_O, ivec2(w.x + 128,255 -  t_id.y));

     imageStore(image_T, ivec2(w.x, 255 - t_id.y), pix_r);
     imageStore(image_T, ivec2(w.x + 128, 255 - t_id.y), pix_l);




}