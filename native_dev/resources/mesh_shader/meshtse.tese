// mesh tesselation evaluation shader
#version 420 core

// primitive gen specification:
//  quad patches
//  fractional odd spacing
//  ccw winding
layout (quads, fractional_odd_spacing, ccw) in;

uniform mat4 model;      // variable model matrix
uniform mat4 view;       // variable view matrix
uniform mat4 projection; // variable projection matrix

out float Height; // send to Fragment Shader for coloring

void main()
{
    // get patch coordinate: generated in primitive generation stage
    float u = gl_TessCoord.x;   // fractional x coord
    float v = gl_TessCoord.y;   // fractional y coord

    // ----------------------------------------------------------------------
    // retrieve initial patch vertex positions
    vec4 p00 = gl_in[0].gl_Position;    // bottom left
    vec4 p01 = gl_in[1].gl_Position;    // top left
    vec4 p10 = gl_in[2].gl_Position;    // bottom right
    vec4 p11 = gl_in[3].gl_Position;    // top right

    // compute initial patch face normal vector
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // bilinearly interpolate position coordinate across patch
    // generate position coordinate based on relative tesselated point coordinate
    vec4 p0 = (p01 - p00) * u + p00;    
    vec4 p1 = (p11 - p10) * u + p10; 
    vec4 p = (p1 - p0) * v + p0;

    float Height = p00.x;
    // displace point along normal
    p += normal * Height;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = projection * view * model * p;
}