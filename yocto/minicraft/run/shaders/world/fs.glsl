#version 400

//Variables en entree
in vec3 normal;
in vec4 color;
in vec2 uv;
in vec4 wPos;
flat in float type;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_BOIS 2.0
#define CUBE_PIERRE 3.0
#define CUBE_EAU 4.0
#define CUBE_BRANCHES 38.0

uniform vec3 sunPos;
uniform float elapsed;
uniform vec3 camPos;
uniform vec3 sunColor;

uniform sampler2D myTexture;

out vec4 FragColor;
out vec4 color_out;

//Globales
const float ambientLevel = 0.4;


float noise(vec4 position)
{
    return 0.5f * sin(position.x / 3 + elapsed) 
    + 0.05f * sin((position.y+position.x) + elapsed) 
    - 1;
}

void main()
{
    vec3 SunPosDur = normalize(sunPos);
	vec3 toLight = normalize(sunPos);
    vec3 SunColorDur = vec3(sunColor);
    vec3 newNorm = normal;

    float specular = 0;

    if(type == CUBE_EAU)
    {
        vec4 A = wPos;
        vec4 B = wPos + vec4(0.2f,0,0,0);
        vec4 C = wPos + vec4(0,0.2f,0,0);

        A.z += noise(A);
        B.z += noise(B);
        C.z += noise(C);

        newNorm = normalize(cross(vec3(B-A),vec3(C-A)));

        vec3 toCam =normalize( camPos - A.xyz);
        vec3 halfVec = normalize(SunPosDur) + toCam;
        halfVec = normalize(halfVec);
        float angle = dot(newNorm,halfVec);
        specular  = pow(abs(angle),100)*50;

        float diffuse = max(0,dot(toLight,newNorm));

        color_out = vec4(0.0,0.0,1.0,1);
        vec3 lightingColor = sqrt(color_out.xyz * diffuse + SunColorDur.xyz * specular * 0.97) + 0.03 * sunColor;
        color_out = vec4(lightingColor, 0.8);
    }

    else
    {
        float diffuse = max(0,dot(toLight,newNorm));

        // Textures
        vec2 newuv = vec2((uv.x + type) / 32.0, uv.y);
        vec4 textureColor = texture(myTexture, newuv);

        // Lighting calculations
        vec3 lightingColor = sqrt(textureColor.xyz * diffuse + SunColorDur.xyz * specular * 0.97) + 0.03 * sunColor/10;

        // Output the final color
        FragColor = vec4(lightingColor, textureColor.a); // Assuming you want to retain alpha from the texture
        color_out = vec4(FragColor.xyz, 1); // Assuming color_out is used for further processing   
    }
}