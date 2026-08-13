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
	float* image = nullptr;
	string filename;

	static void reset_to_base(Texture&);
	Texture& operator=(Texture&&);
	Texture(Texture&&);
	Texture operator=(const Texture&) = delete;
	Texture(const Texture&) = delete;

	~Texture(){
	//	std::cout << "DESTRUCTOR ON TEX ID : " << ID << '\n';
		if (ID != 0) {
			glDeleteTextures(1, &ID);
		}
		if (image) {
			stbi_image_free(image);
		}
	}

	Texture(GLenum internal_format = GL_RGBA32F, GLenum format = GL_RGBA, string file_name = "", int width = 256, int height = 256);
	
	void bind_texture();

	void bind_image_2D(int layout);

	void unbind_texture(int unit);

	void delete_texture();

	static void activate_tex_unit(int i);

	void unbind_image_texture(int unit);

};

#endif