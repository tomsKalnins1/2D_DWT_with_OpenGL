#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <map>

#include <thread>
#include <mutex>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <algorithm>

#include "ShaderProgram.h"

#include "VAO.h"
#include "VBO.h"
#include "Texture.h"
#include "Wavelet.h"


#define M_PI 3.14159265358979323846

using std::vector, std::string, std::cout, std::endl, std::mutex, std::thread;

mutex m;




namespace A {
	int a = 100;
}

bool save = true;

void saveImg(string path) {

	GLsizei nrChannels = 3;
	GLsizei stride = nrChannels * 1024;
	stride += (stride % 4) ? (4 - stride % 4) : 0;
	GLsizei bufferSize = stride * 1024;
	vector<char> buffer(bufferSize);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, 1024, 1024, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
	stbi_flip_vertically_on_write(true);
	stbi_write_png(path.c_str(), 1024, 1024, 3, buffer.data(), stride);

}
mutex mut;

float img_noise[256 * 256 * 4];
float img_noise_1[256 * 256 * 4];


void xor_shift(float& seed, int th_id) {
	uint32_t int_ptr = reinterpret_cast<uintptr_t>(&seed);
	int_ptr *= (th_id + 1);
	int_ptr ^= int_ptr << 13;
	int_ptr ^= int_ptr >> 17;
	int_ptr ^= int_ptr << 5;

	
	img_noise[th_id ] = ((float)int_ptr) / 4294967295.0f;
	img_noise[th_id + 1] = ((float)int_ptr) / 4294967295.0f;
	img_noise[th_id + 2] = ((float)int_ptr) / 4294967295.0f;
	img_noise[th_id + 3] = 1.0f;

}

void xor_shift_1(float& seed, int th_id) {
	uint32_t int_ptr = reinterpret_cast<uintptr_t>(&seed);
	int_ptr *= (th_id + 1);
	int_ptr ^= int_ptr << 13;
	int_ptr ^= int_ptr >> 17;
	int_ptr ^= int_ptr << 5;


	img_noise_1[th_id] = ((float)int_ptr) / 4294967295.0f;
	img_noise_1[th_id + 1] = ((float)int_ptr) / 4294967295.0f;
	img_noise_1[th_id + 2] = ((float)int_ptr) / 4294967295.0f;
	img_noise_1[th_id + 3] = 1.0f;

}

void create_noise_0() {
	for (int i = 0; i < 256 * 256; i++) {
		xor_shift(img_noise[i * 4], i * 4);
	//	thread t(xor_shift, std::ref(img_noise[i * 4]), i * 4);
	//	t.join();
	
	}
}
void create_noise_1() {
	for (int i = 0; i < 256 * 256; i++) {
		xor_shift_1(img_noise_1[i * 4], i * 4);

		//	thread t(xor_shift, std::ref(img_noise[i * 4]), i * 4);
		//	t.join();

	}
}

struct Subband {

