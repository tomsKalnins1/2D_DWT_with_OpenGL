#include "../Header_files/ShaderProgram.h"
#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include <random>
#include <cstdlib>

AddGaussianNoise::AddGaussianNoise(string source_file_path, float omega, int img_dimension_width) {
	
	GaussianNoise gn(source_file_path, omega);

	string source_code = gn.set_compute_shader_values(source_file_path);

	//	std::cout << "WAVELET TRANSFORM SHADER SOURCE UPSAMPLE : \n" << source_code << '\n';

	const char* shader_source_char = source_code.c_str();


	gn.ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(gn.ID, 1, &shader_source_char, NULL);
	glCompileShader(gn.ID);

	GLint compiled;
	glGetShaderiv(gn.ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(gn.ID, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + source_file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, gn.ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << source_file_path << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(gn.ID);

}

uint32_t AddGaussianNoise::rot_bits_left_32(uint32_t in, uint32_t s) {

	return (in << s) | (in >> (32 - s)) & 0xFFFF;

}

uint32_t AddGaussianNoise::xoshiro_32(std::vector<uint32_t>& seed) {
	uint32_t res = rot_bits_left_32(seed[1] * 5, 7) * 9;
	uint32_t t = (seed[1]) << 17;

	seed[2] ^= seed[0];
	seed[3] ^= seed[1];
	seed[1] ^= seed[2];
	seed[0] ^= seed[3];

	seed[2] ^= t;
	seed[3] ^= rot_bits_left_32(seed[3], 45);

	return res;
}

std::vector<float> AddGaussianNoise::generate_rands_xoshiro_32(int num_rands, int base) {

	std::vector<float> rands(num_rands, 0.0f);
	int num_proc = std::thread::hardware_concurrency();
	std::vector<std::thread> thrds(num_proc);
	int mod = num_rands % num_proc;
	int num_rands_per_proc = (num_rands / 4 ) / num_proc;
	std::cout << "NUM RANDS PER PROC = " << num_rands_per_proc << '\n';
//	int mod = num;
	int add = 0;

	struct Xoshiro {
		std::vector<uint32_t> s;

		Xoshiro() : s(4) {}
	};

	if (mod) {
		add = mod;
	}

	float normalize = pow(2.0, 32.0) - 1;

	for (int i = 0; i < num_proc; i++) {

		Xoshiro x;
		std::seed_seq s{ base + i, base + i, base + i, base + i };
		std::vector<uint32_t> seeds(4);
		s.generate(seeds.begin(), seeds.end());
		x.s = seeds;

		thrds[i] = std::thread([&rands, &normalize, num_proc, num_rands_per_proc, add, x, i] mutable {

			int add_0 = (i == num_proc - 1) ? add : 0;

			for (int k = 0; k < num_rands_per_proc + add_0; k++) {

				rands[(i * num_rands_per_proc + k ) * 4] = ((float) xoshiro_32(x.s)) / normalize;
				float val = rands[(i * num_rands_per_proc + k) * 4];

			}

			});
	}

	for (int i = 0; i < num_proc; i++) {

		thrds[i].join();

	}


	return rands;

}

void AddGaussianNoise::generate_uniform_noise(Texture& output, int base) {

	int size = output.width * output.width * 4;
	
	std::vector<float> n_0 = generate_rands_xoshiro_32(size, base);

	glGenTextures(1, &output.ID);

	glBindTexture(GL_TEXTURE_2D, output.ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, output.width, output.width, 0, GL_RGBA, GL_FLOAT, n_0.data());


}

void AddGaussianNoise::apply_Gaussian_noise(Texture& u_0, Texture& u_1, Texture& input, Texture& output, float deviation) {
	
	AddGaussianNoise agn("Compute_shaders\\Add_Gaussian_Noise_DYNAMIC.glsl", deviation, output.width);

	Texture::activate_tex_unit(0);
	u_0.bind_texture();
	u_0.bind_image_2D(0);

	Texture::activate_tex_unit(1);
	u_1.bind_texture();
	u_1.bind_image_2D(1);

	Texture::activate_tex_unit(2);
	output.bind_texture();
	output.bind_image_2D(2);

	Texture::activate_tex_unit(3);
	input.bind_texture();
	input.bind_image_2D(3);

	agn.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(output.width), (unsigned int)ceil(output.width), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	u_0.unbind_image_texture(0);
	u_0.unbind_image_texture(1);
	output.unbind_image_texture(2);
	input.unbind_image_texture(3);
	u_1.unbind_texture(0);
	u_1.unbind_texture(1);
	output.unbind_texture(2);
	input.unbind_texture(2);

}