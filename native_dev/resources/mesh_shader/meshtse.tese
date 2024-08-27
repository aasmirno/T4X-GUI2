// mesh tesselation evaluation shader
#version 420 core

// primitive gen specification:
//  quad patches
//  fractional odd spacing
//  ccw winding
layout (quads, fractional_odd_spacing, ccw) in;

uniform sampler2D data_texture;     // height data texture
uniform mat4 view;                  // variable view matrix
uniform mat4 projection;            // variable projection matrix

in vec2 TextureCoord[];

out vec2 color_coord;
out vec4 patch_normal;
out vec4 FragPos;

void main()
{
    // get patch coordinate: generated in primitive generation stage
    float u = gl_TessCoord.x;   // fractional x coord
    float v = gl_TessCoord.y;   // fractional y coord

    // ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // lookup texel at patch coordinate for height and scale + shift as desired
    float height = texture(data_texture, texCoord).r;

    // ----------------------------------------------------------------------
    // retrieve initial patch vertex positions
    vec4 p00 = gl_in[0].gl_Position;    // bottom left
    vec4 p01 = gl_in[1].gl_Position;    // top left
    vec4 p10 = gl_in[2].gl_Position;    // bottom right
    vec4 p11 = gl_in[3].gl_Position;    // top right

    // compute patch surface normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );
    patch_normal = normal;

    // bilinearly interpolate position coordinate across patch
    // generate position coordinate based on relative tesselated point coordinate
    vec4 p0 = (p01 - p00) * u + p00;    
    vec4 p1 = (p11 - p10) * u + p10; 
    vec4 p = (p1 - p0) * v + p0;

    // displace point along normal
    p += normal * (height * 10.0);

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    color_coord = texCoord;
    FragPos = p;
    gl_Position = projection * view * p;
}