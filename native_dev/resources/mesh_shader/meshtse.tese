// mesh tesselation evaluation shader
#version 420 core

// primitive gen specification:
//  quad patches
//  fractional odd spacing
//  ccw winding
layout (quads, fractional_odd_spacing, ccw) in;

uniform mat4 view;       // variable view matrix
uniform mat4 projection; // variable projection matrix
in vec4 PatchNormal[];

out vec2 TexCoord;
out vec4 Normal;
out vec3 FragPos;
out vec3 LightPos;

void main()
{
    Normal = PatchNormal[0];
    // get patch coordinate: generated in primitive generation stage
    float u = gl_TessCoord.x;   // fractional x coord
    float v = gl_TessCoord.y;   // fractional y coord

    // ----------------------------------------------------------------------
    // retrieve initial patch vertex positions
    vec4 p00 = gl_in[0].gl_Position;    // bottom left
    vec4 p01 = gl_in[1].gl_Position;    // top left
    vec4 p10 = gl_in[2].gl_Position;    // bottom right
    vec4 p11 = gl_in[3].gl_Position;    // top right

    // bilinearly interpolate position coordinate across patch
    // generate position coordinate based on relative tesselated point coordinate
    vec4 p0 = (p01 - p00) * u + p00;    
    vec4 p1 = (p11 - p10) * u + p10; 
    vec4 p = (p1 - p0) * v + p0;

    TexCoord = vec2(u * 0.5, v * 0.5);
    if(p.z < 5){
        TexCoord += vec2(0.0, 0.5);
    } else if(p.z < 7.5){
        TexCoord += vec2(0.5, 0.5);
    } else if(p.z > 9){
        TexCoord += vec2(0.5, 0.0);
    }

    // transform light position
    vec4 lp = projection * view * vec4(1.0, 1.0, 50.0, 1.0);
    LightPos = lp.xyz;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    vec4 pos = projection * view * p;
    FragPos = pos.xyz;
    gl_Position = pos;
}