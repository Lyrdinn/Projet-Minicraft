#version 400

in vec2 uv;
in vec3 TexCoord;

uniform sampler2D myTexture;

uniform sampler2D skybox;
uniform vec3 sunColor;

out vec4 FragColor;
out vec4 color_out;

void main()
{
    vec4 textureColor = texture(skybox, uv);
    vec3 lightingColor = textureColor.xyz * (sunColor);

    FragColor = textureColor;
    FragColor.w = 0.5;
    color_out = vec4(FragColor.xyz, 1.0);
}