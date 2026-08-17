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

ShaderProgram ShaderProgram::undo_gamma_correction_prog(int img_width) {

	ShaderProgram no_gamma;

	string file_path = "undo_gamma_correction_DYNAMIC.glsl";
	
	string shader_source = ShaderSource::get_file_content(file_path);
	int index = 0;

	string img_w = "IMG_WIDTH";
	index = shader_source.find(img_w);
	shader_source = shader_source.replace(index, img_w.size(), std::to_string(img_width));

	const char* shader_source_char = shader_source.c_str();

	unsigned int prog_id;

	prog_id = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(prog_id, 1, &shader_source_char, NULL);
	glCompileShader(prog_id);

	GLint compiled;
	glGetShaderiv(prog_id, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char errorLog[1024];
		glGetShaderInfoLog(prog_id, 1024, NULL, errorLog);
		std::cout << ("COMPUTE SHADER " + file_path + " \n COMPILATION FAILED : \n") << errorLog << '\n';
	}

	no_gamma.ID = glCreateProgram();

	glAttachShader(no_gamma.ID, prog_id);
	glLinkProgram(no_gamma.ID);
	GLint linkSuccess = 0;
	glGetProgramiv(no_gamma.ID, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE) {

		char infoLog[1024];
		glGetProgramInfoLog(no_gamma.ID, 1024, NULL, infoLog);
		std::cout << "PROGRAM LINK FAILED, PROBLEM WITH " << file_path << " :\n" << infoLog << std::endl;

	}

	glDeleteShader(prog_id);

	return no_gamma;
}

void ShaderProgram::undo_gamma_correction(Texture& input, int img_width) {
	
	ShaderProgram undo_gamma = undo_gamma_correction_prog(img_width);

	Texture::activate_tex_unit(0);
	input.bind_texture();
	input.bind_image_2D(0);

	undo_gamma.use_shader_prog();
	glDispatchCompute((unsigned int)ceil(1), (unsigned int)ceil(img_width), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	input.unbind_image_texture(0);
	input.unbind_texture(0);


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