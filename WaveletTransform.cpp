
#include "ShaderProgram.h"

WaveletTransform::WaveletTransform(string file_path, int decomposition_level, int img_dimension_width, int size_filter_num_part, WaveletTransform::type_of_shader type_of_shader) {

	ShaderSource comp;

	string source_code = "";

	if (type_of_shader == WaveletTransform::type_of_shader::SORT_SUBBAND) {
		SortSubbandLocal ssl(file_path, decomposition_level, img_dimension_width, size_filter_num_part);
		source_code = ssl.set_compute_shader_values(file_path);
		comp = ssl;
	}

	if (type_of_shader == WaveletTransform::type_of_shader::APPLY_INVERSE_TRANSFORM) {
		ApplyTransform ait(file_path, decomposition_level, img_dimension_width, size_filter_num_part);
		source_code = ait.set_compute_shader_values_inverse(file_path, size_filter_num_part);
		std::cout << "APPLY INVERSE TRANSFORM MATRIX DYNAMIC : \n" << source_code << '\n';
		comp = ait;

	}

	if (type_of_shader == WaveletTransform::type_of_shader::SOFT_THRESHOLD) {
		SoftThreshold st(file_path, decomposition_level, img_dimension_width, size_filter_num_part);
		source_code = st.set_compute_shader_values(file_path);
		std::cout << "APPLY INVERSE TRANSFORM MATRIX DYNAMIC : \n" << source_code << '\n';
		comp = st;

	}



	//	std::cout << "SHADER SOURCE : \n" << source_code << '\n';

	const char* shader_source_char = source_code.c_str();


	comp.ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp.ID, 1, &shader_source_char, NULL);
	glCompileShader(comp.ID);

	GLint compiled;
	glGetShaderiv(comp.ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(comp.ID, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, comp.ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << file_path << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(comp.ID);


}

WaveletTransform::WaveletTransform(string file_path, int decomposition_level, int img_width, int H_L_0, int H_L_1) {

	UpsampleSubband us(file_path, decomposition_level, img_width, H_L_0, H_L_1);

	string source_code = us.set_compute_shader_values(file_path);

//	std::cout << "WAVELET TRANSFORM SHADER SOURCE UPSAMPLE : \n" << source_code << '\n';
	
	const char* shader_source_char = source_code.c_str();


	us.ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(us.ID, 1, &shader_source_char, NULL);
	glCompileShader(us.ID);

	GLint compiled;
	glGetShaderiv(us.ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(us.ID, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, us.ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << file_path << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(us.ID);



}

	//img_dimension_width is also used for specifying the subband to convolve MUST RENAME THE ARGS LATER

WaveletTransform::WaveletTransform(string file_path, int decomposition_level, int img_dimension_width, WaveletTransform::type_of_shader type_of_shader) {

	ShaderSource comp;

	string source_code = "";

	if (type_of_shader == WaveletTransform::type_of_shader::APPLY_TRANSFORM) {
		ApplyTransform at(file_path, decomposition_level, img_dimension_width);
		source_code = at.set_compute_shader_values(file_path);
	//	std::cout << "APPLY MATRIX DYNAMIC : \n" << source_code << '\n';
		comp = at;

	}

	if (type_of_shader == WaveletTransform::type_of_shader::ADD_SUBBANDS) {
		AddIDWTSubbands aidwts(file_path, decomposition_level, img_dimension_width);
		source_code = aidwts.set_compute_shader_values(file_path);
		//	std::cout << "APPLY MATRIX DYNAMIC : \n" << source_code << '\n';
		comp = aidwts;

	}

	if (type_of_shader == WaveletTransform::type_of_shader::TRANSPOSE_REGION) {
		TransposeRegion tr(file_path, decomposition_level, img_dimension_width);
		source_code = tr.set_compute_shader_values(file_path);
	//	std::cout << "TRANSPOSE REGION DYNAMIC : \n" << source_code << '\n';
		comp = tr;
	}


	//	ApplyTransform comp(file_path, decomposition_level, img_dimension_width);

	//	std::cout << "APPLY TRANSFORM DYNAMIC source for file : " << file_path << '\n' << source_code << '\n';
		//std::cout << "SHADER SOURCE APPLY TRANSFORM : \n" << source_code << '\n';

	const char* shader_source_char = source_code.c_str();


	comp.ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp.ID, 1, &shader_source_char, NULL);
	glCompileShader(comp.ID);

	GLint compiled;
	glGetShaderiv(comp.ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(comp.ID, 1024, NULL, errorLog);
		std::cout << (" APPLY TRANSFORM COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, comp.ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "APPLY PROGRAM LINK FAILED, PROBLEM WITH " << file_path << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(comp.ID);


}
/*
WaveletTransform::WaveletTransform(string file_path, int img_width, int H_L) {

	string source_code = "";
	ApplyTransform comp(file_path, 1, 256);
	source_code = comp.set_compute_shader_values_inverse(file_path, H_L);


//	std::cout << "INVERSE DWT : \n" << source_code << '\n';

	const char* shader_source_char = source_code.c_str();


	comp.ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp.ID, 1, &shader_source_char, NULL);
	glCompileShader(comp.ID);

	GLint compiled;
	glGetShaderiv(comp.ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(comp.ID, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, comp.ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << file_path << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(comp.ID);


}
*/


void WaveletTransform::transpose(Texture& img, Texture& buffer_tex,  int decomposition_level, int img_width) {
	
	WaveletTransform transp("transpose_region_DYNAMIC.glsl", decomposition_level, img_width, WaveletTransform::type_of_shader::TRANSPOSE_REGION);
	

	Texture::activate_tex_unit(0);
	img.bind_texture();
	img.bind_image_2D(0);

	Texture test = Texture{};
	//Texture::reset_to_base(buffer_tex);
	Texture::activate_tex_unit(1);
	buffer_tex.bind_texture();
	buffer_tex.bind_image_2D(1);

	transp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(1), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	img.unbind_image_texture(0);
	buffer_tex.unbind_image_texture(1);
	img.unbind_texture(0);
	buffer_tex.unbind_texture(1);

}

void WaveletTransform::upsample(Texture& dwt, Texture& subband, int decomposition_level, int img_width, int H_L_0, int H_L_1) { //add subband specification 0/1, 0/1
	
	struct Subband {

		int H_L_0;
		int H_L_1;

	};
	//REMOVE THE H_L_0 AND H_L_1 FROM THE UPSAMPLE CONSTRUCTOR AND JUST USE THE SHADER TYPE ENUM IN THE EXISTING CONSTRUCTOR (string, int, int, shader_type)
	WaveletTransform upsample_comp("upsample.glsl", decomposition_level, img_width, H_L_0, H_L_1);

	Texture::activate_tex_unit(2);
	dwt.bind_texture();
	dwt.bind_image_2D(0);

	//Texture::reset_to_base(subband);
	Texture::activate_tex_unit(3);
	subband.bind_texture();
	subband.bind_image_2D(1);

	Subband subband_signal = { H_L_0, H_L_1 };

	unsigned int sub_H_L;
	glCreateBuffers(1, &sub_H_L);
	glNamedBufferStorage(sub_H_L, sizeof(Subband), &subband_signal, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sub_H_L);

	upsample_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width / pow(2, decomposition_level)), 1);//level specific 128
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	dwt.unbind_texture(0);
	subband.unbind_texture(1);
	dwt.unbind_image_texture(0);
	subband.unbind_image_texture(1);

}

void WaveletTransform::sort_subbands(Texture& input, Texture& output, int decomp_lvl, int img_width, int num_sort_partitions) {
	
	WaveletTransform sort_subbands_comp("Sort_subband_LOCAL_DYNAMIC.glsl", decomp_lvl, img_width, num_sort_partitions, WaveletTransform::SORT_SUBBAND);


	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);

//	Texture::reset_to_base(output);
	Texture::activate_tex_unit(1);
	output.bind_texture();
	output.bind_image_2D(1);
	
	sort_subbands_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width / pow(2, decomp_lvl)), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_image_texture(0);
	output.unbind_image_texture(1);
	input.unbind_texture(0);
	output.unbind_texture(1);



}

