#pragma once
#include <vector>
#include <random>
#include <cmath>
#include "FastNoiseLite.h"

class HeightMap
{
private:
	enum ElevationType : uint8_t { L1, L2, L3, L4, L5, L6, L7, L8 };

	std::vector<uint8_t> height_map_ids;
	std::vector<float> height_map;
	float max_height = 0.0f;
	int width = 0;
	int height = 0;

	std::vector<std::vector<int>> erosionBrushIndices;
	std::vector<std::vector<float>> erosionBrushWeights;
	int erosionRadius = 1;
	float inertia = .05f; // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction. 
	float sedimentCapacityFactor = 3; // Multiplier for how much sediment a droplet can carry
	float minSedimentCapacity = .01f; // Used to prevent carry capacity getting too close to zero on flatter terrain

	float erodeSpeed = .5f;

	float depositSpeed = .3f;

	float evaporateSpeed = .01f;
	float gravity = 4;
	int maxDropletLifetime = 30;

	float initialWaterVolume = 1;
	float initialSpeed = 1;

public:
	float getMaxHeight() { return max_height; }

	//get height at (x,y)
	float getHeight(int x, int y) {
		if (coordCheck(x, y)) {
			return height_map[y * width + x];
		}
		return 0.0f;
	}

	//get raw height map vector
	std::vector<float>& getHeightMap() {
		return height_map;
	}

	//assign ids and return id array
	uint8_t* getIDArray() {
		//assign ids
		for (size_t index = 0; index < height_map.size(); index++) {
			//if max height is 0 (no elevation), assign 0.0f otherwise normalise
			float normalised_height = height_map[index];

			if (normalised_height < 1.0f / 8.0f) {
				height_map_ids[index] = L1;
			}
			else if (normalised_height < 2.0f / 8.0f) {
				height_map_ids[index] = L2;
			}
			else if (normalised_height < 3.0f / 8.0f) {
				height_map_ids[index] = L3;
			}
			else if (normalised_height < 4.0f / 8.0f) {
				height_map_ids[index] = L4;
			}
			else if (normalised_height < 5.0f / 8.0f) {
				height_map_ids[index] = L5;
			}
			else if (normalised_height < 6.0f / 8.0f) {
				height_map_ids[index] = L6;
			}
			else if (normalised_height < 7.0f / 8.0f) {
				height_map_ids[index] = L7;
			}
			else{
				height_map_ids[index] = L8;
			}
		}

		return &height_map_ids[0];
	}

	//check coordinate against bounds
	bool coordCheck(int x, int y) {
		if (x < 0 || x >= width) {
			return false;
		}
		if (y < 0 || y >= height) {
			return false;
		}
		return true;
	}

	//initialise height map to 0.0f
	void init(int size_x, int size_y) {
		width = size_x;
		height = size_y;
		refresh();
	}

	//refresh the height map to 0.0f
	void refresh() {
		max_height = 0;
		height_map.clear();
		height_map.resize(height * width);

		height_map_ids.clear();
		height_map_ids.resize(height * width);
	}

	//add perlin noise to current height map
	void addPerlin(float perlin_freq, bool fractal_ridge, float octave_weight) {
		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

		if (fractal_ridge) {
			noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
		}

		//generate perlin noise
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(perlin_freq);
		for (size_t index = 0; index < height_map.size(); index++)
		{
			height_map[index] += ((noise.GetNoise((float)(index % width), (float)(index / height)) + 1.0) * octave_weight);
		}

	}

	//subtract perlin noise from current height map
	void subPerlin(float perlin_freq, bool fractal_ridge, float octave_weight) {
		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

		if (fractal_ridge) {
			noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
		}

		//generate 1st octave perlin noise
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(perlin_freq);
		for (size_t index = 0; index < height_map.size(); index++)
		{
			height_map[index] -= ((noise.GetNoise((float)(index % width), (float)(index / height)) + 1.0) * octave_weight);

			//limit at 0.0f
			if (height_map[index] < 0) {
				height_map[index] = 0;
			}
		}
	}


	struct Droplet {
		float height;
		float gradientX;
		float gradientY;
	};

