#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <map>
#include <array>

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
#include "ShaderSource.h"

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



template<class ...H>
class Filter {
	
public:
	std::array<float, sizeof ...(H)> h0;


	Filter() {}
	constexpr Filter(H ...arg) : h0{ static_cast<float>(arg) ... } {}

	constexpr std::array<float, sizeof ...(H)> get_highpass() const{
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

};



int main() {

	constexpr Filter fff{ 0.0352262919f, -0.0854412739f, -0.1350110200f, 0.4598775021f, 0.8068915093f, 0.3326705530f };
	constexpr auto high_p = fff.get_highpass();

//	constexpr Container<float> cont{0.0352262919f, 0.0352262919f};
// //constexpr auto fil = get_arr<float>(0.0352262919, -0.0854412739, -0.1350110200, 0.4598775021, 0.8068915093, 0.3326705530);

	for (int i = 0; i < 6; i++) {
		cout << "Filter compile time compute output h0 = " << fff.h0[i] << '\n';
	}

	for (int i = 0; i < 6; i++) {
		cout << "Filter compile time compute output h1 = " << high_p[i] << '\n';
	}

	//ValueV<std::array<float, 3>> vvv{ 1.55555f, 4.000123f, 4.000123f };

	//constexpr ValueV values[]{ ValueV{1.1f}, ValueV{2.1f}, ValueV{3.0f} };



	/*
	std::array<float, 6> arr = { 0.0352262919, -0.0854412739, -0.1350110200,
	 0.4598775021,  0.8068915093,  0.3326705530 };
	*/



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

	string pathToImage = "C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\CAMERAMAN_ORIGINAL.png";
	
	Texture input_img(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);

	//------------------------------------ ADD NOISE TEXTURE
	 
	Texture noise_u_0 = Texture{};
	Texture noise_u_1 = Texture{};

	AddGaussianNoise::generate_uniform_noise(noise_u_0, 1);
	AddGaussianNoise::generate_uniform_noise(noise_u_1, 2);
	
	Texture image_0(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);
	
	
	Texture noised = Texture{};

	AddGaussianNoise::apply_Gaussian_noise(noise_u_0, noise_u_1, image_0, noised, 0.04);

	//------------------------------------ ADD NOISE TEXTURE
	Texture t_1 = Texture{}; //dummy store texture
	
	WaveletTransform::do_DWT(image_0, t_1, 1, 256);
	WaveletTransform::do_DWT(image_0, t_1, 2, 256);
	WaveletTransform::do_DWT(image_0, t_1, 3, 256);
	WaveletTransform::do_DWT(image_0, t_1, 4, 256);

	Texture sft_1(GL_RGBA32F, GL_RGBA, "", 256, 256);
	Texture sft_2(GL_RGBA32F, GL_RGBA, "", 256, 256);
	Texture sft_3(GL_RGBA32F, GL_RGBA, "", 256, 256);
	Texture sft_4(GL_RGBA32F, GL_RGBA, "", 256, 256);
	

//	WaveletTransform::sort_subbands(image_0, sft_4, 4, 256, 1);
//	WaveletTransform::apply_soft_threshold(image_0, sft_4, 4, 256, 1);

	bool do_DENOISE = true;
	if (do_DENOISE) {

		WaveletTransform::sort_subbands(image_0, sft_4, 4, 256, 1);
		WaveletTransform::apply_soft_threshold(image_0, sft_4, 4, 256, 1);
		WaveletTransform::sort_subbands(image_0, sft_3, 3, 256, 1);
		WaveletTransform::apply_soft_threshold(image_0, sft_3, 3, 256, 1);

		WaveletTransform::sort_subbands(image_0, sft_2, 2, 256, 1);
		WaveletTransform::apply_soft_threshold(image_0, sft_2, 2, 256, 1);

		WaveletTransform::sort_subbands(image_0, sft_1, 1, 256, 8);
		WaveletTransform::apply_soft_threshold(image_0, sft_1, 1, 256, 8);
	}
	

	WaveletTransform::do_inverse_DWT(image_0, t_1, 4, 256);
	bool do_IDWT = true;
	if(do_IDWT) {
		WaveletTransform::do_inverse_DWT(image_0, t_1, 3, 256);

		WaveletTransform::do_inverse_DWT(image_0, t_1, 2, 256);
		WaveletTransform::do_inverse_DWT(image_0, t_1, 1, 256);
	}
	
	


	

		



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
		
			//saveImg("C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\IDWT_denoised\\!!!_DB_3_CAMERAMAN_LVL_1_2_3_DWT_IDWT_DENOISE_DEV_004.png");

		}


		glfwSwapBuffers(window);

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}