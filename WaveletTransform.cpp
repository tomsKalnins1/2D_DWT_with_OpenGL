
#include "ShaderProgram.h"



void WaveletTransform::transpose(Texture& img, int decomposition_level, int img_width) {
	
	ShaderProgram transp("transpose_region_DYNAMIC.glsl", decomposition_level, img_width, ShaderProgram::type_of_shader::TRANSPOSE_REGION);

	Texture::activate_tex_unit(0);
	img.bind_texture();
	img.bind_image_2D(0);

	Texture t_1 = Texture{};

	Texture::activate_tex_unit(1);
	t_1.bind_texture();
	t_1.bind_image_2D(1);

	transp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(1), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//img.unbind_texture();
	//glDeleteTextures(1, &t_1.ID);
	


}

void WaveletTransform::upsample(Texture& dwt, Texture& subband, int decomposition_level, int img_width, int H_L_0, int H_L_1) { //add subband specification 0/1, 0/1
	
	struct Subband {

		int H_L_0;
		int H_L_1;

	};

	ShaderProgram upsample("upsample.glsl");//not dynamic yet

	Texture::activate_tex_unit(2);
	dwt.bind_texture();
	dwt.bind_image_2D(0);

	Texture::reset_to_base(subband);
	Texture::activate_tex_unit(3);
	subband.bind_texture();
	subband.bind_image_2D(1);

	Subband subband_signal = { H_L_0, H_L_1 };

	unsigned int sub_H_L;
	glCreateBuffers(1, &sub_H_L);
	glNamedBufferStorage(sub_H_L, sizeof(Subband), &subband_signal, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sub_H_L);

	upsample.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(128), 1);//level specific 128
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


}

void WaveletTransform::do_DWT(Texture& input, Texture& output, int decomp_level, int img_width) {
	
	ShaderProgram dwt("apply_DWT_matrix_DYNAMIC.glsl", decomp_level, img_width, ShaderProgram::type_of_shader::APPLY_TRANSFORM);

	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);

	Texture output_intermediate = Texture{};
	Texture::activate_tex_unit(1);
	output_intermediate.bind_texture();
	output_intermediate.bind_image_2D(1);


	dwt.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width / decomp_level), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//input.unbind_texture();

	transpose(input, decomp_level, img_width);


	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);

	Texture::reset_to_base(output_intermediate);
	Texture::activate_tex_unit(1);
	output_intermediate.bind_texture();
	output_intermediate.bind_image_2D(1);

	dwt.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width/decomp_level), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_texture();
	glDeleteTextures(1, &output_intermediate.ID);

	transpose(input, decomp_level, img_width);

	input.unbind_texture();
}

void WaveletTransform::convolve(Texture& input, Texture& output, int img_width, int H_L) {
	
	ShaderProgram conv("apply_IDWT_matrix_DYNAMIC.glsl", 256, H_L);

	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	output.bind_texture();
	output.bind_image_2D(1);

	conv.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_texture();
	output.unbind_texture();


}


void WaveletTransform::do_inverse_DWT(Texture& input, Texture& output, int level, int img_width) {


	Texture LL = Texture{};
	Texture LH = Texture{};
	Texture HL = Texture{};
	Texture HH = Texture{};

	Texture LL_b = Texture{};
	Texture LH_b = Texture{};
	Texture HL_b = Texture{};
	Texture HH_b = Texture{};

	upsample(input, LL, level, img_width, 0, 0);
	upsample(input, LH, level, img_width, 0, 1);
	upsample(input, HL, level, img_width, 1, 0);
	upsample(input, HH, level, img_width, 1, 1);

	
	convolve(LL, LL_b, 256, 0);
	transpose(LL_b, 1, 256);
	convolve(LL_b, LL, 256, 0);
	transpose(LL, 1, 256);
	
	convolve(LH, LH_b, 256, 0);
	transpose(LH_b, 1, 256);	
	convolve(LH_b, LH, 256, 1);
	transpose(LH, 1, 256);
	
	convolve(HL, HL_b, 256, 1);
	transpose(HL_b, 1, 256);
	convolve(HL_b, HL, 256, 0);
	transpose(HL, 1, 256);

	convolve(HH, HH_b, 256, 1);
	transpose(HH_b, 1, 256);
	convolve(HH_b, HH, 256, 1);
	transpose(HH, 1, 256);
	

	ShaderProgram add("add_textures_dwt_inverse_DYNAMIC.glsl");

	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);


	Texture::activate_tex_unit(1);
	LL.bind_texture();
	LL.bind_image_2D(1);

	Texture::activate_tex_unit(2);
	LH.bind_texture();
	LH.bind_image_2D(2);


	Texture::activate_tex_unit(3);
	HL.bind_texture();
	HL.bind_image_2D(3);

	Texture::activate_tex_unit(4);
	HH.bind_texture();
	HH.bind_image_2D(4);


	add.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_texture();
	output.unbind_texture();

	glDeleteTextures(1, &LL.ID);
	glDeleteTextures(1, &LH.ID);
	glDeleteTextures(1, &HL.ID);
	glDeleteTextures(1, &HH.ID);

	glDeleteTextures(1, &LL_b.ID);
	glDeleteTextures(1, &LH_b.ID);
	glDeleteTextures(1, &HL_b.ID);
	glDeleteTextures(1, &HH_b.ID);

}