	void erode() {
		InitializeBrushIndices(erosionRadius);
		int num_droplets = 100000;

		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<> coordx(0, width - 3);
		std::uniform_real_distribution<> coordy(0, height - 3);


		for (num_droplets; num_droplets > 0; num_droplets--) {
			//generate new droplet
			float startx = coordx(mt);
			float starty = coordy(mt);
			float dirX = 0;
			float dirY = 0;
			float speed = initialSpeed;
			float water = initialWaterVolume;
			float sediment = 0;

			for (int track = 0; track < maxDropletLifetime; track++) {
				int nodeX = (int)startx;
				int nodeY = (int)starty;
				int dropletIndex = nodeY * height + nodeX;
				//int dropletIndex = nodeY * mapSize + nodeX;
				// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
				float cellOffsetX = startx - nodeX;
				float cellOffsetY = starty - nodeY;

				Droplet d = calculate(startx, starty);

				dirX = (dirX * inertia - d.gradientX * (1 - inertia));
				dirY = (dirY * inertia - d.gradientY * (1 - inertia));

				float len = std::sqrt(dirX * dirX + dirY * dirY);
				if (len != 0) {
					dirX /= len;
					dirY /= len;
				}
				startx += dirX;
				starty += dirY;

				if ((dirX == 0.0f && dirY == 0.0f) || !coordCheck(startx, starty)) {
					break;
				}

				// Find the droplet's new height and calculate the deltaHeight
				float newHeight = calculate(startx, starty).height;
				float deltaHeight = newHeight - d.height;

				// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
				float sedimentCapacity = std::max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);

				// If carrying more sediment than capacity, or if flowing uphill:
				if (sediment > sedimentCapacity || deltaHeight > 0) {
					// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
					float amountToDeposit = (deltaHeight > 0) ? std::min(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
					sediment -= amountToDeposit;

					// Add the sediment to the four nodes of the current cell using bilinear interpolation
					// Deposition is not distributed over a radius (like erosion) so that it can fill small pits
					if (dropletIndex > 0 && dropletIndex < height * width) {
						height_map[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);
					}
					if (dropletIndex + 1 > 0 && dropletIndex + 1 < height * width) {
						height_map[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetY);
					}
					if (dropletIndex + width > 0 && dropletIndex + width < height * width) {
						height_map[dropletIndex + width] += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;
					}
					if (dropletIndex + width + 1 > 0 && dropletIndex + width + 1 < height * width) {
						height_map[dropletIndex + width + 1] += amountToDeposit * cellOffsetX * cellOffsetY;
					}

				}
				else {
					// Erode a fraction of the droplet's current carry capacity.
					// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
					float amountToErode = std::min((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);

					// Use erosion brush to erode from all nodes inside the droplet's erosion radius
					for (int brushPointIndex = 0; brushPointIndex < erosionBrushIndices[dropletIndex].size(); brushPointIndex++) {
						int nodeIndex = erosionBrushIndices[dropletIndex][brushPointIndex];
						float weighedErodeAmount = amountToErode * erosionBrushWeights[dropletIndex][brushPointIndex];
						float deltaSediment = (height_map[nodeIndex] < weighedErodeAmount) ? height_map[nodeIndex] : weighedErodeAmount;
						height_map[nodeIndex] -= deltaSediment;
						sediment += deltaSediment;
					}
				}
				// Update droplet's speed and water content
				speed = std::sqrt(speed * speed + deltaHeight * gravity);
				water *= (1 - evaporateSpeed);
			}

		}
	}

	Droplet calculate(float px, float py) {
		int coordX = (int)px;
		int coordY = (int)py;

		// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
		float x = px - coordX;
		float y = py - coordY;

		int nodeIndexNW = coordY * height + coordX;    //x,y
		float heightNW = (nodeIndexNW >= 0 && nodeIndexNW < width * height) ? height_map[nodeIndexNW] : 0.0; //x,y
		float heightNE = (nodeIndexNW + 1 >= 0 && nodeIndexNW + 1 < width * height) ? height_map[nodeIndexNW + 1] : 0.0; //x+1,y
		float heightSW = (nodeIndexNW + width >= 0 && nodeIndexNW + width < width * height) ? height_map[nodeIndexNW + width] : 0.0;
		float heightSE = (nodeIndexNW + width + 1 >= 0 && nodeIndexNW + width + 1 < width * height) ? height_map[nodeIndexNW + width + 1] : 0.0;

		float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
		float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

		float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
		return Droplet{ height = height, gradientX = gradientX, gradientY = gradientY };
	}

	void InitializeBrushIndices(int radius) {
		erosionBrushIndices.resize(width * height);
		erosionBrushWeights.resize(width * height);

		std::vector<int> xOffsets;
		xOffsets.resize(radius * radius * 4);
		std::vector<int> yOffsets;
		yOffsets.resize(radius * radius * 4);
		std::vector<int> weights;
		weights.resize(radius * radius * 4);
		float weightSum = 0;
		int addIndex = 0;

		for (int i = 0; i < erosionBrushIndices.size(); i++) {
			int centreX = i % width;
			int centreY = i / height;

			if (centreY <= radius || centreY >= height - radius || centreX <= radius + 1 || centreX >= width - radius) {
				weightSum = 0;
				addIndex = 0;
				for (int y = -radius; y <= radius; y++) {
					for (int x = -radius; x <= radius; x++) {
						float sqrDst = x * x + y * y;
						if (sqrDst < radius * radius) {
							int coordX = centreX + x;
							int coordY = centreY + y;

							if (coordX >= 0 && coordX < width && coordY >= 0 && coordY < height) {
								float weight = 1 - std::sqrt(sqrDst) / radius;
								weightSum += weight;
								weights[addIndex] = weight;
								xOffsets[addIndex] = x;
								yOffsets[addIndex] = y;
								addIndex++;
							}
						}
					}
				}
			}

			int numEntries = addIndex;
			erosionBrushIndices[i].resize(numEntries);
			erosionBrushWeights[i].resize(numEntries);

			for (int j = 0; j < numEntries; j++) {
				erosionBrushIndices[i][j] = (yOffsets[j] + centreY) * height + xOffsets[j] + centreX;
				erosionBrushWeights[i][j] = weights[j] / weightSum;
			}
		}
	}

};

