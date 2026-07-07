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


using std::string;


class ShaderProgram {

public:

	unsigned int ID;

	ShaderProgram(const char* vertex_shader, const char* fragment_shader);
	//ShaderProgram(const char* compute_shader, int num_samples, int samples_per_processor);
	ShaderProgram(const char* compute_shader, int num_samples, int samples_per_processor);
	ShaderProgram() = delete;

	void use_shader_prog();

	void delete_shader_prog();

	static void set_uniform(unsigned int shader_id, string uniformName, glm::vec3 vector);
	static void set_uniform(unsigned int shader_id, string uniformName, glm::mat4 matrix);
	static void set_uniform(unsigned int shader_id, string uniformName, float value);
	static void set_uniform(unsigned int shader_id, string uniformName,unsigned int value);

	static int num_bits(unsigned int number);

};


#endif