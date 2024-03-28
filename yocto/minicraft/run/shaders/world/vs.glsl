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

float noise(vec4 position)
{
    return .5f*sin(position.x/3 + elapsed)-1;
}

void main()
{
    vec4 vecIn = vec4(vs_position_in,1.0);    
        
    normal = (nmat * vec4(vs_normal_in,1.0)).xyz; 

    type = vs_type_in;

    uv = vs_uv_in;

    //Couleur par défaut violet
    color = vec4(1.0,1.0,0.0,1.0);

    wPos = m * vecIn; 

    //Couleur fonction du type
    if(vs_type_in == CUBE_HERBE)
        color = vec4(0,1,0,1);
    if(vs_type_in == CUBE_TERRE)
        color = vec4(0.2,0.1,0,1);
    if (vs_type_in == CUBE_PIERRE){
        color = vec4(0.03,0.03,0.03,1);
    }
    if(vs_type_in == CUBE_EAU){
        color = vec4(0.0,0.0,1.0,0.7);
        wPos.z += noise(wPos);
    }
    if (vs_type_in == CUBE_TRONC) {
        color = vec4(168/255, 157/255, 122/255, 1);
    }
    if (vs_type_in == CUBE_BRANCHES) {
        color = vec4(0.0, 102/255, 0.0, 1);   
    }

    gl_Position = p * v * wPos;
                
}