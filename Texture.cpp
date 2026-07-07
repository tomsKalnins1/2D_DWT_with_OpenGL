#include "Texture.h"

Texture::Texture(GLenum internal_format, GLenum format, string file_name, int w, int h) :internal_color_format{ internal_format },
color_format{ format }, filename{ file_name }, width{ w }, height{ h } {

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	if (filename != "no_file") {

		stbi_set_flip_vertically_on_load(true);

		image = stbi_loadf((file_name).c_str(), &width, &height, &numColorChannels, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, internal_color_format, width, height, 0, color_format, GL_FLOAT, image);

	}
	else {

		glTexImage2D(GL_TEXTURE_2D, 0, internal_color_format, width, height, 0, color_format, GL_FLOAT, NULL);

	}

}

void Texture::reset_to_base(Texture& tex) {

	glDeleteTextures(1, &tex.ID);
	/*
	*	SET ALL THE VALUES TO 0 OF PREVIOUS TEXTURE ONLY THEN MOVE A NEW INSTANCE IN ITS PLACE !!!
	tex.ID = ;
	height = ;
	width = to_move.width;
	numColorChannels = to_move.numColorChannels;
	color_format = to_move.color_format;
	internal_color_format = to_move.internal_color_format;
	image = to_move.image;
	to_move.image = nullptr;
	filename = to_move.filename;
	filename = to_move.filename;
	*/
	tex = Texture{};
	std::cout << "where the tex is moved  = " << &tex << '\n';
}

Texture& Texture::operator=(Texture&& to_move) {
	std::cout << "move operator \n reference is = " <<  &to_move << '\n';

	ID = to_move.ID;
	height = to_move.height;
	width = to_move.width;
	numColorChannels = to_move.numColorChannels;
	color_format = to_move.color_format;
	internal_color_format = to_move.internal_color_format;
	image = to_move.image;
	to_move.image = nullptr;
	filename = to_move.filename;
	filename = to_move.filename;

	return *this;

}

Texture::Texture(Texture&& to_move) {

	ID = to_move.ID;
	height = to_move.height;
	width = to_move.width;
	numColorChannels = to_move.numColorChannels;
	color_format = to_move.color_format;
	internal_color_format = to_move.internal_color_format;
	image = to_move.image;
	filename = to_move.filename;
	filename = to_move.filename;
	to_move.delete_texture();


}

void Texture::bind_image_2D(int layout) {

	glBindImageTexture(layout, ID, 0, GL_FALSE, 0, GL_READ_WRITE, internal_color_format);

}

void Texture::activate_tex_unit(int unit) {

	glActiveTexture(GL_TEXTURE0 + unit);

}

void Texture::bind_texture() {

	glBindTexture(GL_TEXTURE_2D, ID);

}

void Texture::unbind_texture() {

	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture::delete_texture() {

	glDeleteTextures(1, &ID);

}