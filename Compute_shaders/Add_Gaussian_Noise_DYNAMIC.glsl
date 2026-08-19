#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D noise_0;
layout(rgba32f, binding = 1) uniform image2D noise_1;
layout(rgba32f, binding = 2) uniform image2D AWGN;
layout(rgba32f, binding = 3) uniform image2D input_img;
//layout(rgba32f, binding = 4) uniform image2D deviation_noise;




const float pi = 3.141592653589793238462643383279502884197169399375;

const float standard_deviation = STANDARD_DEVIATION;

void add_noise_to_input(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 w_id = ivec2(gl_LocalInvocationID.xy);

    vec4 img_p = imageLoad(input_img, ivec2(th_id));
    vec4 noise_p = imageLoad(AWGN, ivec2(th_id)) * standard_deviation;

    vec4 sum = img_p + noise_p;
        
    imageStore(input_img, ivec2(th_id), sum);

}
/*
void draw_deviation(){

     ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
     
     if(th_id.x == 0 ){
     float mean = 0.0;
     for(int i = 0; i < 256; i++){
        mean += imageLoad(AWGN, ivec2(i, th_id.y)).x;
     }
     mean = mean / 256.0;
     float deviation = 0.0;
     for(int i = 0; i < 256; i++){
        deviation += pow(imageLoad(AWGN, ivec2(i, th_id.y)).x - mean, 2.0);
     }

     deviation = sqrt(deviation / 256.0);

        uint dev =  floatBitsToInt(deviation);

        uint pix_val = 0; 

        for(int i = 0; i < 32; i++){
            pix_val = ( dev >> i ) & 1;
            float pix_val_fl = float(pix_val);
            vec4 pix = vec4(pix_val_fl, pix_val_fl, pix_val_fl, 1.0);
            if(pix_val == 0){
                
                pix = vec4(0.0, 0.0, 1.0, 1.0);        

            }

            

            imageStore(deviation_noise, ivec2(i * 4, th_id.y), pix);
        }
     
     }

}
*/
void main(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    vec4 n_0 = imageLoad(noise_0, th_id);
    vec4 n_1 = imageLoad(noise_1, th_id);

    

   float pix_NRNG = sqrt((-2.0) * log(n_0.x)) * cos(2.0 * pi * n_1.x);
   
   imageStore(AWGN, ivec2(th_id.x, th_id.y), vec4(pix_NRNG, pix_NRNG, pix_NRNG, 1.0));
  // draw_deviation();

   add_noise_to_input();

}