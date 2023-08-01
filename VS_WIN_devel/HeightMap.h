#pragma once
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <cmath>
#include "FastNoiseLite.h"

class HeightMap
{
private:
	enum ElevationType : uint8_t { L1, L2, L3, L4, L5, L6, L7, L8 };
	enum Direction { L, R, U, D };
	std::vector<uint8_t> height_map_ids;
	std::vector<float> height_map;
	float max_height = 0.0f;
	int width = 0;
	int height = 0;

	//tectonics parameters
	struct Plate {
		int origin;	//flat index origin
		std::unordered_map<int, int> indicies;	//points belonging to plate
		std::unordered_map<int, int> neighbors; //list of neighbors
		std::vector < std::pair<int, Direction> > boundary_points; //list of boundary points <index, direction>

		struct MotionVector {	//Motion vector
			float magnitude;
			uint8_t dir;
		};

		MotionVector vector;

		void assignVector() {
			std::random_device rd;
			std::mt19937 mt(rd());

			std::uniform_real_distribution<> rand(0, 9);
			vector.dir = (int)rand(mt);
			vector.magnitude = rand(mt) / 8.0f;

		}
	};

	std::vector<uint8_t> voronoi_ids;
	std::vector<Plate> plates;

	//erosion params
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
			else {
				height_map_ids[index] = L8;
			}
		}

		for (int i = 0; i < plates.size(); i++) {
			//draw plate origin in white
			height_map_ids[plates[i].origin] = L8;

			//draw plate boundaries in white
			for (auto pair : plates[i].boundary_points) {
				height_map_ids[pair.first] = L8;
			}

		}

		return &height_map_ids[0];
	}

	//create voronoi id diagram from plate origin points
	uint8_t* getPlateVoronoi() {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<> diffusion(0, 10.0f);
		voronoi_ids.clear();
		voronoi_ids.resize(width * height);
		//brute force algortihm, should be fine as it is only run once and with relatively small input sizes
		for (size_t index = 0; index < height_map.size(); index++) {
			int closest_plate_org = 0;
			float dist = 10000000000.0f;
			for (size_t i = 0; i < plates.size(); i++) {
				if (coordDist(index, plates[i].origin) < dist) {
					closest_plate_org = i;
					dist = coordDist(index, plates[i].origin);
				}
			}

			voronoi_ids[index] = closest_plate_org;
		}

		return &voronoi_ids[0];
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

	/*
	* Utility methods
	*/
	//add noise to current height map
	void addNoise(float perlin_freq, bool fractal_ridge, float octave_weight) {
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

	//subtract noise from current height map
	void subNoise(float perlin_freq, bool fractal_ridge, float octave_weight) {
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

	//find local maximum
	int findMaximum() {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<> coordx(0, width - 3);
		std::uniform_real_distribution<> coordy(0, height - 3);
		int search_x = (int)coordx(mt);
		int search_y = (int)coordy(mt);
		bool changed = true;

		printf("start (%d,%d)\n", search_x, search_y);

		//find vertical max
		while (changed) {
			float curr_height = height_map[search_y * width + search_x];
			if (search_y - 1 > 0 && height_map[(search_y - 1) * width + search_x] > curr_height) { //check north neighbor
				changed = true;
				search_y -= 1;
			}
			else if (search_y + 1 < height && height_map[(search_y + 1) * width + search_x] > curr_height) {
				changed = true;
				search_y += 1;
			}
			else if (search_x - 1 > 0 && height_map[search_y * width + (search_x - 1)] > curr_height) {
				changed = true;
				search_x -= 1;
			}
			else if (search_x + 1 < width && height_map[search_y * width + (search_x + 1)] > curr_height) {
				changed = true;
				search_x += 1;
			}
			else {
				changed = false;
			}
		}

		printf("end (%d,%d)\n", search_x, search_y);

		return search_y * width + search_x;
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

	//euclidean distance between flat indices
	float coordDist(int i1, int i2) {
		return std::sqrt(pow(i1 % width - i2 % width, 2) + pow(i1 / width - i2 / width, 2));
	}

	//find the equation for a line between 2 flat indices
	int* lineEquation(int i1, int i2) {

	}

	/*
		Tectonic map generation section and utilities
	*/
	//initialise plates: noise + sectioning + randomise direction vectors
	void initialisePlates() {
		refresh();

		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

		//generate noise
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(0.004);
		for (size_t index = 0; index < height_map.size(); index++)
		{
			height_map[index] += ((noise.GetNoise((float)(index % width), (float)(index / height)) + 1.0) * 0.3);
		}

		//find n local maximums, assign as plate origins
		int n = 6;
		int plate_org = findMaximum();
		plates.clear();
		plates.push_back(Plate{plate_org});
		//create remaining plates
		for (int i = 0; i < n - 1; i++) {
			plate_org = findMaximum();
			while (orgCheck(plate_org)) {
				plate_org = findMaximum();
			}
			plates.push_back(Plate{ plate_org });
		}

		//create plate structs using voronoi method
		for (size_t index = 0; index < height_map.size(); index++) {
			int closest_plate_index = 0;
			float dist = 10000000000.0f;
			//find closest plate
			for (size_t i = 0; i < plates.size(); i++) {
				if (coordDist(index, plates[i].origin) < dist) {
					closest_plate_index = i;
					dist = coordDist(index, plates[i].origin);
				}
			}
			//assign point to plate
			plates[closest_plate_index].indicies.insert({index,0});
		}

		//refresh vornoi ids
		getPlateVoronoi();

		//assign neighbors and boundaries
		assignNeighbors();

		//assign plate random vectors
		for (auto plate : plates) {
			plate.assignVector();
		}
	}

	//check plate origin: returns false if valid org
	bool orgCheck(int org) {
		//check existing origins and lengths
		for (size_t i = 0; i < plates.size(); i++) {
			if (org == plates[i].origin || coordDist(plates[i].origin, org) < 100) {
				return true;
			}
		}
		return false;
	}

	//assign plate neighbors using dfs
	void assignNeighbors() {
		//iterate each plate
		for (size_t i = 0; i < plates.size(); i++) {
			//run dfs from origin
			std::stack<int> dfs_stack;
			dfs_stack.push(plates[i].origin);
			std::unordered_map<int, int> visited;

			while (!dfs_stack.empty()) {
				//get coordinate
				int curr_index = dfs_stack.top();
				dfs_stack.pop();

				int currX = curr_index % width;
				int currY = curr_index / width;

				//check left (check bounds and check visited)
				if (coordCheck(currX - 1, currY)  && visited.find(currY * width + (currX - 1)) == visited.end()) {
					//if the index is in the same plate, add it to the stack
					if (plates[i].indicies.find(currY * width + (currX - 1)) != plates[i].indicies.end()) {
						dfs_stack.push(currY * width + (currX - 1));
					}
					//else the index belongs to a neighbor, mark it as such
					else {
						plates[i].neighbors[voronoi_ids[currY * width + (currX - 1)]];
						plates[i].boundary_points.push_back({ curr_index, L });
					}
				}

				//check right
				if (coordCheck(currX + 1, currY) && visited.find(currY * width + (currX + 1)) == visited.end()) {
					//if the index is in the same plate, add it to the stack
					if (plates[i].indicies.find(currY * width + (currX + 1)) != plates[i].indicies.end()) {
						dfs_stack.push(currY * width + (currX + 1));
					}
					//else the index belongs to a neighbor, mark it as such
					else {
						plates[i].neighbors[voronoi_ids[currY * width + (currX + 1)]];
						plates[i].boundary_points.push_back({ curr_index, R });
					}
				}

				//check up
				if (coordCheck(currX, currY - 1) && visited.find((currY-1) * width + currX) == visited.end()) {
					//if the index is in the same plate, add it to the stack
					if (plates[i].indicies.find((currY - 1) * width + currX) != plates[i].indicies.end()) {
						dfs_stack.push((currY - 1) * width + currX);
					}
					//else the index belongs to a neighbor, mark it as such
					else {
						plates[i].neighbors[voronoi_ids[(currY - 1) * width +  currX]];
						plates[i].boundary_points.push_back({ curr_index, U });
					}
				}
				 
				//check down
				if (coordCheck(currX, currY + 1) && visited.find((currY + 1) * width + currX) == visited.end()) {
					//if the index is in the same plate, add it to the stack
					if (plates[i].indicies.find((currY + 1) * width + currX) != plates[i].indicies.end()) {
						dfs_stack.push((currY + 1) * width + currX);
					}
					//else the index belongs to a neighbor, mark it as such
					else {
						plates[i].neighbors[voronoi_ids[(currY + 1) * width + currX]];
						plates[i].boundary_points.push_back({ curr_index, U });
					}
				}

				//set current index as visited
				visited.insert({ curr_index,0 });
			}
		}
	}


	//run tectonic sim


	/*
		Erosion section
	*/
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

