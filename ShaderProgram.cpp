#include "ShaderProgram.h"


ShaderProgram::ShaderProgram(string path_to_vert, string path_to_frag) {

	unsigned int vert_ID, frag_ID;

	string shader_source_vertex = ShaderSource::get_file_content(path_to_vert.c_str());
	const char* shader_source_vertex_char = shader_source_vertex.c_str();
	vert_ID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_ID, 1, &shader_source_vertex_char, NULL);
	glCompileShader(vert_ID);

	GLint compiled;
	
	glGetShaderiv(vert_ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(vert_ID, 1024, NULL, errorLog);
		std::cout << ("RENDER PIPELINE SHADER " + path_to_vert + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}


	string shader_source_fragment = ShaderSource::get_file_content(path_to_frag.c_str());

	const char* shader_source_fragment_char = shader_source_fragment.c_str();

	frag_ID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_ID, 1, &shader_source_fragment_char, NULL);
	glCompileShader(frag_ID);

	compiled = 0;

	glGetShaderiv(frag_ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(frag_ID, 1024, NULL, errorLog);
		std::cout << ("RENDER PIPELINE SHADER " + path_to_frag + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}
	


	ID = glCreateProgram();

	glAttachShader(ID, vert_ID);
	glAttachShader(ID, frag_ID);

	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH "<< path_to_vert << "	OR " << path_to_frag << "  :  \n" << infoLog << std::endl;

	}

	glDeleteShader(vert_ID);
	glDeleteShader(frag_ID);

}


ShaderProgram::ShaderProgram(string file_path, int decomposition_level, int img_dimension_width, int size_filter) {

	GenerateTransform comp(file_path, decomposition_level, img_dimension_width, size_filter);
	string source_code = comp.set_compute_shader_values(file_path);

//	std::cout << "SHADER SOURCE : \n" << source_code << '\n';

	const char* shader_source_char = source_code.c_str();


	comp.ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp.ID, 1, &shader_source_char, NULL);
	glCompileShader(comp.ID);

	GLint compiled;
	glGetShaderiv(comp.ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(comp.ID, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, comp.ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << file_path << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(comp.ID);


}

ShaderProgram::ShaderProgram(string file_path, int decomposition_level, int img_dimension_width) {

	ApplyTransform comp(file_path, decomposition_level, img_dimension_width);
	string source_code = comp.set_compute_shader_values(file_path);

	//std::cout << "SHADER SOURCE APPLY TRANSFORM : \n" << source_code << '\n';

	const char* shader_source_char = source_code.c_str();


	comp.ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp.ID, 1, &shader_source_char, NULL);
	glCompileShader(comp.ID);

	GLint compiled;
	glGetShaderiv(comp.ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(comp.ID, 1024, NULL, errorLog);
		std::cout << (" APPLY TRANSFORM COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, comp.ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "APPLY PROGRAM LINK FAILED, PROBLEM WITH " << file_path << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(comp.ID);


}

ShaderProgram::ShaderProgram(string path_to_comp) {

	unsigned int comp_ID;

	string shader_source = ShaderSource::get_file_content(path_to_comp.c_str());
	//std::cout << "REGULAR SHADER PROGRAM CONSTRUCTOR CALL : \n" << shader_source << '\n';
	const char* shader_source_char = shader_source.c_str();



	comp_ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp_ID, 1, &shader_source_char, NULL);
	glCompileShader(comp_ID);

	GLint compiled;
	glGetShaderiv(comp_ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(comp_ID, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + path_to_comp + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, comp_ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << path_to_comp << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(comp_ID);


}

/*
ShaderProgram::ShaderProgram(string file_path, int num_samples, int samples_per_processor) {

	string shader_source = ShaderSource::get_file_content(file_path.c_str());

	const char* shader_source_char = shader_source.c_str();

	ID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(ID, 1, &shader_source_char, NULL);
	glCompileShader(ID);

	GLint compiled;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(ID, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	ID = glCreateProgram();

	glAttachShader(ID, comp.ID);
	glLinkProgram(ID);
	GLint linkSuccess = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << path_to_comp <<" :\n" << infoLog << std::endl;

	}

	glDeleteShader(comp.ID);


}

*/

void ShaderProgram::use_shader_prog() {

	glUseProgram(ID);

}

int ShaderProgram::num_bits(unsigned int number_samples) {

	unsigned int num = 1;
	unsigned int bits = 0;

	while (num < number_samples){

		num <<= 1;
		bits++;

	}

	return bits;

}

void ShaderProgram::delete_shader_prog() {

	glDeleteProgram(ID);

}


void ShaderProgram:: set_uniform(unsigned int shader_id, string uniformName, glm::vec3 vector) {

	int progID;

	glGetIntegerv(GL_CURRENT_PROGRAM, &progID);

	if(shader_id != progID) {
	
		glUseProgram(shader_id);

	}


	unsigned int loc = glGetUniformLocation(shader_id, uniformName.c_str());

	glUniform3fv(loc, 1, &vector[0]);

}

void ShaderProgram::set_uniform(unsigned int shader_id, string uniformName, glm::mat4 matrix) {

	int progID;
	glGetIntegerv(GL_CURRENT_PROGRAM, &progID);

	if (shader_id != progID) {

		glUseProgram(shader_id);

	}

	unsigned int loc = glGetUniformLocation(shader_id, uniformName.c_str());

	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));

}

void ShaderProgram::set_uniform(unsigned int shader_id, string uniformName, float value) {

	int progID;

	glGetIntegerv(GL_CURRENT_PROGRAM, &progID);

	if (shader_id != progID) {

		glUseProgram(shader_id);

	}

	unsigned int loc = glGetUniformLocation(shader_id, uniformName.c_str());

	glUniform1f(loc, value);

}


void ShaderProgram::set_uniform(unsigned int shader_id, string uniformName,unsigned int value) {

	int progID;

	glGetIntegerv(GL_CURRENT_PROGRAM, &progID);

	if (shader_id != progID) {

		glUseProgram(shader_id);

	}

	unsigned int loc = glGetUniformLocation(shader_id, uniformName.c_str());

	glUniform1i(loc, value);

}