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
#include <cmath>

using std::string;

class ShaderSource {



	public:
		unsigned int ID;
		string path_to_shader = "";

		ShaderSource() {}
		ShaderSource(string path) : path_to_shader{ path } {}
//		ShaderSource(const ShaderSource&) = delete;
//		ShaderSource& operator=(const ShaderSource&) = delete;
		
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
//	GenerateTransform(const GenerateTransform&) = delete;
//	GenerateTransform& operator=(const GenerateTransform&) = delete;

	GenerateTransform(string file_path, int decomposition_level, int img_dimension_x, int size_filter);

	string set_compute_shader_values(string source) override;

};

class ApplyTransform : public ShaderSource {

public:

	int decomp_lvl;
	int img_dimension_x;
	int subband; //one of the 4 subbands 0 for L, 1 for H

	ApplyTransform() = delete;
//	ApplyTransform(const GenerateTransform&) = delete;
//	ApplyTransform& operator=(const GenerateTransform&) = delete;

	ApplyTransform(string file_path, int decomposition_level, int img_dimension_x);
	ApplyTransform(string file_path, int decomposition_level, int img_dimension_x, int H_L);


	string set_compute_shader_values(string source) override;
	string set_compute_shader_values_inverse(string source, int H_L);

};

class TransposeRegion : public ShaderSource {

public:

	int decomp_lvl;
	int img_dimension_x;

	TransposeRegion() = delete;

	TransposeRegion(string path, int decomposition_lvl, int img_dimension_x);

	string set_compute_shader_values(string source) override;


};

class SortSubbandLocal : public ShaderSource {

public:
	int decomp_lvl;
	int img_dimension_x;
	int num_partitions;

	SortSubbandLocal() = delete;

	SortSubbandLocal(string path, int decomposition_level, int img_dimension_x, int num_partions);

	string set_compute_shader_values(string source) override;


};

class AddIDWTSubbands : public ShaderSource {

public:
	int decomp_lvl;
	int img_dimension_x;
	int num_partitions;

	AddIDWTSubbands() = delete;

	AddIDWTSubbands(string path, int decomposition_level, int img_dimension_x);

	string set_compute_shader_values(string source) override;


};

class UpsampleSubband : public ShaderSource {

public:
	int decomp_lvl;
	int img_dimension_x;
	int num_partitions;
	int H_L_0;
	int H_L_1;

	UpsampleSubband() = delete;

	UpsampleSubband(string path, int decomposition_level, int img_dimension_x, int H_L_0, int H_L_1);

	string set_compute_shader_values(string source) override;


};

class SoftThreshold : public ShaderSource {

public:
	int decomp_lvl;
	int img_dimension_x;
	int num_partitions;




	SoftThreshold() = delete;

	SoftThreshold(string path, int decomposition_level, int img_dimension_x, int num_part);

	string set_compute_shader_values(string source) override;


};

class GaussianNoise : public ShaderSource {

public:
	float deviation;

	/*

	struct Xoshiro {
		std::vector<uint32_t> s;

		Xoshiro() : s(4) {}
	};
	*/

	GaussianNoise() = delete;

	GaussianNoise(string path, float deviation);

	string set_compute_shader_values(string source) override;


};



#endif