#include "ShaderSource.h"

ApplyTransform::ApplyTransform(string file_path, int decomposition_level, int img_dimension_width) : decomp_lvl{decomposition_level},
img_dimension_x{img_dimension_width} {

}

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

	std::cout << "APPLY TRANSFORM : \n" << comp_shader_source << '\n';

	return comp_shader_source;

}