#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <array>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <algorithm>

#include "../Header_files/ShaderProgram.h"
#include "../Header_files/ShaderSource.h"
#include "../Header_files/VAO.h"
#include "../Header_files/VBO.h"
#include "../Header_files/Texture.h"
#include "../Header_files/DiscreteWaveletTransform.h"


#define M_PI 3.14159265358979323846

using std::vector, std::string, std::cout, std::endl;


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

	string v = "Vertex_shaders\\vertex_shader.glsl";
	string f = "Fragment_shaders\\display_DWT.glsl";

	ShaderProgram sh(v, f);

	VAO vao;
	vao.bind_VAO();
	VBO vbo(plane_img, sizeof(plane_img));
	vao.link_VBO(vbo, 2, 2);
	vao.unbind_VAO();
	vbo.unbind_VBO();


	//-------------------------------------------------------------------------------------------------------- LOAD INPUT IMAGE
	
	string pathToImage = "C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\CAMERAMAN_ORIGINAL.png";
	
	
	Texture image_0(GL_RGBA32F, GL_RGBA, pathToImage, 256, 256);
	ShaderProgram::undo_gamma_correction(image_0, 256);

	//-------------------------------------------------------------------------------------------------------- GENERATE GAUSSIAN NOISE

	Texture noise_u_0 = Texture{};	//uniform noise
	Texture noise_u_1 = Texture{};	//uniform noise
	Texture AWGN = Texture{};		// white Gaussian noise
	Texture deviations = Texture{};

	AddGaussianNoise::generate_uniform_noise(noise_u_0);
	AddGaussianNoise::generate_uniform_noise(noise_u_1);
	AddGaussianNoise::apply_Gaussian_noise(noise_u_0, noise_u_1, image_0, AWGN, 0.04);

	//-------------------------------------------------------------------------------------------------------- COMPUTE FILTER COEFFS 

	//constexpr Filter filter_vals{ 0.4829629131f, 0.8365163037f, 0.224143868f, -0.1294095226f };
	//DB2 COEFFS lowpass synthesis : 0.4829629131f, 0.8365163037f, 0.224143868f, -0.1294095226f
	//DB3 COEFFS lowpass synthesis : 0.3326705530f, 0.8068915093f, 0.4598775021f, -0.1350110200f, -0.0854412739f, 0.0352262919f
	//DB4 COEFFS lowpass synthesis : 0.2303778133f, 0.7148465706f, 0.6308807679f, -0.0279837694f, -0.1870348117f, 0.0308413818f, 0.0328830117f, -0.0105974018f
	//DB5 COEFFS lowpass synthesis : 0.160102397f, 0.603829265f, 0.72430855f, 0.138428152f, -0.242294878f, -0.0322448686f, 0.0775714889f, -0.00624149013f, -0.0125807514f, 0.00333572528f
	constexpr GetFilter filter_vals{ 0.160102397f, 0.603829265f, 0.72430855f, 0.138428152f, -0.242294878f, -0.0322448686f, 0.0775714889f, -0.00624149013f, -0.0125807514f, 0.00333572528f };
	std::array<float, 10> h00 = filter_vals.h0;
	std::array<float, 10> h11 = filter_vals.h1_w;
	std::array<float, 10> g00 = filter_vals.g0_w;
	std::array<float, 10> g11 = filter_vals.g1_w;
	
	DiscreteWaveletTransform w0{ 256, h00, h11, g00, g11 };

	Texture buff_tex = Texture{};
	
	//-------------------------------------------------------------------------------------------------------- DO DWT

	w0.do_DWT(image_0, buff_tex, 1);
	w0.do_DWT(image_0, buff_tex, 2);
	w0.do_DWT(image_0, buff_tex, 3);
//	w0.do_DWT(image_0, buff_tex, 4);
//	w0.do_DWT(image_0, buff_tex, 5);

	//-------------------------------------------------------------------------------------------------------- NORMAL SHRINK WITH SOFT THRESHOLDING

	Texture sft_1(GL_RGBA32F, GL_RGBA, "", 256, 256);
	Texture sft_2(GL_RGBA32F, GL_RGBA, "", 256, 256);
	Texture sft_3(GL_RGBA32F, GL_RGBA, "", 256, 256);
	Texture sft_4(GL_RGBA32F, GL_RGBA, "", 256, 256);
	Texture sft_5(GL_RGBA32F, GL_RGBA, "", 256, 256);

//	w0.sort_subbands(image_0, sft_5, 5, 1);
//	w0.apply_soft_threshold(image_0, sft_5, 5, 1);

//	w0.sort_subbands(image_0, sft_4, 4, 2);
//	w0.apply_soft_threshold(image_0, sft_4, 4, 2);

//	w0.sort_subbands(image_0, sft_3, 3, 4);
//	w0.apply_soft_threshold(image_0, sft_3, 3, 4);

	w0.sort_subbands(image_0, sft_2, 2, 4);
	w0.apply_soft_threshold(image_0, sft_2, 2, 4);

	w0.sort_subbands(image_0, sft_1, 1, 16);
	w0.apply_soft_threshold(image_0, sft_1, 1, 16);
// 
//-------------------------------------------------------------------------------------------------------- DO IDWT

//	w0.do_IDWT(image_0, buff_tex, 5);
//	w0.do_IDWT(image_0, buff_tex, 4);	
	w0.do_IDWT(image_0, buff_tex, 3);
	w0.do_IDWT(image_0, buff_tex, 2);
	w0.do_IDWT(image_0, buff_tex, 1);
	
	
	

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
		ShaderProgram::set_uniform(sh.ID, "filterTexture", (unsigned int)7);

		vao.bind_VAO();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		if (save) {
	
		//	saveImg("C:\\Users\\Toms\\Desktop\\OpenGL\\WaveletTransform\\Description_images\\DB5_3_LVL_DWT_DENOISE_LVL_1_2_16_4.png");

		}


		glfwSwapBuffers(window);

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}