#include "ShaderSource.h"

UpsampleSubband::UpsampleSubband(string file_path, int decomposition_level, int img_dimension_width, int hl_0, int hl_1) : ShaderSource(file_path),
decomp_lvl{ decomposition_level },
img_dimension_x{ img_dimension_width },
H_L_0{ hl_0 },
H_L_1{ hl_1 } {}

string UpsampleSubband::set_compute_shader_values(string source_file_path) {

	string comp_shader_source = ShaderSource::get_file_content(path_to_shader.c_str());

	string num_invoc = "NUM_INV";

	int num_invocations = img_dimension_x / pow(2, decomp_lvl);

	int index = 0;

	while (comp_shader_source.find(num_invoc) < comp_shader_source.size()) {

		index = comp_shader_source.find(num_invoc);

		comp_shader_source = comp_shader_source.replace(index, num_invoc.size(), std::to_string(num_invocations));

	}
	std::cout << "UPSAMPLE DYNAMIC : \n" << comp_shader_source << '\n';

	return comp_shader_source;

}
