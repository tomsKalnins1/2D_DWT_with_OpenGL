#include "ShaderSource.h"

GenerateTransform::GenerateTransform(string file_path, int decomposition_level, int img_dimension_width, int size_filter) : decomp_lvl{ decomposition_level },
img_dimension_x{img_dimension_width},
size_of_filter{size_filter} {



}

string GenerateTransform::set_compute_shader_values(string source_file_path) {

	string comp_shader_source = ShaderSource::get_file_content(source_file_path.c_str());
	
	string num_invoc = "NUM_INV";
	string width_img = "WIDTH_IMG";
	string s_of_filter = "SIZE_OF_FILTER";
	string lp_filter_val = "LOW_PASS_FILTER_VALUES";
	string hp_filter_val = "HIGH_PASS_FILTER_VALUES";




	int num_invocations = img_dimension_x / pow(2, decomp_lvl);

	//for now the strings of filters here, later use constexpr to generate the values at compile time ?
	string low_pass = "0.4829629131, 0.8365163037, 0.224143868, -0.1294095226 ";
	string high_pass = "0.1294095226, 0.224143868, -0.8365163037, 0.4829629131 ";

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

	index = 0;

	while (comp_shader_source.find(s_of_filter) < comp_shader_source.size()) {

		index = comp_shader_source.find(s_of_filter);
		comp_shader_source = comp_shader_source.replace(index, s_of_filter.size(), std::to_string(size_of_filter));

	}

	index = 0;

	index = comp_shader_source.find(lp_filter_val);
	comp_shader_source = comp_shader_source.replace(index, lp_filter_val.size(), low_pass);

	index = 0;

	index = comp_shader_source.find(hp_filter_val);
	comp_shader_source = comp_shader_source.replace(index, hp_filter_val.size(), high_pass);

	std::cout << "GENERATE TRANSFORM DYNAMIC : \n" << comp_shader_source << '\n';

	return comp_shader_source;

}