void WaveletTransform::apply_soft_threshold(Texture& input, Texture& sorted_subband, int decomp_lvl, int img_width, int num_sort_partitions) {
	
	WaveletTransform soft("Soft_threshold_DYNAMIC.glsl", decomp_lvl, img_width, num_sort_partitions, WaveletTransform::type_of_shader::SOFT_THRESHOLD);

	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);

	//	Texture::reset_to_base(output);
	Texture::activate_tex_unit(1);
	sorted_subband.bind_texture();
	sorted_subband.bind_image_2D(1);

	soft.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width / pow(2, decomp_lvl)), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_image_texture(0);
	sorted_subband.unbind_image_texture(1);
	input.unbind_texture(0);
	sorted_subband.unbind_texture(1);

}



void WaveletTransform::do_DWT(Texture& input, Texture& output, int decomp_level, int img_width) {

	WaveletTransform dwt("apply_DWT_matrix_DYNAMIC.glsl", decomp_level, img_width, WaveletTransform::type_of_shader::APPLY_TRANSFORM);

	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);

	//Texture output_intermediate = Texture{};
//	Texture::reset_to_base(output);
	Texture::activate_tex_unit(1);
	output.bind_texture();
	output.bind_image_2D(1);


	dwt.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width / pow(2, decomp_level - 1)), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_texture(0);
	input.unbind_image_texture(0);
	output.unbind_texture(1);
	output.unbind_image_texture(1);

	transpose(output, input, decomp_level, img_width);

	Texture::activate_tex_unit(0);
	output.bind_texture();
	output.bind_image_2D(0);

