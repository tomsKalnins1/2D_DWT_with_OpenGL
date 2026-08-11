#include "ShaderSource.h"



SoftThreshold::SoftThreshold(string file_path, int decomposition_level, int img_dimension_width, int num_part) :	ShaderSource(file_path),
																													decomp_lvl{ decomposition_level },
																													img_dimension_x{ img_dimension_width },
																													num_partitions{ num_part } {}

string SoftThreshold::set_compute_shader_values(string source_file_path) {

	string comp_shader_source = ShaderSource::get_file_content(source_file_path.c_str());

	string num_inv = "NUM_INV";
	string size_partition = "PARTITION_SIZE";
	string subband_size = "SUBBAND_SIZE";
	string scale_p = "SCALE_PARAM";

	int size_sub = img_dimension_x / pow(2, decomp_lvl);
	int num_invoc = num_partitions;
	int size_of_part = size_sub / num_invoc;

	int denom = log(img_dimension_x);
	int num = img_dimension_x * img_dimension_x;

	float arg = (float)(num / denom);

	float scale_parameter = std::sqrt(std::log2(arg));

	int index = 0;

	index = comp_shader_source.find(scale_p);

	comp_shader_source = comp_shader_source.replace(index, scale_p.size(), std::to_string(scale_parameter));

	index = 0;

	while (comp_shader_source.find(num_inv) < comp_shader_source.size()) {

		index = comp_shader_source.find(num_inv);

		comp_shader_source = comp_shader_source.replace(index, num_inv.size(), std::to_string(num_invoc));

	}

	index = 0;

	while (comp_shader_source.find(size_partition) < comp_shader_source.size()) {

		index = comp_shader_source.find(size_partition);

		comp_shader_source = comp_shader_source.replace(index, size_partition.size(), std::to_string(size_of_part));

	}

	index = 0;

	while (comp_shader_source.find(subband_size) < comp_shader_source.size()) {

		index = comp_shader_source.find(subband_size);

		comp_shader_source = comp_shader_source.replace(index, subband_size.size(), std::to_string(size_sub));

	}
	std::cout << "SOFT THRESHOLD SHADER : \n" << comp_shader_source << '\n';

	return comp_shader_source;

}