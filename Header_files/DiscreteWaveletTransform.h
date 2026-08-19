#ifndef DISCRETE_WAVELET_TRANSFORM
#define DISCRETE_WAVELET_TRANSFORM

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "ShaderSource.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <vector>
#include <array>




template<class ...H>
class GetFilter {

public:
	std::array<float, sizeof ...(H)> h0;


	constexpr GetFilter() = default;
	constexpr GetFilter(H ...arg) : h0{ static_cast<float>(arg) ... } {}

	constexpr std::array<float, sizeof ...(H)> get_highpass_analysis() const {
		std::array<float, sizeof ...(H)> h1{};
		for (int i = 0; i < sizeof ...(H); i++) {
			int val = -1;
			for (int k = 0; k < i; k++) {
				val *= (-1);
			}
			h1[sizeof...(H) - 1 - i] = h0[i] * (-1) * val;

		}
		return h1;
	}

	constexpr std::array<float, sizeof ...(H)> get_lowpass_synthesis() const {
		std::array<float, sizeof ...(H)> g0{};
		for (int i = 0; i < sizeof ...(H); i++) {

			g0[sizeof...(H) - 1 - i] = h0[i];

		}
		return g0;
	}

	constexpr std::array<float, sizeof ...(H)> get_highpass_synthesis() const {
		std::array<float, sizeof ...(H)> g1{};
		for (int i = 0; i < sizeof ...(H); i++) {
			int val = 1;
			for (int k = 0; k < i; k++) {
				val *= (-1);
			}
			g1[i] = h0[i] * val;

		}
		return g1;
	}

};


//class Test kas variable number of types
template<typename... C>
class Filter {
public:

	//instantiates parametriced Filter<float, float, float, ...> (floats in my case of use)
	GetFilter<C ...> ff;
	std::array<float, sizeof ...(C)> h0_w;
	std::array<float, sizeof ...(C)> h1_w;
	std::array<float, sizeof ...(C)> g0_w;
	std::array<float, sizeof ...(C)> g1_w;

	//exprects to be evaluated at compile time when instantiated
	//C... unpacks the parameter list to be Test(float, float, float, ...) and calls the the arguments to be args
	//args ... unpacks the actual values to bet then Test(float a, float b, float c, ...) : ff {a, b, c, ...}
	constexpr Filter(C ...args) : ff{ args ... } {
		h0_w = ff.h0;
		h1_w = ff.get_highpass_analysis();
		g0_w = ff.get_lowpass_synthesis();
		g1_w = ff.get_highpass_synthesis();
	}

};

template<std::size_t W>
class Wavelet_0 {

public:


	int image_width;

	std::array<float, W> h0;
	std::array<float, W> h1;
	std::array<float, W> g0;
	std::array<float, W> g1;
	Wavelet_0(int img_w, const std::array<float, W> f0, const std::array<float, W> f1, const std::array<float, W> k0, const std::array<float, W> k1) :
		h0{ f0 },
		h1{ f1 },
		g0{ k0 },
		g1{ k1 },
		image_width{img_w} {
	}

