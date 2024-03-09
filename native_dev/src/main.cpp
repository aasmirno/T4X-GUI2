#pragma once
#include <stdio.h>
#include <iostream>

#include "Renderer.h"

int main(int argc, char* argv[]) {
    std::cout << "Engine start\n";
    
    Renderer r;
    if(!r.initialise()){
        std::cout << "FATAL ERROR: failed to initialise render manager\n";
        return 0;
    }

    r.addRenderObject();
    r.addRenderObject();
    for(int i = 0; i < 500; i++){
        r.render();
    }

    return 0;
}