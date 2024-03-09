#pragma once
#include <stdio.h>
#include <iostream>

#include "Renderer.h"

int main(int argc, char* argv[]) {
    std::cout << "Engine start\n";
    
    Renderer r;
    r.initialise();

    r.addRenderObject();
    r.addRenderObject();
    for(int i = 0; i < 500; i++){
        r.render();
    }

    return 0;
}