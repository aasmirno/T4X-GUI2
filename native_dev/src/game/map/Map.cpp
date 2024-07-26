#include "T4X/game/map/Map.h"

void Map::generate()
{
	// generate random noise
	auto fnPerlin = FastNoise::New<FastNoise::Perlin>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();


	auto add = FastNoise::New<FastNoise::Add>();
	auto multiply = FastNoise::New<FastNoise::Multiply>();

	auto simplex = FastNoise::New<FastNoise::Simplex>();

	fnFractal->SetSource(fnPerlin);
	fnFractal->SetOctaveCount(2);
	fnFractal->SetGain(3.5);
	fnFractal->SetWeightedStrength(1.2);
	fnFractal->SetLacunarity(0.120);

	//add 1
	add->SetRHS(1.0f);
	add->SetLHS(fnFractal);

	multiply->SetLHS(add);
	multiply->SetRHS(5.0f);
	//multiply->GenUniformGrid2D(height_map.data(), 0, 0, height, width, 0.1f, 1337);
	multiply->GenUniformGrid2D(height_map.data(), 0, 0, height, width, 0.1f, 1337);
}

bool Map::initialise(unsigned map_x, unsigned map_y)
{
	if (map_x == 0 || map_y == 0)
	{
		printf("[ MAP ERROR ] improper input dimensions: x=%d, y=%d", map_x, map_y);
		return false;
	}

	// check and set dimensions
	if (map_x > MAX_MAP_DIM)
	{
		map_x = MAX_MAP_DIM;
	}
	if (map_y > MAX_MAP_DIM)
	{
		map_y = MAX_MAP_DIM;
	}
	height = map_x;
	width = map_y;

	// resize vector
	height_map.resize(height * width, 0.0f);
	generate();
	return true;
}

float* Map::getHeightData()
{
	return &height_map[0];
}