	int H_L_0;
	int H_L_1;

};

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(1024, 1024, "DWT", NULL, NULL);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, 1024, 1024);

	float plane_img[] = {

		// coords    // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f

	};

	string v = "vertex_shader.vs";
	string f = "display_DWT.fs";

	ShaderProgram sh(v, f);


	VAO vao;
	vao.bind_VAO();
	VBO vbo(plane_img, sizeof(plane_img));
	vao.link_VBO(vbo, 2, 2);
	vao.unbind_VAO();
	vbo.unbind_VBO();

	string pathToImage = "C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\SHREK.png";

	Texture input_img(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);
	Texture input_test_offset(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);
	Texture output_1 = Texture{};
	Texture output_2 = Texture{};
	//-------------------------------------gen DWT matrix 



	//------------------------------------NOISE TEXTURE

	create_noise_0();
	create_noise_1();

	unsigned int ID;
	glGenTextures(1, &ID);


	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 256, 0, GL_RGBA, GL_FLOAT, img_noise);

	unsigned int ID_1;
	glGenTextures(1, &ID_1);

	glBindTexture(GL_TEXTURE_2D, ID_1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 256, 0, GL_RGBA, GL_FLOAT, img_noise_1);



	//------------------------------------NOISE TEXTURE
	// 


	//----------------------------------------GENERATE NOISE

	ShaderProgram noise_comp("Box_Muller_noise_NRNG.glsl");

	Texture noise = Texture{};
	Texture n_0 = Texture{};
	Texture n_1 = Texture{};
	n_0.ID = ID;
	n_1.ID = ID_1;

	Texture::activate_tex_unit(0);
	n_0.bind_texture();
	n_0.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	n_0.bind_texture();
	n_1.bind_image_2D(1);

	Texture::activate_tex_unit(2);
	noise.bind_texture();
	noise.bind_image_2D(2);

	noise_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(256), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//----------------------------------------GENERATE NOISE

	//------------------------------------ADD NOISE
	ShaderProgram add_noise("add_noise_to_image.glsl");
	Texture image_0(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);
	Texture noised = Texture{};

	Texture empty = Texture{};

	Texture::activate_tex_unit(0);
	image_0.bind_texture();
	image_0.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	noise.bind_texture();
	noise.bind_image_2D(1);

	Texture::activate_tex_unit(2);
	noised.bind_texture();
	noised.bind_image_2D(2);

	add_noise.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(256), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	//--------------------------------------------------GENERATE DWT MATRIX
	// 

	// 
	// 
	//--------------------------------------------------------------------------------------------DO DWT


	//--------------------------------------------------------------------------------LVL 1 TRANSFORM AND APPLY TRANSFORM COMP_SH


	WaveletTransform apply_mat("apply_DWT_matrix_DYNAMIC.glsl", 1, 256, WaveletTransform::type_of_shader::APPLY_TRANSFORM);
	//WaveletTransform conv_shader("apply_IDWT_matrix_DYNAMIC.glsl", 0, 0);
	//	ShaderProgram apply_mat_inv("apply_IDWT_matrix_DYNAMIC.glsl", 1, 256, ShaderProgram::type_of_shader::APPLY_TRANSFORM);
		//--------------------------------------------------------------------------------LVL 1 TRANSFORM AND APPLY TRANSFORM COMP_SH
		//--------------------------------------------------------------------------------LVL 2 TRANSFORM AND APPLY TRANSFORM COMP_SH




		//ShaderProgram apply_mat_2("apply_DWT_matrix_DYNAMIC.glsl", 2, 256);
	WaveletTransform apply_mat_2("apply_DWT_matrix_DYNAMIC.glsl", 2, 256, WaveletTransform::type_of_shader::APPLY_TRANSFORM);
	//--------------------------------------------------------------------------------LVL 2 TRANSFORM AND APPLY TRANSFORM COMP_SH

	ShaderProgram rot("transpose.glsl");
	WaveletTransform bitonic_sort_comp("Sort_subband_LOCAL_DYNAMIC.glsl", 1, 256, 16, WaveletTransform::SORT_SUBBAND);
	WaveletTransform bitonic_sort_comp_2("Sort_subband_LOCAL_DYNAMIC.glsl", 2, 256, 8, WaveletTransform::SORT_SUBBAND);
	ShaderProgram soft_thr_comp("Soft_thresholding_remove_noise_LOCAL.glsl");
	ShaderProgram soft_thr_comp_2("Soft_thresholding_remove_noise_LOCAL_2.glsl");



	Texture sorted_HH = Texture{};
	Texture sorted_HH_2 = Texture{};
	Texture dwt_mat = Texture{};
	Texture dwt_mat_inv = Texture{};
	Texture dwt_mat_inv_flip = Texture{};
	Texture test_Tn = Texture{};
	Texture t_1 = Texture{};
	Texture t_2 = Texture{};

	//SYNTHESIS

	Texture subband_buffer = Texture{};
	//------------------------------ TEST TRANSPOSE --------- TEST TRANSPOSE---- TEST TRANSPOSE 

	ShaderProgram transp_region("transpose_region.glsl");
	//ShaderProgram upsample("upsample.glsl");

	/*
	Texture::activate_tex_unit(7);
	input_img.bind_texture();
	input_img.bind_image_2D(0);

//	Texture::reset_to_base(t_2);
	Texture::activate_tex_unit(8);
	t_2.bind_texture();
	t_2.bind_image_2D(1);

	transp_region_dynamic_lvl_1.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(1), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	*/

	//------------------------------ TEST TRANSPOSE --------- TEST TRANSPOSE---- TEST TRANSPOSE
	Texture sub_sub = Texture{};

	bool do_DWT = true;
	if (do_DWT) {

		//--------------------------------------------------GENERATE DWT MATRIX

		WaveletTransform::do_DWT(noised, t_1, 1, 256);

	}
	

	//	WaveletTransform::transpose(noised, 2, 256);
	//	WaveletTransform::sort_subbands(noised, sorted_HH, 1, 256, 16);

	WaveletTransform::do_DWT(noised, t_1, 2, 256);
