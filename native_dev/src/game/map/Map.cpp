#include "game/map/Map.h"

bool Map::initialise(unsigned map_x, unsigned map_y){
    //check and set dimensions
    if(map_x > MAX_MAP_DIM){
        map_x = MAX_MAP_DIM;
    }
    if(map_y > MAX_MAP_DIM){
        map_y = MAX_MAP_DIM;
    }
    height = map_x;
    width = map_y;

    //fill tile data vector
    tile_data.resize(height * width);
    elevation.resize(height * width);


    //generate random noise
    auto fnPerlin = FastNoise::New<FastNoise::Perlin>();
    auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

    fnFractal->SetSource(fnPerlin);
    fnFractal->SetOctaveCount(16);
    fnFractal->SetGain(-0.680);
    fnFractal->SetWeightedStrength(2.220);
    fnFractal->SetLacunarity(2.600);

    fnFractal->GenUniformGrid2D(elevation.data(), 0, 0, height, width, 0.1f, 1337);
    return true;
}

uint16_t* Map::getData(){
    for(int i = 0; i < height * width; i++){
        if (elevation[i] < 0.125){
            tile_data[i] = 0;
        } else if(elevation[i] < 0.25){
            tile_data[i] = 1;
        } else if(elevation[i] < 0.375){
            tile_data[i] = 2;
        } else if(elevation[i] < 0.5){
            tile_data[i] = 3;
        } else if(elevation[i] < 0.625){
            tile_data[i] = 4;
        }  else if(elevation[i] < 0.75){
            tile_data[i] = 5;
        } else {
            tile_data[i] = 6;
        }
    }

    return &tile_data[0];
}