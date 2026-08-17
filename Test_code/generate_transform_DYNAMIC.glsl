#version 460 core

layout(local_size_x = NUM_INV, local_size_y = 1, local_size_z = 1) in;
//NUM_INV = 128;

layout(rgba32f, binding = 0) uniform image2D dwt_mat;

const int full_img_dimension_x = WIDTH_IMG; //256


const float h0[SIZE_OF_FILTER] = { LOW_PASS_FILTER_VALUES };
const float h1[SIZE_OF_FILTER] = { HIGH_PASS_FILTER_VALUES };



shared int size_filter = SIZE_OF_FILTER;


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
    int a_1 = (th_id.y * 2 + 1 - (w_id.x + NUM_INV));
    int ind_left = ((a_0 % WIDTH_IMG) + WIDTH_IMG) % WIDTH_IMG;
    int ind_right = ((a_1 % WIDTH_IMG) + WIDTH_IMG) % WIDTH_IMG;
    vec4 pix_low_0 = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_high_0 = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_low_1 = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 pix_high_1 = vec4(0.0, 0.0, 0.0, 1.0);
    
    if(ind_left >= 0 && ind_left < SIZE_OF_FILTER){
        pix_low_0 = vec4(h0[ind_left], h0[ind_left], h0[ind_left], 1.0);
    }
    if(ind_left >=0 && ind_left < SIZE_OF_FILTER){
        pix_high_0 = vec4(h1[ind_left], h1[ind_left], h1[ind_left], 1.0);
    }
    if(ind_right >=0 && ind_right < SIZE_OF_FILTER){
        pix_low_1 = vec4(h0[ind_right], h0[ind_right], h0[ind_right], 1.0);
    }
    if(ind_right >=0 && ind_right < SIZE_OF_FILTER){
        pix_high_1 = vec4(h1[ind_right], h1[ind_right], h1[ind_right], 1.0);
    }

     imageStore(dwt_mat, ivec2(w_id.x, th_id.y), pix_low_0);
     imageStore(dwt_mat, ivec2(w_id.x + NUM_INV, th_id.y), pix_low_1);
     
     imageStore(dwt_mat, ivec2(w_id.x, NUM_INV + th_id.y), pix_high_0);
     imageStore(dwt_mat, ivec2(w_id.x + NUM_INV, NUM_INV + th_id.y),pix_high_1);
    


}