//	WaveletTransform::do_DWT(noised, t_1, 3, 256);


	WaveletTransform dwt("apply_DWT_matrix_DYNAMIC.glsl", 3, 256, WaveletTransform::type_of_shader::APPLY_TRANSFORM);

	Texture::activate_tex_unit(0);
	noised.bind_texture();
	noised.bind_image_2D(0);

	//Texture output_intermediate = Texture{};
//	Texture::reset_to_base(output);
	Texture::activate_tex_unit(1);
	t_1.bind_texture();
	t_1.bind_image_2D(1);


	dwt.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256 / pow(2, 2)), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	noised.unbind_texture(0);
	noised.unbind_image_texture(0);
	t_1.unbind_texture(1);
	t_1.unbind_image_texture(1);

	WaveletTransform::transpose(noised, t_1, 3, 256);

	Texture::activate_tex_unit(0);
	noised.bind_texture();
	noised.bind_image_2D(0);

	//	Texture::reset_to_base(input);
	Texture::activate_tex_unit(1);
	t_1.bind_texture();
	t_1.bind_image_2D(1);

	dwt.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256 / pow(2, 2)), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	noised.unbind_texture(1);
	noised.unbind_image_texture(1);
	t_1.unbind_texture(0);
	t_1.unbind_image_texture(0);

	WaveletTransform::transpose(noised, t_1, 3, 256);



	int tex_size = 256 / 1;

	Texture LL(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture LH(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HL(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HH(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);

	Texture LL_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture LH_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HL_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HH_b(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);

	Texture LL_b1(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture LH_b1(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HL_b1(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);
	Texture HH_b1(GL_RGBA32F, GL_RGBA, "", tex_size, tex_size);

//	std::cout << "1. \t SIZE OF HH SYNTHESIS = \t" << HH.height << "  SIZE OF HH_b = \t" << HH_b.height << '\n';
	WaveletTransform::do_inverse_DWT(noised, t_1, 3, 256);
	WaveletTransform::do_inverse_DWT(noised, t_1, 2, 256);
	WaveletTransform::do_inverse_DWT(noised, t_1, 1, 256);
	
	
	bool do_IDWT_2 = false;
	if(do_IDWT_2){
		WaveletTransform::upsample(noised, LL, 1, 256, 0, 0);

		WaveletTransform::upsample(noised, LH, 1, 256, 0, 1);
		WaveletTransform::upsample(noised, HL, 1, 256, 1, 0);
		WaveletTransform::upsample(noised, HH, 1, 256, 1, 1);

		WaveletTransform::convolve(LL, LL_b, 256, 1, 0);
		WaveletTransform::transpose(LL, LL_b1, 1, 256);
		WaveletTransform::convolve(LL, LL_b, 256, 1, 0);
		WaveletTransform::transpose(LL, LL_b1, 1, 256);

		WaveletTransform::convolve(LH, LH_b, 256, 1, 0);
		WaveletTransform::transpose(LH_b, LH, 1, 256);
		WaveletTransform::convolve(LH_b, LH, 256, 1, 1);
		WaveletTransform::transpose(LH, LH_b, 1, 256);

		WaveletTransform::convolve(HL, HL_b, 256, 1, 1);
		WaveletTransform::transpose(HL_b, HL, 1, 256);
		WaveletTransform::convolve(HL_b, HL, 256, 1, 0);
		WaveletTransform::transpose(HL, HL_b, 1, 256);

	//	std::cout << " 2. \t SIZE OF HH SYNTHESIS = \t" << HH.height << "  SIZE OF HH_b = \t" << HH_b.height << '\n';

		WaveletTransform::convolve(HH, HH_b, 256, 1, 1);
		WaveletTransform::transpose(HH_b, HH, 1, 256);
		WaveletTransform::convolve(HH_b, HH, 256, 1, 1);
		WaveletTransform::transpose(HH, HH_b, 1, 256);

//	std::cout << " 3. \t SIZE OF HH SYNTHESIS = \t" << HH.height << "  SIZE OF HH_b = \t" << HH_b.height << '\n';

	WaveletTransform::add_IDWT_subbands(noised, LL, LH, HL, HH, 1, 256);
	}
	

//	WaveletTransform::do_inverse_DWT(noised, t_1, 2, 256);
//	   WaveletTransform::do_inverse_DWT(noised, t_1, 1, 256);
	// Unbind all texture units

//	WaveletTransform::sort_subbands(noised, sorted_HH_2, 2, 256, 8);




	//------------------------------------------LVL 2 DWT FORWARD
	/*
	bool do_lvl_2_dwt = false;

	if (do_lvl_2_dwt) {
	//	WaveletTransform::do_DWT(noised, t_2, 2, 256);
	}

	//-LVL 2 ---------TRANSPOSE

	//-LVL 2 ---------SORT HH, LH, HL

	Texture::activate_tex_unit(0);
	noised.bind_texture();
	noised.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	sorted_HH_2.bind_texture();
	sorted_HH_2.bind_image_2D(1);

	bitonic_sort_comp_2.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(64), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//-LVL 2 ---------SORT HH, LH, HL


	Texture::activate_tex_unit(0);
	noised.bind_texture();
	noised.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	sorted_HH.bind_texture();
	sorted_HH.bind_image_2D(1);

	bitonic_sort_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(128), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//---------------------------------------------------BITONIC SORT
	//---------------------------------------------------COMPUTE AND APPLY THRESHOLD



	bool do_threshold = false;

	if (do_threshold) {
		Texture::activate_tex_unit(0);
		noised.bind_texture();
		noised.bind_image_2D(0);

		Texture::activate_tex_unit(1);
		sorted_HH.bind_texture();
		sorted_HH.bind_image_2D(1);

		Texture::activate_tex_unit(2);
		test_Tn.bind_texture();
		test_Tn.bind_image_2D(2);

		soft_thr_comp.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(128), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	}

	bool do_threshold_2 = false;

	if (do_threshold_2) {
		Texture::activate_tex_unit(0);
		t_2.bind_texture();
		t_2.bind_image_2D(0);

		Texture::activate_tex_unit(1);
		sorted_HH_2.bind_texture();
		sorted_HH_2.bind_image_2D(1);

		Texture::activate_tex_unit(2);
		test_Tn.bind_texture();
		test_Tn.bind_image_2D(2);

		soft_thr_comp_2.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(64), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	}

	*/
	//---------------------------------------------------COMPUTE AND APPLY THRESHOLD

	//----------------------------------LVL 2 INVERSE DWT
	/*
	bool do_IDWT_2 = false;

	if (do_IDWT_2) {

		Texture::activate_tex_unit(7);
		lvl_2_dwt.bind_texture();
		lvl_2_dwt.bind_image_2D(0);


		Texture::activate_tex_unit(8);
		lvl_2_dwt_inv.bind_texture();
		lvl_2_dwt_inv.bind_image_2D(1);

		transp_region_dynamic_lvl_1.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(1), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";

		Texture::activate_tex_unit(0);
		t_2.bind_texture();
		t_2.bind_image_2D(0);

		Texture::activate_tex_unit(8);
		lvl_2_dwt_inv.bind_texture();
		lvl_2_dwt_inv.bind_image_2D(1);

		Texture::reset_to_base(input_img);
		Texture::activate_tex_unit(1);
		input_img.bind_texture();
		input_img.bind_image_2D(2);

		apply_mat_2.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(128), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";

		Texture::activate_tex_unit(7);
		t_2.bind_texture();
		t_2.bind_image_2D(0);

		Texture::reset_to_base(t_1);
		Texture::activate_tex_unit(8);
		t_1.bind_texture();
		t_1.bind_image_2D(1);

		transp_region_dynamic_lvl_2.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(1), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//------------------------------------------------------------------------------------------TRANSPOSE

		Texture::activate_tex_unit(0);
		t_2.bind_texture();
		t_2.bind_image_2D(0);

		Texture::activate_tex_unit(8);
		lvl_2_dwt_inv.bind_texture();
		lvl_2_dwt_inv.bind_image_2D(1);

		Texture::reset_to_base(noised);
		Texture::activate_tex_unit(1);
		noised.bind_texture();
		noised.bind_image_2D(2);

		apply_mat_2.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(128), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//---------------------------------------------------------------------------------------	TRANSPOSE

		Texture::activate_tex_unit(7);
		t_2.bind_texture();
		t_2.bind_image_2D(0);

		Texture::reset_to_base(t_1);
		Texture::activate_tex_unit(8);
		t_1.bind_texture();
		t_1.bind_image_2D(1);

		transp_region_dynamic_lvl_2.use_shader_prog();
		glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(1), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";

	}
	*/

	//----------------------------------LVL 2 INVERSE DWT



	if (window == NULL) {

		glfwTerminate();

	}





	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		glClearColor(0.156f, 0.427f, 0.482f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		sh.use_shader_prog();
		glActiveTexture(GL_TEXTURE7);
		noised.bind_texture();
		//glBindTexture(GL_TEXTURE_2D, ID_1);
		ShaderProgram::set_uniform(sh.ID, "filterTexture", (unsigned int)7);

		vao.bind_VAO();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		if (save) {
			
		//	saveImg("C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\Test_debug_images\\SHREK_NOISE_IDWT_2.png");

		}


		glfwSwapBuffers(window);

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}