#include "render/RenderObjects/RenderObject.h"

bool RenderObject::initialise(int obj_id, GLuint shader_program){
    if(shader_program == 0){
        printf("failed to initialise render object, bad shader program handle:%d\n", shader_program);
        return false;
    }

    object_id = obj_id;
    shader_pid = shader_program;

    return genBuffers();
}