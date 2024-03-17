#version 400

uniform vec3 sunColor;

out vec4 color_out;

void main()
{
	color_out = vec4(sqrt(sunColor),1);
}