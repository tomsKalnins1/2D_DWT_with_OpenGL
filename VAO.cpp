#include "VAO.h"

using namespace std;



VAO::VAO() {

	glGenVertexArrays(1, &ID);

}

void VAO::link_VBO(VBO vbo, int numCoords, int numTexCoords) {

	vbo.bind_VBO();
	glBindVertexArray(ID);

	if (numCoords == 2) {
	
		if (numTexCoords == 2) {

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		}
	}

	if (numCoords == 3) {
	
		if (numTexCoords == 2) {

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
		
		}

		if (numCoords == 0) {
		
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		
		}


	
	}
}

void VAO::bind_VAO() {

	glBindVertexArray(ID);

	

}

void VAO::unbind_VAO() {

	glBindVertexArray(0);

}

void VAO::delete_VAO() {

	glDeleteVertexArrays(1, &ID);

}

