#include "render/RenderObjects/RenderObject.h"

bool RenderObject::initialise(int obj_id){
    object_id = obj_id;
    return genBuffers();
}