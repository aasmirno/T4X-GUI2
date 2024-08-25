// mesh tesselation control shader
#version 420 core

// vertices per patch, same as defined in call
layout (vertices=4) out;

// varying input from vertex shader
in vec2 TexCoord[];
// varying output to evaluation shader
out vec2 TextureCoord[];

void main()
{
    // ----------------------------------------------------------------------
    // send initial patch vertices to eval stage
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch (tess levels global within patch)
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 8;
        gl_TessLevelOuter[1] = 8;
        gl_TessLevelOuter[2] = 8;
        gl_TessLevelOuter[3] = 8;

        gl_TessLevelInner[0] = 4;
        gl_TessLevelInner[1] = 4;
    }
}