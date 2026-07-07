#version 460 core

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D image_O;
layout(rgba32f, binding = 1) uniform image2D dwt;
layout(rgba32f, binding = 2) uniform image2D image_T;



void synchronize()
{

    memoryBarrierShared();
    barrier();

}



void main()
{
    ivec2 texC_g = ivec2(gl_GlobalInvocationID.xy);

        ivec2 w_id = ivec2(gl_GlobalInvocationID.xy);

        ivec2 w = ivec2(gl_LocalInvocationID.xy);

        float low_c = 0.0;
        float high_c = 0.0;

        for(int k = 0; k < 4; k++){

            vec4 img = imageLoad(image_O, ivec2(((w.x) * 2 + k) % 256, 255 - texC_g.y));

            vec4 low = imageLoad(dwt, ivec2(((w.x) * 2 + k) % 256, 255 - w.x));
            vec4 high = imageLoad(dwt, ivec2(((w.x) * 2 + k) % 256, 127 - w.x));

            low_c  += low.x * img.x;
            high_c += high.x * img.x;
            
        }

            imageStore(image_T, ivec2(w.x, 255 - texC_g.y), vec4( low_c, low_c, low_c, 1.0));
            imageStore(image_T, ivec2(w.x + 128 , 255 - texC_g.y), vec4( high_c, high_c, high_c, 1.0));

}