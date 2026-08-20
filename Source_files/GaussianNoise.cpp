#include "../Header_files/ShaderSource.h"

#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include <random>


GaussianNoise::GaussianNoise(string file_path, float omega) : ShaderSource(file_path),
                                                                    deviation{omega} {}

string GaussianNoise::set_compute_shader_values(string source_file_path){
string comp_shader_source = ShaderSource::get_file_content(source_file_path.c_str());

string st_deviation = "STANDARD_DEVIATION";

int index = comp_shader_source.find(st_deviation);

comp_shader_source = comp_shader_source.replace(index, st_deviation.size(), std::to_string(deviation));


//std::cout << "GAUSSIAN NOISE SHADER : \n" << comp_shader_source << '\n';

return comp_shader_source;

}

