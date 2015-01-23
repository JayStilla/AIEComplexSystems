#version 330

layout( location = 0 ) in vec4 position; 
layout( location = 1 ) in vec4 colour;
layout( location = 2 ) in vec2 texCoord; 

out vec2 TexCoord; 

uniform mat4 projectionViewModel; 

void main()
{
	TexCoord = texCoord; 
	gl_Position = position; 
}