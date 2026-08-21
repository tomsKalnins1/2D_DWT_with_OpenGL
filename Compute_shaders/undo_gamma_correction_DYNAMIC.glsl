#version 460 core

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D input_img;


void undo_gamma_correction(){

     ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
     vec4 new_val = vec4(0.0, 0.0, 0.0, 1.0);

     for(int i = 0; i < IMG_WIDTH / 256; i++){
        /*
        stbi has constant static float stbi__l2h_gamma=2.2f which is used in pow(data[i*comp+k]/255.0f, stbi__l2h_gamma)
        which seems to correct gamma so that has to be undone as the image becomes too dark by doing pow(pix_val, 1.0/2.2)
        the image is loaded to texture using stbi_loadf , which calls stbi_loadf_from_file, which then calls, stbi__loadf_main
        which then calls stbi__ldr_to_hdr which is the function in the stb library that does the pow(pix, stbi__l2h_gamma) gamma correction 
        */
        float no_gamma_c = pow(imageLoad(input_img, ivec2(th_id.x + i , th_id.y + i)).x, 1.0 / 2.2);
        new_val = vec4(no_gamma_c, no_gamma_c, no_gamma_c, 1.0);
        imageStore(input_img, ivec2(th_id.x + i, th_id.y + i), new_val);
     }
}

void main(){

    undo_gamma_correction();      

}