	ShaderProgram transpose_prog(int decomp_level) {
		
		ShaderProgram transp;
		string path = "Compute_shaders\\transpose_region_DYNAMIC.glsl";

		string source_code;
		TransposeRegion comp(path, decomp_level, image_width);
		source_code = comp.set_compute_shader_values(path);

		const char* shader_source_char = source_code.c_str();


		comp.ID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(comp.ID, 1, &shader_source_char, NULL);
		glCompileShader(comp.ID);

		GLint compiled;
		glGetShaderiv(comp.ID, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			char errorLog[1024];
			glGetShaderInfoLog(comp.ID, 1024, NULL, errorLog);
			std::cout << (" APPLY TRANSFORM COMPUTE SHADER " + path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
		}

		transp.ID = glCreateProgram();

		glAttachShader(transp.ID, comp.ID);
		glLinkProgram(transp.ID);
		GLint linkSuccess = 0;
		glGetProgramiv(transp.ID, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE) {

			char infoLog[1024];
			glGetProgramInfoLog(transp.ID, 1024, NULL, infoLog);
			std::cout << "APPLY PROGRAM LINK FAILED, PROBLEM WITH " << path << " :\n" << infoLog << std::endl;

		}

		glDeleteShader(comp.ID);

		
	

		return transp;


	}

	ShaderProgram upsample_prog(int decomp_lvl, int H_L_0, int H_L_1) {
		
		ShaderProgram upsmpl;
		string path = "Compute_shaders\\upsample_subband_DYNAMIC.glsl";

		UpsampleSubband us(path, decomp_lvl, image_width, H_L_0, H_L_1);

		string source_code = us.set_compute_shader_values(path);

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
			std::cout << ("COMPUTE SHADER " + path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
		}

		upsmpl.ID = glCreateProgram();

		glAttachShader(upsmpl.ID, us.ID);
		glLinkProgram(upsmpl.ID);
		GLint linkSuccess = 0;
		glGetProgramiv(upsmpl.ID, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE) {

			char infoLog[1024];
			glGetProgramInfoLog(upsmpl.ID, 1024, NULL, infoLog);
			std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << path << " :\n" << infoLog << std::endl;

		}

		glDeleteShader(us.ID);

		return upsmpl;

	
	}

	ShaderProgram sort_subbands_prog(int decomp_lvl,int num_sort_partitions) {

		ShaderProgram sort;
		
		string path = "Compute_shaders\\Sort_subband_LOCAL_DYNAMIC.glsl";
		string source_code = "";
		SortSubbandLocal srt(path, decomp_lvl, image_width, num_sort_partitions);
		source_code = srt.set_compute_shader_values(path);
	//	std::cout << "SORT FROM DWT HEADER : \n" << source_code << '\n';
	
		const char* shader_source_char = source_code.c_str();


		srt.ID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(srt.ID, 1, &shader_source_char, NULL);
		glCompileShader(srt.ID);

		GLint compiled;
		glGetShaderiv(srt.ID, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			char errorLog[1024];
			glGetShaderInfoLog(srt.ID, 1024, NULL, errorLog);
			std::cout << ("COMPUTE SHADER " + path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
		}

		sort.ID = glCreateProgram();

		glAttachShader(sort.ID, srt.ID);
		glLinkProgram(sort.ID);
		GLint linkSuccess = 0;
		glGetProgramiv(sort.ID, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE) {

			char infoLog[1024];
			glGetProgramInfoLog(sort.ID, 1024, NULL, infoLog);
			std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << path << " :\n" << infoLog << std::endl;

		}

		glDeleteShader(srt.ID);

		return sort;

	}

	ShaderProgram apply_soft_threshold_prog(int decomp_lvl, int num_sort_partitions) {
		ShaderProgram soft;
		string path = "Compute_shaders\\Soft_threshold_DYNAMIC.glsl";
		string source_code = "";

		SoftThreshold st(path, decomp_lvl, image_width, num_sort_partitions);
		source_code = st.set_compute_shader_values(path);


		const char* shader_source_char = source_code.c_str();


		st.ID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(st.ID, 1, &shader_source_char, NULL);
		glCompileShader(st.ID);

		GLint compiled;
		glGetShaderiv(st.ID, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			char errorLog[1024];
			glGetShaderInfoLog(st.ID, 1024, NULL, errorLog);
			std::cout << ("COMPUTE SHADER " + path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
		}

		soft.ID = glCreateProgram();

		glAttachShader(soft.ID, st.ID);
		glLinkProgram(soft.ID);
		GLint linkSuccess = 0;
		glGetProgramiv(soft.ID, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE) {

			char infoLog[1024];
			glGetProgramInfoLog(soft.ID, 1024, NULL, infoLog);
			std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << path << " :\n" << infoLog << std::endl;

		}

		glDeleteShader(st.ID);

		return soft;
	}

	ShaderProgram do_DWT_prog(int decomp_lvl) {

		ShaderProgram do_dwt;
		string path = "Compute_shaders\\apply_DWT_matrix_DYNAMIC.glsl";
		string source_code = "";

		string analysis_low = "";
		string analysis_high = "";

		char sep;

		for (int i = 0; i < h0.size(); i++) {

			sep = (i != h0.size() - 1) ? ',' : ' ';
			analysis_low += std::to_string(h0[i]) + sep;
			//	std::cout << g0[i] << '\n';
		}


		for (int i = 0; i < h0.size(); i++) {

			sep = (i != h0.size() - 1) ? ',' : ' ';
			analysis_high += std::to_string(h1[i]) + sep;
			//	std::cout << g1[i] << '\n';
		}
	
		ApplyTransform at(path, decomp_lvl, image_width);
		source_code = at.set_compute_shader_values(path, h0.size(), analysis_low, analysis_high);
	//	std::cout << "DO_DWT_SAHDER CODE : \n" << source_code << '\n';

		const char* shader_source_char = source_code.c_str();


		at.ID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(at.ID, 1, &shader_source_char, NULL);
		glCompileShader(at.ID);

		GLint compiled;
		glGetShaderiv(at.ID, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			char errorLog[1024];
			glGetShaderInfoLog(at.ID, 1024, NULL, errorLog);
			std::cout << (" APPLY TRANSFORM COMPUTE SHADER " + path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
		}

		do_dwt.ID = glCreateProgram();

		glAttachShader(do_dwt.ID, at.ID);
		glLinkProgram(do_dwt.ID);
		GLint linkSuccess = 0;
		glGetProgramiv(do_dwt.ID, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE) {

			char infoLog[1024];
			glGetProgramInfoLog(do_dwt.ID, 1024, NULL, infoLog);
			std::cout << "APPLY PROGRAM LINK FAILED, PROBLEM WITH " << path << " :\n" << infoLog << std::endl;

		}

		glDeleteShader(at.ID);

		return do_dwt;


	}

	//convolve subband
	ShaderProgram do_IDWT_prog(int decomp_lvl, int H_L) {

		ShaderProgram idwt;
		string source_code = "";
		string path = "Compute_shaders\\apply_IDWT_matrix_DYNAMIC.glsl";

		string synthesis_low = "";
		string synthesis_high = "";

		char sep;

		for (int i = 0; i < h0.size(); i++) {

			sep = (i != h0.size() - 1) ? ',' : ' ';
			synthesis_low += std::to_string(g0[i]) + sep;
			//	std::cout << g0[i] << '\n';
		}


		for (int i = 0; i < h0.size(); i++) {

			sep = (i != h0.size() - 1) ? ',' : ' ';
			synthesis_high += std::to_string(g1[i]) + sep;
			//	std::cout << g1[i] << '\n';
		}
	//	std::cout <<  analysis_low << '\n';


		
		string filter_conv = "";
		if (H_L == 1) {
			filter_conv = synthesis_high;
		}
		if (H_L == 0) {
			filter_conv = synthesis_low;
		}

		ApplyTransform ait(path, decomp_lvl, image_width, H_L);
		source_code = ait.set_compute_shader_values_inverse(path, H_L, h0.size(), filter_conv);

	//	std::cout << "INVERSE DWT SOURCE = \n" << source_code << '\n';


		const char* shader_source_char = source_code.c_str();


		ait.ID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(ait.ID, 1, &shader_source_char, NULL);
		glCompileShader(ait.ID);

		GLint compiled;
		glGetShaderiv(ait.ID, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			char errorLog[1024];
			glGetShaderInfoLog(ait.ID, 1024, NULL, errorLog);
			std::cout << ("COMPUTE SHADER " + path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
		}

		idwt.ID = glCreateProgram();

		glAttachShader(idwt.ID, ait.ID);
		glLinkProgram(idwt.ID);
		GLint linkSuccess = 0;
		glGetProgramiv(idwt.ID, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE) {

			char infoLog[1024];
			glGetProgramInfoLog(idwt.ID, 1024, NULL, infoLog);
			std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << path << " :\n" << infoLog << std::endl;

		}

		glDeleteShader(ait.ID);


		return idwt;
	
	}

	ShaderProgram add_IDWT_subbands_prog( int decomp_lvl) {
		
		ShaderProgram add;

		string path = "Compute_shaders\\add_textures_dwt_inverse_DYNAMIC.glsl";
		string source_code = "";

		AddIDWTSubbands ads(path, decomp_lvl, image_width);
		source_code = ads.set_compute_shader_values(path);

		const char* shader_source_char = source_code.c_str();


		ads.ID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(ads.ID, 1, &shader_source_char, NULL);
		glCompileShader(ads.ID);

		GLint compiled;
		glGetShaderiv(ads.ID, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			char errorLog[1024];
			glGetShaderInfoLog(ads.ID, 1024, NULL, errorLog);
			std::cout << (" APPLY TRANSFORM COMPUTE SHADER " + path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
		}

		add.ID = glCreateProgram();

		glAttachShader(add.ID, ads.ID);
		glLinkProgram(add.ID);
		GLint linkSuccess = 0;
		glGetProgramiv(add.ID, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE) {

			char infoLog[1024];
			glGetProgramInfoLog(add.ID, 1024, NULL, infoLog);
			std::cout << "APPLY PROGRAM LINK FAILED, PROBLEM WITH " << path << " :\n" << infoLog << std::endl;

		}

		glDeleteShader(ads.ID);

		return add;


	
	}

	void transpose(Texture& input, Texture& intermediate, int decomp_lvl) {
		
		ShaderProgram transp = transpose_prog(decomp_lvl);

		Texture::activate_tex_unit(0);
		input.bind_texture();
		input.bind_image_2D(0);


		//	Texture::reset_to_base(buffer_tex);
		Texture::activate_tex_unit(1);
		intermediate.bind_texture();
		intermediate.bind_image_2D(1);

		transp.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(1), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		input.unbind_image_texture(0);
		intermediate.unbind_image_texture(1);
		input.unbind_texture(0);
		intermediate.unbind_texture(1);

	
	}

	void upsample(Texture& dwt, Texture& subband, int decomp_lvl, int H_L_0, int H_L_1) {
		
		ShaderProgram up = upsample_prog(decomp_lvl, H_L_0, H_L_1);

		struct Subband {

			int H_L_0;
			int H_L_1;

		};

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

		up.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(image_width / pow(2, decomp_lvl)), 1);//level specific 128
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		dwt.unbind_texture(0);
		subband.unbind_texture(1);
		dwt.unbind_image_texture(0);
		subband.unbind_image_texture(1);
	
	}

	void sort_subbands(Texture& input, Texture& output, int decomp_lvl, int num_sort_partitions) {

		ShaderProgram sort = sort_subbands_prog(decomp_lvl, num_sort_partitions);




		Texture::activate_tex_unit(0);
		input.bind_texture();
		input.bind_image_2D(0);

		//	Texture::reset_to_base(output);
		Texture::activate_tex_unit(1);
		output.bind_texture();
		output.bind_image_2D(1);

		sort.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(image_width / pow(2, decomp_lvl)), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glFinish();

		input.unbind_image_texture(0);
		output.unbind_image_texture(1);
		input.unbind_texture(0);
		output.unbind_texture(1);



	}

	void apply_soft_threshold(Texture& input, Texture& sorted_subband, int decomp_lvl, int num_sort_partitions) {

		ShaderProgram soft = apply_soft_threshold_prog(decomp_lvl, num_sort_partitions);

		Texture::activate_tex_unit(0);
		input.bind_texture();
		input.bind_image_2D(0);

		//Texture::reset_to_base(sorted_subband);
		Texture::activate_tex_unit(1);
		sorted_subband.bind_texture();
		sorted_subband.bind_image_2D(1);

		soft.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(image_width / pow(2, decomp_lvl)), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		input.unbind_image_texture(0);
		sorted_subband.unbind_image_texture(1);
		input.unbind_texture(0);
		sorted_subband.unbind_texture(1);

	}

	void do_DWT(Texture& input, Texture& output, int decomp_lvl) {
		ShaderProgram dwt = do_DWT_prog(decomp_lvl);

		Texture::activate_tex_unit(0);
		input.bind_texture();
		input.bind_image_2D(0);

		Texture output_intermediate = Texture{};
		//Texture::reset_to_base(output_intermediate);
		Texture::activate_tex_unit(1);
		output.bind_texture();
		output.bind_image_2D(1);


		dwt.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(image_width / pow(2, decomp_lvl - 1)), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


		input.unbind_texture(0);
		input.unbind_image_texture(0);
		output_intermediate.unbind_texture(1);
		output_intermediate.unbind_image_texture(1);

		transpose(input, output, decomp_lvl);

		Texture::activate_tex_unit(0);
		input.bind_texture();
		input.bind_image_2D(0);

		//	Texture::reset_to_base(output);
		Texture::activate_tex_unit(1);
		output.bind_texture();
		output.bind_image_2D(1);

		dwt.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(image_width / pow(2, decomp_lvl - 1)), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		input.unbind_texture(1);
		input.unbind_image_texture(1);
		output_intermediate.unbind_texture(0);
		output_intermediate.unbind_image_texture(0);

		transpose(input, output, decomp_lvl);

		input.unbind_texture(0);
		input.unbind_image_texture(0);
	}

	void convolve(Texture& input, Texture& output, int decomp_lvl, int H_L) {
		
		ShaderProgram conv = do_IDWT_prog(decomp_lvl, H_L);

		Texture::activate_tex_unit(0);
		input.bind_texture();
		input.bind_image_2D(0);

		Texture::reset_to_base(output);
		Texture::activate_tex_unit(1);
		output.bind_texture();
		output.bind_image_2D(1);

		conv.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(image_width), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		input.unbind_image_texture(0);
		output.unbind_image_texture(1);

		input.unbind_texture(0);
		output.unbind_texture(1);
	
	}

	void add_IDWT_subbands(Texture& dwt, Texture& LL, Texture& LH, Texture& HL, Texture& HH, int decomp_lvl) {
	
		ShaderProgram add = add_IDWT_subbands_prog(decomp_lvl);

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
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(image_width / pow(2, decomp_lvl - 1)), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		dwt.unbind_texture(0);
		LL.unbind_texture(1);
		LH.unbind_texture(2);
		HL.unbind_texture(3);
		HH.unbind_texture(4);
		dwt.unbind_image_texture(0);
		LL.unbind_image_texture(1);
		LH.unbind_image_texture(2);
		HL.unbind_image_texture(3);
		HH.unbind_image_texture(4);
	
	}



	void do_IDWT(Texture& input, Texture& output, int decomp_lvl) {
		ShaderProgram conv_0 = do_IDWT_prog(decomp_lvl, 0);
		ShaderProgram conv_1 = do_IDWT_prog(decomp_lvl, 1);

		int tex_size = image_width / pow(2, decomp_lvl - 1);

		Texture LL(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
		Texture LH(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
		Texture HL(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
		Texture HH(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);

		Texture LL_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
		Texture LH_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
		Texture HL_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
		Texture HH_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);

		upsample(input, LL, decomp_lvl, 0, 0);
		upsample(input, LH, decomp_lvl, 0, 1);
		upsample(input, HL, decomp_lvl, 1, 0);
		upsample(input, HH, decomp_lvl, 1, 1);


		convolve(LL, LL_b, decomp_lvl, 0);
		transpose(LL, LL_b, decomp_lvl);
		convolve(LL, LL_b,  decomp_lvl, 0);
		transpose(LL, LL_b, decomp_lvl);

		convolve(LH, LH_b, decomp_lvl, 0);
		transpose(LH, LH_b, decomp_lvl);
		convolve(LH, LH_b, decomp_lvl, 1);
		transpose(LH, LH_b, decomp_lvl);

		convolve(HL, HL_b, decomp_lvl, 1);
		transpose(HL, HL_b, decomp_lvl);
		convolve(HL, HL_b, decomp_lvl, 0);
		transpose(HL, HL_b, decomp_lvl);

		convolve(HH, HH_b, decomp_lvl, 1);
		transpose(HH, HH_b, decomp_lvl);
		convolve(HH, HH_b, decomp_lvl, 1);
		transpose(HH, HH_b, decomp_lvl);

		add_IDWT_subbands(input, LL, LH, HL, HH, decomp_lvl);



		input.unbind_image_texture(0);
		output.unbind_image_texture(1);

		input.unbind_texture(0);
		output.unbind_texture(1);
	}
};


#endif