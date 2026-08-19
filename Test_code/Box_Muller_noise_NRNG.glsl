#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D noise_0;
layout(rgba32f, binding = 1) uniform image2D noise_1;
layout(rgba32f, binding = 2) uniform image2D NRNG;


const float pi = 3.141592653589793238462643383279502884197169399375;


void main()
{

    ivec2 st = ivec2(gl_GlobalInvocationID.xy);

    vec4 n_0 = imageLoad(noise_0, st);
    vec4 n_1 = imageLoad(noise_1, st);

    

   float pix_NRNG = sqrt((-2.0) * log(n_0.x)) * cos(2.0 * pi * n_1.x);
   
   imageStore(NRNG, ivec2(st.x, st.y), vec4(pix_NRNG, pix_NRNG, pix_NRNG, 1.0));
   

}