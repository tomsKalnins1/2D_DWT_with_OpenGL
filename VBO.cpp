#include "VBO.h"

using namespace std;

VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
	vert = vertices;
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

}

void VBO::bind_VBO() {

	glBindBuffer(GL_ARRAY_BUFFER, ID);

}

void VBO::unbind_VBO() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void VBO::delete_VBO() {

	glDeleteBuffers(1, &ID);

}
