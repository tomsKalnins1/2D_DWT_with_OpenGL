#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D dwt_mat;




const float h0[4] = { -0.1294095226, 0.224143868, 0.8365163037, 0.4829629131 };
const float h1[4] = { 0.4829629131, -0.8365163037, 0.224143868, 0.1294095226 };


shared int size_filter = 4;


void synchronize()
{

    memoryBarrierShared();
    barrier();

}




void main()
{
    ivec2 th_id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 w_id = ivec2(gl_LocalInvocationID.xy);


    int a_0 = (th_id.y * 2 + 1 - w_id.x);
    int a_1 = (th_id.y * 2 + 1 - (w_id.x + 128));
    int ind_left = ((a_0 % 256) + 256) % 256;
    int ind_right = ((a_1 % 256) + 256) % 256;
    vec4 pix_low_0 = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_high_0 = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_low_1 = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_high_1 = vec4(0.0, 0.0, 0.0, 1.0);
    
    if(ind_left >= 0 && ind_left < 4){
        pix_low_0 = vec4(h0[ind_left], h0[ind_left], h0[ind_left], 1.0);
    }
    if(ind_left >=0 && ind_left < 4){
        pix_high_0 = vec4(h1[ind_left], h1[ind_left], h1[ind_left], 1.0);
    }
    if(ind_right >=0 && ind_right < 4){
        pix_low_1 = vec4(h0[ind_right], h0[ind_right], h0[ind_right], 1.0);
    }
    if(ind_right >=0 && ind_right < 4){
        pix_high_1 = vec4(h1[ind_right], h1[ind_right], h1[ind_right], 1.0);
    }

     imageStore(dwt_mat, ivec2(w_id.x, 255 - th_id.y), pix_low_0);
     imageStore(dwt_mat, ivec2(w_id.x + 128, 255 - th_id.y), pix_low_1);
     
     imageStore(dwt_mat, ivec2(w_id.x, 127 - th_id.y), pix_high_0);
     imageStore(dwt_mat, ivec2(w_id.x + 128, 127 - th_id.y),pix_high_1);
    


}