#version 460 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

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
    ivec2 texC_g = ivec2(gl_GlobalInvocationID.xy);
    for (int i = 0; i < num_samples / gl_WorkGroupSize.x; i++)
    {

        ivec2 curr_coord = ivec2(texC_g.x + gl_WorkGroupSize.x * i, texC_g.y);

        ivec2 store_to = ivec2(texC_g.y, texC_g.x + gl_WorkGroupSize.x * i);

        vec4 own_pix = imageLoad(image_O, store_to);
        
        transpose_a[texC_g.x + gl_WorkGroupSize.x * i] = own_pix;
        synchronize();

    }
    synchronize();
    for (int i = 0; i < num_samples / gl_WorkGroupSize.x; i++)
    {
        vec4 own_pix = transpose_a[texC_g.x + gl_WorkGroupSize.x * i];

        imageStore(image_T, ivec2(texC_g.x + gl_WorkGroupSize.x * i, texC_g.y), own_pix);
        synchronize();

    }
    synchronize();



}