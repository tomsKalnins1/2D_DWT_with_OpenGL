#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D noise_0;
layout(rgba32f, binding = 1) uniform image2D noise_1;
layout(rgba32f, binding = 2) uniform image2D NRNG;


const float pi = 3.141592653589793238462643383279502884197169399375;

uint state_0 = 179;
uint state_1 = 123;

float rand(vec2 co){
    ivec2 st = ivec2(gl_GlobalInvocationID.xy);

    vec2 st_f = vec2(st);
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
    //return (sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


uint xor_shift_0(){
    ivec2 st = ivec2(gl_GlobalInvocationID.xy);
    uint x = state_0;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << (st.x + 1);
    state_0 = x;

    return x;

}

uint xor_shift_1(){
    ivec2 st = ivec2(gl_GlobalInvocationID.xy);
    uint x = state_1;
    x ^= x << 17;
    x ^= x >> 19;
    x ^= x << (st.y + 1);
    state_1 = x;
    return x;

}





void main()
{

    ivec2 st = ivec2(gl_GlobalInvocationID.xy);

    vec4 n_0 = imageLoad(noise_0, st);
    vec4 n_1 = imageLoad(noise_1, st);

    

   float pix_NRNG = sqrt((-2.0) * log(n_0.x)) * cos(2.0 * pi * n_1.x * 30.0);
   
   imageStore(NRNG, ivec2(st.x, st.y), vec4(pix_NRNG, pix_NRNG, pix_NRNG, 1.0));
   

}