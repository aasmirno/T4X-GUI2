// mesh tesselation control shader
#version 420 core

// vertices per patch, same as defined in call
layout (vertices=4) out;

out vec4 PatchNormal[];

void main()
{
    // ----------------------------------------------------------------------
    // send initial patch vertices to eval stage
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    //TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];


    // retrieve initial patch vertex positions
    vec4 p00 = gl_in[0].gl_Position;    // bottom left
    vec4 p01 = gl_in[1].gl_Position;    // top left
    vec4 p10 = gl_in[2].gl_Position;    // bottom right

    // calculate normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec3 normal = normalize(cross(vVec.xyz, uVec.xyz));
    PatchNormal[gl_InvocationID] = vec4(normal, 1.0);

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch (tess levels global within patch)
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 4;
        gl_TessLevelOuter[1] = 4;
        gl_TessLevelOuter[2] = 4;
        gl_TessLevelOuter[3] = 4;

        gl_TessLevelInner[0] = 4;
        gl_TessLevelInner[1] = 4;
    }
}