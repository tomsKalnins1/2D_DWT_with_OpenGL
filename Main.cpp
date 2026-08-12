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
	Texture noise_u_0 = Texture{};
	Texture noise_u_1 = Texture{};

	AddGaussianNoise::generate_uniform_noise(noise_u_0, 1);
	AddGaussianNoise::generate_uniform_noise(noise_u_1, 2);



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

	AddGaussianNoise::apply_Gaussian_noise(noise_u_0, noise_u_1, image_0, noised, 0.02);


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
	
	//------------------------------ TEST TRANSPOSE --------- TEST TRANSPOSE---- TEST TRANSPOSE
	Texture sub_sub = Texture{};

	Texture sft_1(GL_RGBA32F, GL_RGBA, "", 256, 256);
	Texture sft_2(GL_RGBA32F, GL_RGBA, "", 128, 128);
	Texture sft_3(GL_RGBA32F, GL_RGBA, "", 64, 64);
	Texture sft_4(GL_RGBA32F, GL_RGBA, "", 32, 32);

	
	WaveletTransform::do_DWT(image_0, t_1, 1, 256);
	WaveletTransform::do_DWT(image_0, t_1, 2, 256);
	WaveletTransform::do_DWT(image_0, t_1, 3, 256);
	WaveletTransform::do_DWT(image_0, t_1, 4, 256);
	/*
	WaveletTransform::sort_subbands(image_0, sft_1, 1, 256, 16);
	WaveletTransform::apply_soft_threshold(image_0, sft_1, 1, 256, 16);

	WaveletTransform::sort_subbands(image_0, sft_2, 2, 256, 8);
	WaveletTransform::apply_soft_threshold(image_0, sft_2, 2, 256, 8);

	WaveletTransform::sort_subbands(image_0, sft_3, 3, 256, 4);
	WaveletTransform::apply_soft_threshold(image_0, sft_3, 3, 256, 4);


	WaveletTransform::sort_subbands(image_0, sft_4, 4, 256, 2);
	WaveletTransform::apply_soft_threshold(image_0, sft_4, 4, 256, 2);
	*/
	/*
	WaveletTransform::sort_subbands(image_0, LL, 2, 256, 8);
	WaveletTransform::apply_soft_threshold(image_0, LL, 2, 256, 8);

	WaveletTransform::sort_subbands(image_0, LL, 1, 256, 16);
	WaveletTransform::apply_soft_threshold(image_0, LL, 1, 256, 16);
	*/


	
	WaveletTransform::do_inverse_DWT(image_0, t_1, 4, 256);

	WaveletTransform::do_inverse_DWT(image_0, t_1, 3, 256);

	WaveletTransform::do_inverse_DWT(image_0, t_1, 2, 256);
	WaveletTransform::do_inverse_DWT(image_0, t_1, 1, 256);

	

//	Texture::reset_to_base(image_0);





	if (window == NULL) {

		glfwTerminate();

	}





	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		glClearColor(0.156f, 0.427f, 0.482f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		sh.use_shader_prog();
		glActiveTexture(GL_TEXTURE7);
		image_0.bind_texture();
		//glBindTexture(GL_TEXTURE_2D, ID_1);
		ShaderProgram::set_uniform(sh.ID, "filterTexture", (unsigned int)7);

		vao.bind_VAO();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		if (save) {
			
		//	saveImg("C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\Test_debug_images\\XOSHIRO_32_NOISE_DWT_IDWT_NOISE_LVL_1_TO_4.png");

		}


		glfwSwapBuffers(window);

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}