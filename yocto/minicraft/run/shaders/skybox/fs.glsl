#version 330 core

in vec2 uv;
in vec3 TexCoord;

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
    vec3 absTexCoord = abs(TexCoord);
    vec3 maxVal = max(absTexCoord, max(absTexCoord.yzx, absTexCoord.zxy));

    if (maxVal.x == absTexCoord.x) {
        FragColor = texture(skybox_right, uv);
    } 
    else if (maxVal.y == absTexCoord.y) {
        FragColor = texture(skybox_top, uv);
    } 
    else if (maxVal.z == absTexCoord.z) {
        FragColor = texture(skybox_front, uv);
    } 
    else if (-maxVal.x == absTexCoord.x) {
        FragColor = texture(skybox_left, uv);
    } 
    else if (-maxVal.y == absTexCoord.y) {
        FragColor = texture(skybox_bottom, uv);
    }
    else {
        FragColor = texture(skybox_back, uv);
    }

    color_out = vec4(FragColor.xyz, 1);
}