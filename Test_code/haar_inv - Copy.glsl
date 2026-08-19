#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D image_T;

vec4 buff[128];

void upsample() {
    
    ivec2 texC_g = ivec2(gl_GlobalInvocationID.xy);

    uint size_x = gl_WorkGroupSize.x;

    vec4 w_i = imageLoad(image_O, ivec2(texC_g.x + size_x, texC_g.y));

    buff[texC_g.x] = w_i;

    vec4 smpl = imageLoad(image_O, ivec2(texC_g.x, texC_g.y));
    vec4 zero_pix = vec4(0.0, 0.0, 0.0, 1.0);

    imageStore(image_T, ivec2(texC_g.x * 2, texC_g.y), smpl);
    imageStore(image_T, ivec2(texC_g.x * 2 + 1, texC_g.y), zero_pix);

}

void transform() {

    ivec2 texC_g = ivec2(gl_GlobalInvocationID.xy);

    vec4 left = imageLoad(image_T, ivec2(texC_g.x * 2, texC_g.y)) - buff[texC_g.x];
    vec4 right = imageLoad(image_T, ivec2(texC_g.x * 2, texC_g.y)) + buff[texC_g.x];

    imageStore(image_T, ivec2(texC_g.x * 2, texC_g.y), left);
    imageStore(image_T, ivec2(texC_g.x * 2 + 1, texC_g.y), right);

}



void main()
{

    upsample();
    transform();
    /*
    ivec2 texC_g = ivec2(gl_GlobalInvocationID.xy);
    //for (uint i = 0;  i < 128; i+=2) {


    vec4 pix_1 = imageLoad(image_O, ivec2(texC_g.x * 2, texC_g.y));
    vec4 pix_2 = imageLoad(image_O, ivec2(texC_g.x * 2 + 1, texC_g.y));

    float approx = (pix_1.x + pix_2.x) / 2.0;
    float det = (pix_2.x - pix_1.x) / 2.0;

    imageStore(image_T, ivec2(texC_g.x, texC_g.y), vec4(approx, approx, approx, 1.0));
    imageStore(image_T, ivec2(texC_g.x + 128, texC_g.y), vec4(det, det, det, 1.0));

    //}
    */



}