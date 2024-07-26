#pragma once
#include <vector>
#include <cstdint>
#include <stdio.h>
#include <iostream>

#include "FastNoise/FastNoise.h"

const unsigned MAX_MAP_DIM = 10000;

/*
    Game map class
*/
class Map{
private:
    std::vector<float> height_map;

    unsigned height = 0;
    unsigned width = 0;
    void generate();
public:
    bool initialise(unsigned map_x, unsigned map_y);
    float* getHeightData();
};