#version 400

uniform mat4 mvp;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;

layout (location = 4) in vec3 aPos;

out vec3 TexCoord;
out vec2 uv;

void main()
{
	TexCoord = aPos;
	uv = vs_uv_in;
	vec4 vecIn = vec4(vs_position_in,1.0);
	gl_Position = mvp * vecIn;
}