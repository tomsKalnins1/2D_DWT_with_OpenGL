#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H


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
#include "Texture.h"


using std::string;


class ShaderProgram {

public:
	enum type_of_shader {

		TRANSPOSE_REGION = 0,
		APPLY_TRANSFORM = 1,
		APPLY_SOFT_THRESHOLD = 2,
		GENERATE_TRANSFORM = 3,
		SORT_SUBBAND = 4

		

	};

	unsigned int ID;

	ShaderProgram(string vertex_shader, string fragment_shader);
	ShaderProgram(string file_path, int decomposition_level, int img_dimension_width, int size_filter, type_of_shader type);
	ShaderProgram(string file_path, int decomposition_level, int img_dimension_width, type_of_shader type);
	ShaderProgram(string file_path, int decomposition_level, int H_L);
	ShaderProgram(string file_path);

	ShaderProgram() = delete;

	void use_shader_prog();

	void delete_shader_prog();

	static void set_uniform(unsigned int shader_id, string uniformName, glm::vec3 vector);
	static void set_uniform(unsigned int shader_id, string uniformName, glm::mat4 matrix);
	static void set_uniform(unsigned int shader_id, string uniformName, float value);
	static void set_uniform(unsigned int shader_id, string uniformName,unsigned int value);

	static int num_bits(unsigned int number);

};

class WaveletTransform : public ShaderProgram {

public :
	/*
	add filter coeff arrays values of which are computed at compile time,
	for now the coeffs are in the filters, MUST REMEMBER TO CHANGE THAT !!!
	* h0 filter coeffs (analysis)
	* h1 filter coeffs (analysis)
	* g0 filter coeffs (analysis)
	* g1 filter coeffs (analysis)
	
	ALSO MUST ADD A CONSTRUCTOR THAT SPECIFIES THE SIZE AND VALUES OF LOW PASS ANALYSIS FILTER TO BE USED (do I provide set of h0 filter to be used or should it be specified by the constructor call ?)
	MUST SOLVE THIS !!!
	WaveletTransform(tring file_path, int max_level_of_decomp, int img_width, )
	*/

	WaveletTransform(string file_path) : ShaderProgram(file_path) {}
	WaveletTransform(string file_path, int decomposition_level, int img_dimension_width, int size_filter, type_of_shader type) : ShaderProgram(file_path, decomposition_level, img_dimension_width, size_filter, type) {}
	WaveletTransform(string file_path, int decomposition_level, int img_dimension_width, type_of_shader type) : ShaderProgram(file_path, decomposition_level, img_dimension_width, type) {}


	static void transpose(Texture& img, int decomposition_level, int img_width);
	static void upsample(Texture& dwt, Texture& subband, int decomposition_level, int img_width, int H_L_0, int H_L_1);
	static void convolve(Texture& input, Texture& output, int img_width, int H_L);
	static void do_DWT(Texture& input, Texture& output, int level, int img_width);
	static void do_inverse_DWT(Texture& input, Texture& output, int level, int img_width);
	



};


#endif