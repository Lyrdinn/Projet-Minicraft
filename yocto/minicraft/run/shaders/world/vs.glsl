#version 400

uniform float elapsed;
uniform mat4 mvp;
uniform mat4 nmat;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;
layout(location=3) in float vs_type_in;

out vec2 TexCoord;

//Variables en sortie
out vec3 normal;
out vec4 color;
out vec2 uv;
out vec4 wPos;
flat out float type;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_PIERRE 3.0
#define CUBE_EAU 4.0
#define CUBE_TRONC 37.0
#define CUBE_BRANCHES 38.0

float noise_water(vec4 position)
{
    return .5f*sin(position.x/3 + elapsed)-1;
}

float noise_tree(vec4 position)
{
    return .2f*sin(position.y/3 + elapsed)-1;
}

void main()
{
    vec4 vecIn = vec4(vs_position_in,1.0);    
        
    normal = (nmat * vec4(vs_normal_in,1.0)).xyz; 

    type = vs_type_in;

    uv = vs_uv_in;

    wPos = m * vecIn; 

    if(vs_type_in == CUBE_EAU) {
        wPos.z += noise_water(wPos);
    }
    if (vs_type_in == CUBE_BRANCHES) {
        wPos.x += noise_tree(wPos);
    }

    gl_Position = p * v * wPos;
}