#pragma once
#include <vector>
#include <cstdint>
#include <stdio.h>

#include "FastNoise/FastNoise.h"

const unsigned MAX_MAP_DIM = 10000;

/*
    Game map class
*/
class Map{
private:
    std::vector<uint16_t> tile_data;
    std::vector<float> elevation;

    unsigned height = 0;
    unsigned width = 0;

    

public:
    bool initialise(unsigned map_x, unsigned map_y);
    uint16_t* getData();
};