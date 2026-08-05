#include "ShaderSource.h"

TransposeRegion::TransposeRegion(string path, int decomposition_lvl, int img_dimension_x) : ShaderSource(path),
																							decomp_lvl{ decomposition_lvl },
																							img_dimension_x{ img_dimension_x } {}

string TransposeRegion::set_compute_shader_values(string source_file_path) {
	
	string comp_shader_source = ShaderSource::get_file_content(source_file_path.c_str());

	string width_img = "WIDTH_IMG";

	int width = img_dimension_x / decomp_lvl;

	int index = 0;

	while (comp_shader_source.find(width_img) < comp_shader_source.size()) {

		index = comp_shader_source.find(width_img);

		comp_shader_source = comp_shader_source.replace(index, width_img.size(), std::to_string(width));

	}

	return comp_shader_source;

}