#ifndef VAO_H
#define VAO_H

#include "VBO.h"
#include <iostream>


	class VAO {

	public:
		unsigned int ID;
		VAO();

		void link_VBO(VBO vbo, int numCoords, int numTexCoords);

		void bind_VAO();

		void unbind_VAO();

		void delete_VAO();





	};



#endif