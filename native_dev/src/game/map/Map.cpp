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
	fnFractal->SetOctaveCount(16);
	fnFractal->SetGain(-0.680);
	fnFractal->SetWeightedStrength(2.220);
	fnFractal->SetLacunarity(2.600);

	//add 1
	add->SetRHS(1.0f);
	add->SetLHS(simplex);

	multiply->SetLHS(add);
	multiply->SetRHS(5.0f);
	//multiply->GenUniformGrid2D(height_map.data(), 0, 0, height, width, 0.1f, 1337);
	add->GenUniformGrid2D(height_map.data(), 0, 0, height, width, 0.1f, 1337);
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

std::vector<float> Map::getHeightMap(uint16_t resolution)
{
	// create mesh array
	std::vector<float> mesh_map;
	mesh_map.reserve(width * height * 3);

	// transform map parameters to math friendly values
	float mesh_width = (float)width;
	float mesh_height = (float)height;

	for (int i = 0; i <= resolution - 1; i++)
	{
		for (int j = 0; j <= resolution - 1; j++)
		{
			// bottom left patch values
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * (i / (float)resolution));  // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * (j / (float)resolution)); // y
			int x_coord = (width - 1) * (i / (float)resolution);       // map x
			int y_coord = (height - 1) * (j / (float)resolution);      // map y
			mesh_map.push_back(height_map[x_coord * width + y_coord]); // map elevation at the patch coordinate
			//mesh_map.push_back(0.5f);

			// bottom right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * ((i + 1) / (float)resolution)); // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * (j / (float)resolution));      // y
			x_coord = (width - 1) * ((i + 1) / (float)resolution);      // map x
			y_coord = (height - 1) * (j / (float)resolution);           // map y
			mesh_map.push_back(height_map[x_coord * width + y_coord]);      // map elevation at the patch coordinate
			//mesh_map.push_back(0.5f);

			// top right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * (i / (float)resolution));        // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * ((j + 1) / (float)resolution)); // y
			x_coord = (width - 1) * (i / (float)resolution);             // map x
			y_coord = (height - 1) * ((j + 1) / (float)resolution);      // map y
			mesh_map.push_back(height_map[x_coord * width + y_coord]);       // map elevation at the patch coordinate
			//mesh_map.push_back(0.5f);

			// top right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * ((i + 1) / (float)resolution));  // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * ((j + 1) / (float)resolution)); // y
			x_coord = (width - 1) * ((i + 1) / (float)resolution);       // map x
			y_coord = (height - 1) * ((j + 1) / (float)resolution);      // map y
			mesh_map.push_back(height_map[x_coord * width + y_coord]);       // map elevation at the patch coordinate
			//mesh_map.push_back(0.5f);
		}
	}

	return mesh_map;
}
