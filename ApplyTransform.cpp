#include "ShaderSource.h"

ApplyTransform::ApplyTransform(string file_path, int decomposition_level, int img_dimension_width) :	ShaderSource(file_path),
																										decomp_lvl{decomposition_level},
																										img_dimension_x{img_dimension_width} {}

ApplyTransform::ApplyTransform(string file_path, int decomposition_level, int img_dimension_width, int H_L) :	ShaderSource(file_path),
																												decomp_lvl{ decomposition_level },
																												img_dimension_x{ img_dimension_width },
																												subband{H_L} {}

string ApplyTransform::set_compute_shader_values(string source_file_path) {

	string comp_shader_source = ShaderSource::get_file_content(source_file_path.c_str());

	string num_invoc = "NUM_INV";
	string width_img = "WIDTH_IMG";

	
//	std::cout << "DYNAMIC APPLY TRANSFORM  : \n " << comp_shader_source << " end of file " << '\n';

	int num_invocations = img_dimension_x / pow(2, decomp_lvl);

	int index = 0;

	while (comp_shader_source.find(num_invoc) < comp_shader_source.size()) {

		index = comp_shader_source.find(num_invoc);

		comp_shader_source = comp_shader_source.replace(index, num_invoc.size(), std::to_string(num_invocations));

	}

	index = 0;

	while (comp_shader_source.find(width_img) < comp_shader_source.size()) {


		index = comp_shader_source.find(width_img);
		comp_shader_source = comp_shader_source.replace(index, width_img.size(), std::to_string(num_invocations * 2));

	}

//	std::cout << "APPLY TRANSFORM : \n" << comp_shader_source << '\n';

	return comp_shader_source;

}

string ApplyTransform::set_compute_shader_values_inverse(string source_file_path, int H_L) {

	string comp_shader_source = ShaderSource::get_file_content(source_file_path.c_str());

	string filter_0 = "FILTER";
	string num_invoc = "NUM_INV";
	string mask = "MASK";


	//	std::cout << "DYNAMIC APPLY TRANSFORM  : \n " << comp_shader_source << " end of file " << '\n';

	int num_invocations = img_dimension_x/pow(2, decomp_lvl);
	int mod_mask = img_dimension_x / pow(2, decomp_lvl - 1) - 1;

	std::cout << "SET INVERSE DWT COMPUTE VALS img_dimension_x = \t" << img_dimension_x << " decomp level : \t" << decomp_lvl << '\n';

	int index = 0;

	string f_0 = "";

	if (subband == 0) {

		f_0 = "g0";
	}

	if (subband == 1) {

		f_0 = "g1";
	}


		while (comp_shader_source.find(filter_0) < comp_shader_source.size()) {

			index = comp_shader_source.find(filter_0);

			comp_shader_source = comp_shader_source.replace(index, filter_0.size(), f_0);

		}

		index = 0;

		while (comp_shader_source.find(num_invoc) < comp_shader_source.size()) {

			index = comp_shader_source.find(num_invoc);

			comp_shader_source = comp_shader_source.replace(index, num_invoc.size(), std::to_string(num_invocations));

		}

		index = 0;

		while (comp_shader_source.find(mask) < comp_shader_source.size()) {

			index = comp_shader_source.find(mask);

			comp_shader_source = comp_shader_source.replace(index, mask.size(), std::to_string(mod_mask));

		}
	

//	std::cout << "INVERSE APPLY TRANSFORM : \n" << comp_shader_source << '\n';

	return comp_shader_source;

}