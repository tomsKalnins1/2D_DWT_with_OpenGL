
#version 330 core

out vec4 FragColor;

in vec2 tex_coor; //the coordinates of vertices based on which the tecture is applied

uniform sampler2D filterTexture;
uniform sampler2D last;

void main(){

FragColor = texture(filterTexture, tex_coor);

}
