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

	string v = "vert.vs";
	string f = "source_img_frag.fs";

	ShaderProgram sh(v.c_str(), f.c_str());


	VAO vao;
	vao.bind_VAO();
	VBO vbo(plane_img, sizeof(plane_img));
	vao.link_VBO(vbo, 2, 2);
	vao.unbind_VAO();
	vbo.unbind_VBO();

	string pathToImage = "C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\CAMERAMAN_ORIGINAL.png";

	Texture input_img(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);
	Texture input_test_offset(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);
	Texture output_1 = Texture{};
	Texture output_2 = Texture{};
	//-------------------------------------gen DWT matrix 
	ShaderProgram comp_dwt_matrix("generate_transform.glsl", 256, 1);
	ShaderProgram comp_idwt_matrix("generate_inverse_transform.glsl", 256, 1);

	//------------------------------------NOISE TEXTURE

	create_noise_0();
	create_noise_1();

	unsigned int ID;
	glGenTextures(1, &ID);
	cout << "NOISE TEXTURE id = " << ID << '\n';

	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 256, 0, GL_RGBA, GL_FLOAT, img_noise);

	unsigned int ID_1;
	glGenTextures(1, &ID_1);
	cout << "NOISE TEXTURE id = " << ID_1 << '\n';

	glBindTexture(GL_TEXTURE_2D, ID_1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 256, 0, GL_RGBA, GL_FLOAT, img_noise_1);



	//------------------------------------NOISE TEXTURE
	// 
	//---------------------------------------------------BITONIC SORT

	ShaderProgram bitonic_sort_comp("bitonic_sort.glsl", 256, 1);
	//ShaderProgram compute_prog_inv("haar_inv.cs", 256, 2);

	
	Texture sorted = Texture{};
	

	Texture::activate_tex_unit(0);
	input_img.bind_texture();
	input_img.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	sorted.bind_texture();
	sorted.bind_image_2D(1);

	bitonic_sort_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//---------------------------------------------------BITONIC SORT

	//----------------------------------------GENERATE NOISE

	ShaderProgram noise_comp("noise_0_comp_sh.glsl", 256, 1);
	//ShaderProgram compute_prog_inv("haar_inv.cs", 256, 2);
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
	ShaderProgram add_noise("add_noise_to_img.glsl", 256, 1);
	Texture image_0(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);
	Texture noised = Texture{};
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

	Texture dwt_mat = Texture{};
	Texture::activate_tex_unit(0);
	dwt_mat.bind_texture();
	dwt_mat.bind_image_2D(0);

	comp_dwt_matrix.use_shader_prog();
	glDispatchCompute((unsigned int)1, (unsigned int)128, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//-------------------------------------gen DWT matrix 

	ShaderProgram compute_prog("DB_2.cs", 256, 1);
	//ShaderProgram compute_prog_inv("haar_inv.cs", 256, 2);


	//--TEST
	ShaderProgram test_comp("testing_1.glsl", 256, 1);
	//ShaderProgram compute_prog_inv("haar_inv.cs", 256, 2);
	Texture t_1 = Texture{};
	Texture t_2 = Texture{};

	Texture::activate_tex_unit(0);
	input_img.bind_texture();
	input_img.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	dwt_mat.bind_texture();
	dwt_mat.bind_image_2D(1);

	Texture::activate_tex_unit(2);
	t_1.bind_texture();
	t_1.bind_image_2D(2);

	test_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//--TEST

	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";

	string path_trans_v = "transpose.cs";
	ShaderProgram rot(path_trans_v.c_str(), 256, 4);

	Texture::activate_tex_unit(7);
	t_1.bind_texture();
	t_1.bind_image_2D(0);

	Texture::reset_to_base(input_img);
	Texture::activate_tex_unit(8);
	input_img.bind_texture();
	input_img.bind_image_2D(1);

	rot.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";


	Texture::activate_tex_unit(0);
	input_img.bind_texture();
	input_img.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	dwt_mat.bind_texture();
	dwt_mat.bind_image_2D(1);

	Texture::reset_to_base(t_1);
	Texture::activate_tex_unit(2);
	t_1.bind_texture();
	t_1.bind_image_2D(2);

	test_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256 / 1), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";
	
	Texture::activate_tex_unit(7);
	t_1.bind_texture();
	t_1.bind_image_2D(0);

	Texture::reset_to_base(input_img);
	input_img.bind_texture();
	input_img.bind_image_2D(1);

	rot.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";
	/*
	Texture dwt_mat_inv = Texture{};
	Texture dwt_mat_inv_flip = Texture{};
	Texture::activate_tex_unit(7);
	dwt_mat.bind_texture();
	dwt_mat.bind_image_2D(0);


	Texture::activate_tex_unit(8);
	dwt_mat_inv.bind_texture();
	dwt_mat_inv.bind_image_2D(1);

	rot.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";
	
	Texture::activate_tex_unit(0);
	t_1.bind_texture();
	t_1.bind_image_2D(0);

	Texture::activate_tex_unit(8);
	dwt_mat_inv.bind_texture();
	dwt_mat_inv.bind_image_2D(1);

	Texture::reset_to_base(input_img);
	Texture::activate_tex_unit(1);
	input_img.bind_texture();
	input_img.bind_image_2D(2);
	
	test_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	
	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";
	
	Texture::activate_tex_unit(7);
	input_img.bind_texture();
	input_img.bind_image_2D(0);

	Texture::reset_to_base(t_1);
	Texture::activate_tex_unit(8);
	t_1.bind_texture();
	t_1.bind_image_2D(1);

	rot.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	//------------------------------------------------------------------------------------------TRANSPOSE
	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";

	Texture::activate_tex_unit(7);
	input_img.bind_texture();
	input_img.bind_image_2D(0);

	Texture::reset_to_base(t_1);
	Texture::activate_tex_unit(8);
	t_1.bind_texture();
	t_1.bind_image_2D(1);

	rot.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	//------------------------------------------------------------------------------------------TRANSPOSE
	// 	string path_trans_v = "transpose.cs";
	
	Texture::activate_tex_unit(0);
	t_1.bind_texture();
	t_1.bind_image_2D(0);

	Texture::activate_tex_unit(8);
	dwt_mat_inv.bind_texture();
	dwt_mat_inv.bind_image_2D(1);

	Texture::reset_to_base(input_img);
	Texture::activate_tex_unit(1);
	input_img.bind_texture();
	input_img.bind_image_2D(2);

	test_comp.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256 / 1), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	

	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";
		//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";
	/*
	Texture::activate_tex_unit(7);
	output_1.bind_texture();
	output_1.bind_image_2D(0);

	Texture::reset_to_base(input_img);
	Texture::activate_tex_unit(8);
	input_img.bind_texture();
	input_img.bind_image_2D(1);

	rot.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(256), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	*/
	//------------------------------------------------------------------------------------------TRANSPOSE	string path_trans_v = "transpose.cs";
	






	if (window == NULL) {

		glfwTerminate();

	}




	map<string, int> result;

	result.emplace("str", 5);
	result.emplace("str1", 6);
	result.emplace("str2", 7);

	Wavelet::Wavelets ha = Wavelet::Wavelets::HAAR;

	cout << "map : " << result["str"];
	cout << "namespace A::a = " << A::a << '\n';
	cout << ha << "This is haar";
	unsigned int num_cores = std::thread::hardware_concurrency();
	cout << "NUM AVAILABLE CORES ON MY CPU = " << num_cores << '\n';
	uint32_t rands[12];

	auto random_v = [&rands](uint32_t seed, int index) {seed ^= seed << 17; seed ^= seed >> 13; seed ^= seed << 5; rands[index] = seed; cout << "Rand_val : " << seed << '\n'; return seed; };
	vector<thread> thrds(num_cores);
	for (int i = 0; i < 12; i++) {
		thrds[i] = thread(random_v, i + 1, i);
		
	}
	for_each(thrds.begin(), thrds.end(), [](std::thread& t) {
		t.join();
	});


	//cout << "RANDOM VALUE FORM LAMBDA : " << random_v(128) << '\n';



	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		glClearColor(0.156f, 0.427f, 0.482f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		sh.use_shader_prog();
		glActiveTexture(GL_TEXTURE7);
		sorted.bind_texture();
		//glBindTexture(GL_TEXTURE_2D, ID_1);
		ShaderProgram::set_uniform(sh.ID, "filterTexture", (unsigned int)7);

		vao.bind_VAO();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		if (save) {

		//	saveImg("C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\image_with_noise_GRNG.png");

		}

		glfwSwapBuffers(window);

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}