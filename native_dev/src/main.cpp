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
    r.addTileObject();
    for(int i = 0; i < 2000; i++){
        r.render();
    }

    return 0;
}