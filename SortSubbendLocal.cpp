#include "ShaderSource.h"

SortSubbandLocal::SortSubbandLocal(string file_path, int decomposition_level, int img_dimension_width, int num_part) : decomp_lvl{ decomposition_level },
img_dimension_x{ img_dimension_width },
num_partitions{num_part} {

}

string SortSubbandLocal::set_compute_shader_values(string source_file_path) {

	string comp_shader_source = ShaderSource::get_file_content(source_file_path.c_str());

	string num_invoc = "NUM_INV";
	string subband_width = "SUBBAND_WIDTH";
	string num_part = "NUM_PARTITIONS";



	int subband_w = img_dimension_x / pow(2, decomp_lvl);
	int num_invocations = subband_w / 2;

	int index = 0;

	while (comp_shader_source.find(num_invoc) < comp_shader_source.size()) {

		index = comp_shader_source.find(num_invoc);

		comp_shader_source = comp_shader_source.replace(index, num_invoc.size(), std::to_string(num_invocations));

	}

	index = 0;

	while (comp_shader_source.find(subband_width) < comp_shader_source.size()) {

		index = comp_shader_source.find(subband_width);
		comp_shader_source = comp_shader_source.replace(index, subband_width.size(), std::to_string(subband_w));

	}

	index = comp_shader_source.find(num_part);
	comp_shader_source = comp_shader_source.replace(index, num_part.size(), std::to_string(num_partitions));

	

	std::cout << "SORT SUBBAND : \n" << comp_shader_source << '\n';

	return comp_shader_source;

}