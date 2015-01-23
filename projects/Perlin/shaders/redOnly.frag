#version 330

in vec2 TexCoord; 

layout( location = 0 ) out vec4 fragColour; 

uniform sampler2D diffuseMap; 

void main()
{
	fragColour.rgb = texture( diffuseMap, TexCoord ).rrr; 
	fragColour.a = 1; 
}