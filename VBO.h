#ifndef VBO_H
#define VBO_H


#include <glad/glad.h>
#include <iostream>

class VBO {

public:
	unsigned int ID;
	GLfloat* vert;
	VBO(GLfloat* vertices, GLsizeiptr size);

	void bind_VBO();

	void unbind_VBO();

	void delete_VBO();




};

#endif