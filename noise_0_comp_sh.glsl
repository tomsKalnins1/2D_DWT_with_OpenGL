#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image_O;

const float pi = 3.141592653589793238462643383279502884197169399375;
uint state_0 = 179;
uint state_1 = 123;

float rand(vec2 co){
    //return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
    return fract((sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453));
}


uint xor_shift_0(){
    ivec2 st = ivec2(gl_GlobalInvocationID.xy);
    uint x = state_0;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state_0 = x;
    return x;

}

uint xor_shift_1(){
    ivec2 st = ivec2(gl_GlobalInvocationID.xy);
    uint x = state_1;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state_1 = x;
    return x;

}





void main()
{
  //  ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    /*
    
    vec2 st_f = vec2(st);    

    float r = rand(st_f);

    uint rand_i = xor_shift_0();
    rand_i = ((rand_i  % 256) + 256) % 256;
    float rand_f = float(rand_i)/255.0;
    rand_f = rand_f * abs(r);
    
    vec2 st_f_1 = vec2(st);    

    float r_1 = rand(st_f_1);

    uint rand_i_1 = xor_shift_1();
    rand_i_1 = ((rand_i_1 % 256) + 256) % 256;
    float rand_f_1 = float(rand_i_1) / 43758.0;
    //float rand_f_1 = float(rand_i_1)/255.0;
    rand_f_1 = rand_f_1 * abs(r_1);
    */
    ivec2 st = ivec2(gl_GlobalInvocationID.xy);
    
    vec2 st_f = vec2(st);
    float st_f_rand = rand(st_f);
    state_0 = state_0 * st.x;
    uint xor_i = xor_shift_0();
    float xor_f_norm = float(xor_i);
    float rand_f = xor_f_norm * st_f_rand;

    uint xor_i_1 = xor_shift_1();
    state_1 = state_1 * st.x;
    float xor_f_norm_1 = float(xor_i_1);
    float rand_f_1 = xor_f_norm_1 * st_f_rand;

    float pix_NRNG = sqrt((-2.0) * log(rand_f_1)) * cos(2.0 * pi * rand_f_1); // the white squares remain even if I divide the pix_NRNG by 1000.0
    if(pix_NRNG <= 0.0){
        imageStore(image_O, ivec2(st.x, st.y), vec4(1.0, 0.0, 0.0, 1.0));
    }else{
    vec4 pix = vec4( pix_NRNG, pix_NRNG, pix_NRNG, 1.0);
    imageStore(image_O, ivec2(st.x, st.y), pix);
    }


   


}