#version 330

layout( location = 0 ) in vec3 position; 

uniform vec2 dimensions; 

uniform mat3 transform; 

uniform mat4 projection; 

void main() 
{
	vec3 p = position; 

	p.xy *= dimensions; 

	gl_Position = projection * vec4(transform * p, 1);
}