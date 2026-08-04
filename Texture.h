#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>
#include <string>

using std::string;

class Texture{

public:
	unsigned int ID = 0;
	int height;
	int width;
	int numColorChannels;
	GLenum color_format;
	GLenum internal_color_format;
	float* image;
	string filename;

	static void reset_to_base(Texture&);
	Texture& operator=(Texture&&);
	Texture(Texture&&);
	Texture operator=(const Texture&) = delete;
	Texture(const Texture&) = delete;

	~Texture(){
		std::cout << "DELETE CALLED ! \n";
		if (ID != 0)
			glDeleteTextures(1, &ID);


	}

	Texture(GLenum internal_format = GL_RGBA32F, GLenum format = GL_RGBA, string file_name = "", int width = 256, int height = 256);
	
	void bind_texture();

	void bind_image_2D(int layout);

	void unbind_texture();

	void delete_texture();

	static void activate_tex_unit(int i);

};

#endif