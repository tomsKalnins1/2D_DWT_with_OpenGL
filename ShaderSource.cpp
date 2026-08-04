#include "ShaderSource.h"

/*
ShaderSource::ShaderSource(string file_path, GLenum shader_type) {

	string shader_source = get_file_content(file_path.c_str());

	const char* shader_source_char = shader_source.c_str();

	ID = glCreateShader(shader_type);

	glShaderSource(ID, 1, &shader_source_char, NULL);
	glCompileShader(ID);

	GLint compiled;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(ID, 1024, NULL, errorLog);
		std::cout << ("RENDER PIPELINE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}


}
*/

/*
ShaderSource::ShaderSource(string file_path, GLenum shader_type, int num_samples, int samples_per_processor) {
	
	string shader_source = get_file_content(file_path.c_str());

	const char* shader_source_char = shader_source.c_str();

	ID = glCreateShader(shader_type);
	glShaderSource(ID, 1, &shader_source_char, NULL);
	glCompileShader(ID);

	GLint compiled;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(ID, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

}
*/

string ShaderSource::get_file_content(string file_path) {
	
	std::ifstream in(file_path.c_str(), std::ios::binary);

	if (in) {
		
		string content;

		in.seekg(0, std::ios::end);
		content.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&content[0], content.size());
		in.close();
		
		return content;
	}

	throw(errno);

}

/*
string ShaderSource::set_compute_shader_values(string shader_source_file, int num_samples, int samples_per_processor) {

	string shader_file = "";
	string invocation_orient = "X_INVOCATIONS";;
	string samples = "NUM_SAMPLES";

	int index = shader_source_file.find(invocation_orient);
	shader_source_file = shader_source_file.replace(index, invocation_orient.size(), std::to_string(num_samples / samples_per_processor));
	
	while (shader_source_file.find(samples) < shader_source_file.size()) {

		index = shader_source_file.find(samples);

		shader_source_file = shader_source_file.replace(index, samples.size(), std::to_string(num_samples));

	}

	return shader_source_file;

}
*/