#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D dwt;
layout(rgba32f, binding = 1) uniform image2D subband_buffer;


layout(std430, binding = 3) buffer Sub_H_L{

    int H_L_0;
    int H_L_1;

};


void synchronize()
{

    memoryBarrierImage();
    barrier();

}

void upsample(){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);

    vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
    if(H_L_0 == 0 && H_L_1 == 0){
        
        pix = imageLoad(dwt, ivec2(th_id.x, th_id.y));
   //   imageStore(subband_buffer, ivec2(th_id.x * 2, th_id.y * 2), pix);
    }

    if(H_L_0 == 0 && H_L_1 == 1){
        
        pix = imageLoad(dwt, ivec2(th_id.x, th_id.y + 128));
    //      imageStore(subband_buffer, ivec2(th_id.x * 2, th_id.y * 2), pix);
    
    }

    if(H_L_0 == 1 && H_L_1 == 0){
        
        pix = imageLoad(dwt, ivec2(th_id.x + 128, th_id.y));
    //      imageStore(subband_buffer, ivec2(th_id.x * 2, th_id.y * 2), pix);
     
    }

     if(H_L_0 == 1 && H_L_1 == 1){
        
        pix = imageLoad(dwt, ivec2(th_id.x + 128, th_id.y + 128));
      //    imageStore(subband_buffer, ivec2(th_id.x * 2, th_id.y * 2), pix);
    
    }

   
  //      vec4 test = vec4(float(H_L_0), float(H_L_0), 0.0, 1.0);
        imageStore(subband_buffer, ivec2(th_id.x * 2, th_id.y * 2), pix);

        synchronize();
   

}



void main(){

    upsample();
 

}