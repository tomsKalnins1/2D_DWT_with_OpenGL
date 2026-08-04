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
		ShaderSource() {}
		ShaderSource(const ShaderSource&) = delete;
		ShaderSource& operator=(const ShaderSource&) = delete;
		
	//	ShaderSource(string file_path);
	//	ShaderSource(string file_path, GLenum shader_type);
	//	ShaderSource(string file_path, GLenum shader_type, int num_samples, int samples_per_processor);

		static string get_file_content(string path);

		virtual string set_compute_shader_values(string source) { return "";}
		

};

class GenerateTransform : public ShaderSource {

public:

	int decomp_lvl;
	int img_dimension_x;
	int size_of_filter;

	GenerateTransform() = delete;
	GenerateTransform(const GenerateTransform&) = delete;
	GenerateTransform& operator=(const GenerateTransform&) = delete;

	GenerateTransform(string file_path, int decomposition_level, int img_dimension_x, int size_filter);

	string set_compute_shader_values(string source) override;

};

class ApplyTransform : public ShaderSource {

public:

	int decomp_lvl;
	int img_dimension_x;

	ApplyTransform() = delete;
	ApplyTransform(const GenerateTransform&) = delete;
	ApplyTransform& operator=(const GenerateTransform&) = delete;

	ApplyTransform(string file_path, int decomposition_level, int img_dimension_x);

	string set_compute_shader_values(string source) override;

};

#endif