//	Texture::reset_to_base(input);
	Texture::activate_tex_unit(1);
	input.bind_texture();
	input.bind_image_2D(1);

	dwt.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width / pow(2, decomp_level - 1)), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_texture(1);
	input.unbind_image_texture(1);
	output.unbind_texture(0);
	output.unbind_image_texture(0);

	transpose(input, output, decomp_level, img_width);

	input.unbind_texture(0);
	input.unbind_image_texture(0);
	//glDeleteTextures(1, &output_intermediate.ID);
}

void WaveletTransform::convolve(Texture& input, Texture& output, int img_width, int level,int H_L) {

	WaveletTransform conv("apply_IDWT_matrix_DYNAMIC.glsl", level, img_width, H_L, WaveletTransform::type_of_shader::APPLY_INVERSE_TRANSFORM);

	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);

//	Texture::reset_to_base(output);
	Texture::activate_tex_unit(1);
	output.bind_texture();
	output.bind_image_2D(1);

	conv.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_image_texture(0);
	output.unbind_image_texture(1);

	input.unbind_texture(0);
	output.unbind_texture(1);

	


}

void WaveletTransform::add_IDWT_subbands(Texture& dwt, Texture& LL, Texture& LH, Texture& HL, Texture& HH, int level, int img_width) {

	WaveletTransform add("add_textures_dwt_inverse_DYNAMIC.glsl", level, img_width, WaveletTransform::type_of_shader::ADD_SUBBANDS);

	Texture::activate_tex_unit(0);
	dwt.bind_texture();
	dwt.bind_image_2D(0);

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
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width / pow(2, level - 1)), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	dwt.unbind_texture(0);
	LL.unbind_texture(1);
	LH.unbind_texture(2);
	HL.unbind_texture(3);
	HH.unbind_texture(4);
	
}

void WaveletTransform::do_inverse_DWT(Texture& input, Texture& output, int level, int img_width) {

	int tex_size = img_width / pow(2, level - 1);

	Texture LL(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture LH(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HL(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HH(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);

	Texture LL_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture LH_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HL_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HH_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);

	upsample(input, LL, level, img_width, 0, 0);
	upsample(input, LH, level, img_width, 0, 1);
	upsample(input, HL, level, img_width, 1, 0);
	upsample(input, HH, level, img_width, 1, 1);


	convolve(LL, LL_b, img_width, level,0);
	transpose(LL, LL_b,level, img_width);
	convolve(LL, LL_b, img_width, level,0);
	transpose(LL, LL_b, level, img_width);

	convolve(LH, LH_b, img_width, level, 0);
	transpose(LH, LH_b, level, img_width);
	convolve(LH, LH_b, img_width, level, 1);
	transpose(LH, LH_b, level, img_width);

	convolve(HL, HL_b, img_width, level, 1);
	transpose(HL, HL_b, level, img_width);
	convolve(HL, HL_b, img_width, level, 0);
	transpose(HL, HL_b, level, img_width);

	convolve(HH, HH_b, img_width, level, 1);
	transpose(HH, HH_b, level, img_width);
	convolve(HH, HH_b, img_width, level, 1);
	transpose(HH, HH_b, level, img_width);


	WaveletTransform::add_IDWT_subbands(input, LL, LH, HL, HH, level, img_width);
	/*
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
	*/
	input.unbind_texture(0);
	input.unbind_image_texture(0);
	
	glDeleteTextures(1, &LL.ID);
	glDeleteTextures(1, &LH.ID);
	glDeleteTextures(1, &HL.ID);
	glDeleteTextures(1, &HH.ID);

	glDeleteTextures(1, &LL_b.ID);
	glDeleteTextures(1, &LH_b.ID);
	glDeleteTextures(1, &HL_b.ID);
	glDeleteTextures(1, &HH_b.ID);

}
