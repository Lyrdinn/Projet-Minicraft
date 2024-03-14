#version 400

//Variables en entree
in vec3 normal;
in vec4 color;
in vec2 uv;
in vec4 wPos;
flat in float type;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_EAU 4.0

uniform vec3 sunPos;
uniform float elapsed;

out vec4 color_out;

//Globales
const float ambientLevel = 0.4;

float noise(vec4 position)
{
    return 0.5f * sin(position.x / 3 + elapsed) 
    + 0.25f * sin((position.y+position.x) + elapsed) 
    - 1;
}

void main()
{
    vec3 toLight = normalize(sunPos);

    vec3 newNorm = normal;

    if(type == CUBE_EAU)
    {
        vec4 A = wPos;
        vec4 B = wPos + vec4(0.2f,0,0,0);
        vec4 C = wPos + vec4(0,0.2f,0,0);

        A.z += noise(A);
        B.z += noise(B);
        C.z += noise(C);

        newNorm = normalize(cross(vec3(B-A),vec3(C-A)));
    }    
    
    color_out = vec4(sqrt(color.xyz * max(0,dot(toLight,newNorm)) * 0.97 + 0.03 * vec3(0.8,0.9,1)),color.a);

}