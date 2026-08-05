#version 460 core

layout(local_size_x = 1, local_size_y = WIDTH_IMG, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D image_T;



//-------------------------------------------------------------------------SYNCHRONIZE DATA ACCESS
void synchronize()
{

    barrier();
    memoryBarrierShared();
}

void store_transp_to_tex(){
    
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);
    for(int i = 0; i < WIDTH_IMG; i++){
        pix = imageLoad(image_O, ivec2(i, th_id.y));
        imageStore(image_T, ivec2(th_id.y, i), pix);
    }

}

void store_transp_to_img(){

    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    
    vec4 pix = vec4(0.0, 0.0, 0.0, 1.0);

    for(int i = 0; i < WIDTH_IMG; i++){
        
        pix = imageLoad(image_T, ivec2(i, th_id.y));
        imageStore(image_O, ivec2(i, th_id.y), pix);        

    }
    
    
}


void main()
{
    store_transp_to_tex();
    synchronize();    
    store_transp_to_img();
    synchronize();
    



}