
#version 430 core
layout(location = 0)in vec2 aPos;
layout(location = 1)in vec2 aTexCoor;

out vec2 tex_coor;

uniform mat4 move;


void main(){
	
	//gl_Position = move  * vec4(aPos.x, aPos.y, 0.0, 1.0);
	gl_Position = vec4(aPos, 1.0, 1.0);
	tex_coor = aTexCoor;

}
