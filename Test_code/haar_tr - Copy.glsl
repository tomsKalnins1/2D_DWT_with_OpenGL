#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D image_T;



void main() {

    ivec2 texC_g = ivec2(gl_GlobalInvocationID.xy);
    //for (uint i = 0;  i < 128; i+=2) {


        vec4 pix_1 = imageLoad(image_O, ivec2(texC_g.x * 2, texC_g.y));
        vec4 pix_2 = imageLoad(image_O, ivec2(texC_g.x * 2 + 1, texC_g.y));

        float approx = (pix_1.x + pix_2.x) / 2.0;
        float det = (pix_2.x - pix_1.x) / 2.0;

        imageStore(image_T, ivec2(texC_g.x , texC_g.y), vec4(approx, approx, approx, 1.0));
        imageStore(image_T, ivec2(texC_g.x + 128, texC_g.y), vec4(det, det, det, 1.0));

    //}

}