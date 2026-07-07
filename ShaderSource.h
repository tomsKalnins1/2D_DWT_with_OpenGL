#ifndef SHADER_H
#define SHADER_H


#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

using std::string;

class ShaderSource {

	public:
		unsigned int ID;
		ShaderSource() = delete;
		ShaderSource(const ShaderSource&) = delete;
		ShaderSource& operator=(const ShaderSource&) = delete;
		

		ShaderSource(string file_path, GLenum shader_type);
		ShaderSource(string file_path, GLenum shader_type, int num_samples, int samples_per_processor);

		static string get_file_content(const char* path);

		static string set_compute_shader_values(string source, int num_samples, int samples_per_processor);
		

};

#endif