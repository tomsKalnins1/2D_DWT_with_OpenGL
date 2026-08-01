#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image_O;

const float pi = 3.141592653589793238462643383279502884197169399375;

uint state_0 = 179;
uint state_1 = 123;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
    //return (sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
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
    x ^= x << 17;
    x ^= x >> 19;
    x ^= x << 5;
    state_1 = x;
    return x;

}





void main()
{

    ivec2 st = ivec2(gl_GlobalInvocationID.xy);

    vec2 st_f = vec2(st);

   float rand_0 = rand(st_f);


    uint xor_i_0 = xor_shift_0() % 256;
  
    uint xor_i_1  = xor_shift_1() % 256;
  

    float xor_f_0 = float(xor_i_0)/255.0;
    float xor_f_1 = float(xor_i_1);

    vec2 st_0 = vec2(st_f.x / 255.0, st_f.y/255.0);

    float rnd = rand( st_0 );

    vec4 p = vec4(vec3(rnd),1.0);

   // float pix_NRNG = sqrt((-2.0) * log(p)) * cos(2.0 * pi * p * 10.0); 
    imageStore(image_O, ivec2(st.x, st.y), p);
   
   /*
   if( xor_f_0 < 0.0){

        imageStore(image_O, ivec2(st.x, st.y), vec4(1.0, 0.0, 0.0, 1.0));
    
    }else if(xor_f_0 >= 1.0){
    imageStore(image_O, ivec2(st.x, st.y), vec4(0.0, 1.0, 0.0, 1.0));
    }else{
        vec4 pix = vec4( pix_NRNG, pix_NRNG, pix_NRNG, 1.0);
        imageStore(image_O, ivec2(st.x, st.y), pix);
    
    }
    */

   


}