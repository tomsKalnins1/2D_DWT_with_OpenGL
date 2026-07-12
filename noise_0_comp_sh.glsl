#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image_O;

uint state = 179;

float rand(vec2 co){
    //return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
    return abs((sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453));
}


uint xor_shift(){
    ivec2 st = ivec2(gl_GlobalInvocationID.xy);
    uint x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;

}





void main()
{
  //  ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 st = ivec2(gl_GlobalInvocationID.xy);
  

    state = state * (st.x + 255);
    vec2 st_f = vec2(st);
    
    float r = rand(st_f);
    state = state * uint(r);
    uint rand_i = xor_shift();
    rand_i = rand_i % 256;
    float rand_f = float(rand_i) / 255.0 ;

    vec4 pix = vec4(rand_f, rand_f, rand_f, 1.0);
    imageStore(image_O, ivec2(st.x, st.y), pix);


   


}