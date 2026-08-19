#include "../Header_files/Texture.h"



Texture::Texture(GLenum internal_format, GLenum format, string file_name, int w, int h) :internal_color_format{ internal_format },
color_format{ format }, filename{ file_name }, width{ w }, height{ h } {

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	if (filename != "") {

		stbi_set_flip_vertically_on_load(true);

		image = stbi_loadf((file_name).c_str(), &width, &height, &numColorChannels, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, internal_color_format, width, height, 0, color_format, GL_FLOAT, image);

	}
	else {

		glTexImage2D(GL_TEXTURE_2D, 0, internal_color_format, width, height, 0, color_format, GL_FLOAT, image);
		
	//	std::cout << "EMPTY TEXTURE CREATED \n";

	}

//	std::cout << "NEX TEXTURE ID = " << ID << " POINTER TO NEW TEXTURE = " << this << " POINTER TO IMAGE  = " << image << '\n';

}

Texture::Texture(Texture&& to_move) :
	ID(to_move.ID),
	height{ to_move.height },
	width{ to_move.width },
	numColorChannels{ to_move.numColorChannels },
	color_format{ to_move.color_format },
	internal_color_format{ to_move.internal_color_format },
	image{ to_move.image },
	filename{ to_move.filename } {
	std::cout << "STD :: MOVE \n";

	to_move.ID = 0;
	to_move.height = 0;
	to_move.width = 0;
	to_move.numColorChannels = 0;
	to_move.color_format = 0;
	to_move.internal_color_format = 0;
	to_move.image = nullptr;
	to_move.filename = "";

}

void Texture::reset_to_base(Texture& tex) {

//	std::cout << "OLD TEX ID = " << tex.ID << " POINTER  = " << &tex << '\n';

	tex = Texture(GL_RGBA32F, GL_RGBA, "", tex.width, tex.width);

//	std::cout << " MOVED TO TEX WITH ID = \t" << tex.ID << " TEX POINTER = " << &tex << '\n';
}

Texture& Texture::operator=(Texture&& to_move) {

//	std::cout << "move operator \n reference is = " << &to_move << '\n';

	if (this != &to_move) {
		if (ID != 0) {
			glDeleteTextures(1, &ID);
		}
		ID = to_move.ID;
		height = to_move.height;
		width = to_move.width;
		numColorChannels = to_move.numColorChannels;
		color_format = to_move.color_format;
		internal_color_format = to_move.internal_color_format;
		image = to_move.image;
		filename = to_move.filename;

		to_move.ID = 0;
		to_move.height = 0;
		to_move.width = 0;
		to_move.numColorChannels = 0;
		to_move.color_format = 0;
		to_move.internal_color_format = 0;
		to_move.image = nullptr;
		to_move.filename = "";
	}


	return *this;
}

/*
Texture::Texture(Texture&& to_move) {
	std::cout << "move OVERRIDE \n reference is = " << &to_move << '\n';
	ID = to_move.ID;
	height = to_move.height;
	width = to_move.width;
	numColorChannels = to_move.numColorChannels;
	color_format = to_move.color_format;
	internal_color_format = to_move.internal_color_format;
	image = to_move.image;
	filename = to_move.filename;
	filename = to_move.filename;
	to_move.ID = 0;
	to_move.image = nullptr;
//	to_move.delete_texture();


}
*/
void Texture::bind_image_2D(int layout) {

	glBindImageTexture(layout, ID, 0, GL_FALSE, 0, GL_READ_WRITE, internal_color_format);

}

void Texture::activate_tex_unit(int unit) {

	glActiveTexture(GL_TEXTURE0 + unit);

}

void Texture::bind_texture() {

	glBindTexture(GL_TEXTURE_2D, ID);

}

void Texture::unbind_texture(int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture::unbind_image_texture(int unit) {

	glBindImageTexture(unit, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

}

void Texture::delete_texture() {

	glDeleteTextures(1, &ID);

}