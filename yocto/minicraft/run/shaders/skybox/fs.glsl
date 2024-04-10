#version 400

in vec2 uv;
in vec3 TexCoord;

uniform sampler2D myTexture;

uniform sampler2D skybox;

uniform sampler2D skybox_front;
uniform sampler2D skybox_back;
uniform sampler2D skybox_left;
uniform sampler2D skybox_right;
uniform sampler2D skybox_top;
uniform sampler2D skybox_bottom;

out vec4 FragColor;
out vec4 color_out;

void main()
{
    FragColor = texture(skybox, uv);
      
    color_out = vec4(FragColor.xyz, 1);
}