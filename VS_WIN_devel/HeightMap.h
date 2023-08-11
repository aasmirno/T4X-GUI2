#pragma once
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FastNoiseLite.h"

class HeightMap
{
private:
	//enums
	enum ElevationType : uint16_t { L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11, L12 };
	enum Direction { L, R, U, D };
	enum BoundaryType { CONVERG_CNT, DIVERG_CNT, CONVERG_OCN, DIVERG_OCN, CONVERG_OCNCNT, NONE};

	//heightmap metdata
	std::vector<uint16_t> height_map_ids;
	std::vector<float> height_map;
	float max_height = 0.0f;
	float min_height = 100000000.0f;
	int width = 0;
	int height = 0;

	//tectonics parameters
	struct MotionVector {	//Motion vector
		float x;
		float y;
	};
	struct TecTile {
		int seed;
		int index;
		int dist_to_seed;
	};
	struct CPTile {
		int index;
		int scale_position;
		float neighbor_height;
		int dist_to_origin;
	};
	struct Plate {
		int origin;	//flat index origin
		int type;	//plate type
		int shelf_size = 0;
		std::unordered_map<int, int> indicies;	//points belonging to plate
		std::unordered_map<int, BoundaryType> neighbors; //list of neighbors: <plate id, >
		std::unordered_map< int, int> boundary_points; //list of boundary points <index, neighboring plate>

		MotionVector vector;
	};

	//list of all boundaries
	struct BoundaryLocation {
		int index;
		int neighbor_plate_index;
		int boundary_type;
	};
	std::vector<std::vector<BoundaryLocation>> fault_list;


	const int FPROFILE_CONST = 50 - 1;
	//continent continent profiles
	float FAULT_PROFILE_CVG_C[50];
	float FAULT_PROFILE_DVG_C[50];
	//ocean ocean profiles
	float FAULT_PROFILE_DVG_O[50];

	//ocean continent profiles
	float FAULT_PROFILE_CVG_OC_O[50]; //ocean side
	float FAULT_PROFILE_CVG_OC_C[50]; //continent side

	std::vector<uint16_t> voronoi_ids;
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
	uint16_t* getIDArray() {
		//assign ids
		for (size_t index = 0; index < height_map.size(); index++) {
			if (height_map[index] < 1.0f / 12.0f) {
				height_map_ids[index] = L1;
			}
			else if (height_map[index] < 2.0f / 12.0f) {
				height_map_ids[index] = L2;
			}
			else if (height_map[index] < 3.0f / 12.0f) {
				height_map_ids[index] = L3;
			}
			else if (height_map[index] < 4.0f / 12.0f) {
				height_map_ids[index] = L4;
			}
			else if (height_map[index] < 5.0f / 12.0f) {
				height_map_ids[index] = L5;
			}
			else if (height_map[index] < 6.0f / 12.0f) {
				height_map_ids[index] = L6;
			}
			else if (height_map[index] < 7.0f / 12.0f) {
				height_map_ids[index] = L7;
			}
			else if (height_map[index] < 8.0f / 12.0f) {
				height_map_ids[index] = L8;
			}
			else if (height_map[index] < 9.0f / 12.0f) {
				height_map_ids[index] = L9;
			}
			else if (height_map[index] < 10.0f / 12.0f) {
				height_map_ids[index] = L10;
			}
			else if (height_map[index] < 11.0f / 12.0f) {
				height_map_ids[index] = L11;
			}
			else {
				height_map_ids[index] = L12;
			}

		}

		for (int i = 0; i < plates.size(); i++) {
			//draw plate origin in white
			height_map_ids[plates[i].origin] = L12;
			for (auto& bpoint : plates[i].boundary_points) {
				height_map_ids[bpoint.first] = L12;
			}
		}

		return &height_map_ids[0];
	}

	//create plate overlay
	uint16_t* getPlateOverlay() {
		voronoi_ids.clear();
		voronoi_ids.resize(width * height);

		//assign based on ids
		for (size_t p_index = 0; p_index < plates.size(); p_index++) {
			for (auto& index : plates[p_index].indicies) {
				voronoi_ids[index.first] = p_index;
			}
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
	* ------------------------------------------------------------------------------
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

		return search_y * width + search_x;

		////find vertical max
		//while (changed) {
		//	float curr_height = height_map[search_y * width + search_x];
		//	if (search_y - 1 > 0 && height_map[(search_y - 1) * width + search_x] > curr_height) { //check north neighbor
		//		changed = true;
		//		search_y -= 1;
		//	}
		//	else if (search_y + 1 < height && height_map[(search_y + 1) * width + search_x] > curr_height) {
		//		changed = true;
		//		search_y += 1;
		//	}
		//	else if (search_x - 1 > 0 && height_map[search_y * width + (search_x - 1)] > curr_height) {
		//		changed = true;
		//		search_x -= 1;
		//	}
		//	else if (search_x + 1 < width && height_map[search_y * width + (search_x + 1)] > curr_height) {
		//		changed = true;
		//		search_x += 1;
		//	}
		//	else {
		//		changed = false;
		//	}
		//}

		//return search_y * width + search_x;
	}

	//compress heightmap into 0 1 range
	void compress() {
		for (size_t i = 0; i < height_map.size(); i++) {
			if (height_map[i] > max_height) {
				max_height = height_map[i];
			}
		}
		for (size_t i = 0; i < height_map.size(); i++) {
			if (height_map[i] < min_height) {
				min_height = height_map[i];
			}
		}

		float height_range = max_height - min_height;
		for (size_t i = 0; i < height_map.size(); i++) {
			height_map[i] += abs(min_height);
			height_map[i] / height_range;
		}

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
	float lineEquation(int i1, int i2) {
		int x1 = i1 % width; int y1 = i2 / width;
		int x2 = i2 % width; int y2 = i2 / width;

		return (float)(y2 - y1) / (float)(x1 - x2);
	}

	/*
		Tectonic map generation section and utilities
		------------------------------------------------------------------------------
	*/
	//initialise plates: noise + sectioning + randomise direction vectors
	void initialisePlates() {
		refresh();
		double c = 0.1 / pow((double)FPROFILE_CONST + 1, 2.0);
		double c2 = 0.15 / pow((double)FPROFILE_CONST - 4.5, 3.0);

		//initialise profiles
		for (int i = 0; i < FPROFILE_CONST + 1; i++) {
			FAULT_PROFILE_DVG_C[i] = std::min((pow((double)i, 2.0) * c) - 0.05, 0.0);
			FAULT_PROFILE_DVG_O[i] = i < 33 ? (i < 4 ? (0.00375 * i) + 0.035 : -(0.0015 * i) + 0.05) : 0.0;
			//ocean side subduction
			if (i < 5) {
				FAULT_PROFILE_CVG_OC_O[i] = -(0.02 * i);
			}
			else if (i < 15) {
				FAULT_PROFILE_CVG_OC_O[i] = -0.1;
			}
			else if (i < 50) {
				FAULT_PROFILE_CVG_OC_O[i] = -0.02 * (-i+50);
			}
			//continent convergence
			if (i < 5) {
				FAULT_PROFILE_CVG_C[i] = 0.05;
			}
			else if (i < 10) {
				FAULT_PROFILE_CVG_C[i] = -(0.005 * (i - 10)) + 0.005;
			}
			else if (i < 30) {
				FAULT_PROFILE_CVG_C[i] = -(0.00125 * (i - 30));
			}
			else {
				FAULT_PROFILE_CVG_C[i] = 0.0;
			}
			//conitnent side subduction
			if (i < 5) {
				FAULT_PROFILE_CVG_OC_C[i] = 0.0;
			} else if (i < 32) {
				FAULT_PROFILE_CVG_C[i] = 0.005 * (i - 20);
				FAULT_PROFILE_CVG_OC_C[i] = 0.005 * (i - 20);
			}else if (i < 37) {
				FAULT_PROFILE_CVG_C[i] = 0.1;
				FAULT_PROFILE_CVG_OC_C[i] = 0.1;
			}
			else if (i < 50) {
				FAULT_PROFILE_CVG_C[i] = 0.005 * ((-i) + 50);
				FAULT_PROFILE_CVG_OC_C[i] = 0.005 *((-i) + 50);
			}
		}

		//noise inits
		FastNoiseLite noise;	//simplex
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(0.006);
		std::mt19937 mt(rd());	//rand
		std::uniform_real_distribution<> rand(-1.0, 1.0);

		printf("creating plates:\n");
		printf("	Plate 0\n");
		//create n plates
		int n = 15;
		int plate_org = findMaximum();
		plates.clear();

		Plate new_plate = Plate{ plate_org };
		new_plate.type = rand(mt) > 0.2 ? 1 : 0;	//assign type

		//assign plate_height
		float plate_height = noise.GetNoise((float)(new_plate.origin % width), (float)(new_plate.origin / width));
		plate_height = abs(plate_height);
		if (new_plate.type == 1) {	//limit continential plates 0.45 to 0.5
			height_map[new_plate.origin] = plate_height > 0.5f ? 0.5 : (plate_height < 0.45 ? 0.45 : plate_height);
		}
		else {	//limit continential plates 0.1 to 0.3
			height_map[new_plate.origin] = plate_height > 0.3 ? 0.3 : (plate_height < 0.1 ? 0.1 : plate_height);
		}

		plates.push_back(new_plate);

		//create remaining plates
		for (int i = 0; i < n - 1; i++) {
			printf("	creating plate %d\n", i + 1);
			//try new points until criteria are fullfilled
			plate_org = findMaximum();
			while (orgCheck(plate_org)) {
				plate_org = findMaximum();
			}
			new_plate = Plate{ plate_org };
			new_plate.type = rand(mt) > 0.2 ? 1 : 0;	//assign type
			//assign plate_height
			plate_height = noise.GetNoise((float)(new_plate.origin % width), (float)(new_plate.origin / width));
			plate_height = abs(plate_height);
			if (new_plate.type == 1) {	//limit continential plates 0.45 to 0.5
				height_map[new_plate.origin] = plate_height > 0.5f ? 0.5 : (plate_height < 0.45 ? 0.45 : plate_height);
			}
			else {	//limit continential plates 0.1 to 0.3
				height_map[new_plate.origin] = plate_height > 0.3 ? 0.3 : (plate_height < 0.1 ? 0.1 : plate_height);
			}

			plates.push_back(new_plate);
		}

		//assign plates with voronoi method
		printf("assigning points:\n");
		for (int index = 0; index < height_map.size(); index++) {
			int nearest_plate = getNearestPlate(index);
			plates[nearest_plate].indicies.insert({ index, 0 });
			height_map[index] = height_map[plates[nearest_plate].origin];
		}

		//assign plate direction vectors
		printf("assigning motion vectors\n");
		for (size_t i = 0; i < plates.size(); i++) {
			plates[i].vector.x = rand(mt);
			plates[i].vector.y = rand(mt);
		}

		//run 3 relaxations
		printf("running lloyd relaxation\n");
		//lloydRelax();
		//lloydRelax();
		//lloydRelax();

		//compress heightmap
		//compress();
	}

	class DelaunayTriangle {
		//x.y pairs of triangle vertices
		std::vector<std::pair<int, int>> points;

		//check if a point is within the triangle using barycentric method
		bool inTriangle(int x, int y) {
			std::pair<int, int>& p1 = points[0];
			std::pair<int, int>& p2 = points[1];
			std::pair<int, int>& p3 = points[2];

			int denom = (p2.second - p3.second) * (p1.first - p3.first) + (p3.first - p2.first) * (p1.second - p3.second);
			double a = ((p2.second - p3.second) * (x - p3.first) + (p3.first - p2.first) * (y - p3.second)) / (double)denom;
			double b = ((p3.second - p1.second) * (x - p3.first) + (p1.first - p3.first) * (y - p3.second)) / (double)denom;
			double c = 1 - a - b;

			return 0 <= a && a <= 1 && 0 <= b && b <= 1 && 0 <= c && c <= 1;
		}

		//rearrange points based on 
		void reassign() {
			//find tallest point
			for (auto& point : points) {
			}

		}

		DelaunayTriangle(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3) {
			int i = 0;
			for (int i = 0; i < 3; i++) {

			}
		}

		//return true if a point is in the circumcircle of the triangle
		bool inCircle(int x, int y) {
			//check if the point is in triangle first
			if (inTriangle(x, y))
				return true;
			//use determinant method to find circumcircular belonging
		}
	};

	//delaunay triangulation
	void delaunay() {
		std::vector<DelaunayTriangle> point_list;	//list of triangles
		//add point
		//determine if breakage occurs
		//restructure
	}

	//run lloyd relaxation algorithm (kmeans)
	void lloydRelax() {
		for (auto& plate : plates) {
			//get avg
			float avg_x = 0.0f;
			float avg_y = 0.0f;
			int iterations = 1;
			for (auto& index : plate.indicies) {
				int curr_index = index.first;
				avg_x += (curr_index % width);
				avg_y += (curr_index / width);
				iterations++;
			}
			avg_x /= iterations;
			avg_y /= iterations;

			//move origin to avg
			plate.origin = (int)(avg_y)*width + (int)(avg_x);
			plate.indicies.clear();
		}

		//reassign points
		for (int index = 0; index < height_map.size(); index++) {
			int nearest_plate = getNearestPlate(index);
			plates[nearest_plate].indicies.insert({ index, 0 });
			height_map[index] = height_map[plates[nearest_plate].origin];
		}
	}

	//assign plate neighbors using dfs
	void assignNeighbors() {
		//iterate each plate
		for (size_t i = 0; i < plates.size(); i++) {
			printf("	on plate: %d\n", i);
			//run dfs from origin
			std::stack<int> dfs_stack;
			dfs_stack.push(plates[i].origin);
			std::unordered_map<int, int> visited;

			while (!dfs_stack.empty()) {
				//get coordinate
				int curr_index = dfs_stack.top();
				dfs_stack.pop();
				//printf("		point: %d", curr_index);

				int currX = curr_index % width;
				int currY = curr_index / width;

				//check left (check bounds and check visited)
				if (coordCheck(currX - 1, currY) && visited.find(currY * width + (currX - 1)) == visited.end()) {
					//if the index is in the same plate, add it to the stack
					if (plates[i].indicies.find(currY * width + (currX - 1)) != plates[i].indicies.end()) {
						dfs_stack.push(currY * width + (currX - 1));
					}
					//else the index belongs to a neighbor, mark it as such
					else {
						int neighbor = getPlate(currY * width + (currX - 1));
						plates[i].neighbors[neighbor];
						plates[i].boundary_points.insert({ curr_index, neighbor });
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
						int neighbor = getPlate(currY * width + (currX + 1));
						plates[i].neighbors[neighbor];
						plates[i].boundary_points.insert({ curr_index, neighbor });
					}
				}

				//set current index as visited
				visited.insert({ curr_index, 0 });
			}
		}
	}

	//assign plate boundary types
	void assignBoundaries() {
		for (size_t i = 0; i < plates.size(); i++) {
			for (auto& neighbor_index : plates[i].neighbors) {
				//calculate boundary type with neighbor
				plates[i].neighbors[neighbor_index.first] = assignBoundary(i, neighbor_index.first);
			}
		}
	}

	//create continential shelf
	void createShelf() {
		FastNoiseLite noise;	//simplex	
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(0.05);

		for (size_t plate_index = 0; plate_index < plates.size(); plate_index++) {	//for all plates
			float shelf_size = std::pow((noise.GetNoise((float)(plates[plate_index].origin % width), (float)(plates[plate_index].origin / width)) * 10),2.0f);	//randomly choose shelf size

			if (plates[plate_index].type == 1) {	//for all continential plates
				float height_origin = height_map[plates[plate_index].origin];	//get current plate height

				for (auto& point : plates[plate_index].indicies) {	//for all indices in plate
					auto dist_pair = minimumBoundary(point.first, plate_index);
					
					float dist_to_bound = dist_pair.first +(noise.GetNoise((float)(point.first % width), (float)(point.first / width)) * 5);
					if (dist_to_bound < shelf_size) {
						float neighbor_height = dist_pair.second;
						height_map[point.first] = (((height_origin - neighbor_height) / shelf_size) * dist_to_bound) + neighbor_height;
					}
				}
			}
		}
	}

	//get closest ocean boundary point
	std::pair<float, float> minimumBoundary(int index, int plate) {
		int closest_index = 0;
		float neighbor_height = 0.0f;
		float min_dist = std::max(width, height);
		for (auto& boundary_pair : plates[plate].boundary_points) {
			if (
				plates[boundary_pair.second].type == 0	//ocean boundary
				&& coordDist(index, boundary_pair.first) < min_dist	//smaller than current dist
				) {
				neighbor_height = height_map[plates[boundary_pair.second].origin];
				min_dist = coordDist(index, boundary_pair.first);
				closest_index = boundary_pair.first;
			}
		}
		return { min_dist,  neighbor_height };
	}

	//apply boundary transforms
	void applyTransforms() {
		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

		//generate noise
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(0.05);

		for (size_t plate_index = 0; plate_index < plates.size(); plate_index++) {
			printf("	on plate: %d\n", plate_index);
			//run seeded bfs on boundary points
			std::queue<TecTile> bfs_queue;
			std::unordered_map<int, int> visited;

			for (auto& point : plates[plate_index].boundary_points) {
				bfs_queue.push(TecTile{ point.first, point.first, 0 });
				visited.insert({ point.first, 0 });
			}

			while (!bfs_queue.empty()) {
				TecTile curr_index = bfs_queue.front();
				bfs_queue.pop();

				int cx = curr_index.index % width;
				int cy = curr_index.index / width;

				//apply f profile
				BoundaryType boundary = getBoundaryType(curr_index.seed, plate_index);
				switch (boundary) {
				case CONVERG_CNT:
					height_map[curr_index.index] += FAULT_PROFILE_CVG_C[curr_index.dist_to_seed];
					break;
				case DIVERG_CNT:
					height_map[curr_index.index] += FAULT_PROFILE_DVG_C[curr_index.dist_to_seed];
					break;
				case DIVERG_OCN:
					height_map[curr_index.index] += FAULT_PROFILE_DVG_O[curr_index.dist_to_seed];
					break;
				case CONVERG_OCNCNT:
					if (plates[plate_index].type == 0) {	//apply ocean side
						height_map[curr_index.index] += FAULT_PROFILE_CVG_OC_O[curr_index.dist_to_seed];
					}
					else { //apply continent side
						height_map[curr_index.index] += FAULT_PROFILE_CVG_OC_C[curr_index.dist_to_seed];
					}
					break;
				}

				int x_off[4] = { -1,1,0,0 };
				int y_off[4] = { 0,0,-1,1 };
				for (int off = 0; off < 4; off++) {

					int tx = cx + x_off[off];
					int ty = cy + y_off[off];
					int dist = (int)coordDist(ty * width + tx, curr_index.seed) - (abs(noise.GetNoise((float)tx, (float)ty))*5);

					if (coordCheck(tx, ty)
						&& plates[plate_index].indicies.find(ty * width + tx) != plates[plate_index].indicies.end()
						&& dist < FPROFILE_CONST
						&& visited.find(ty * width + tx) == visited.end()
						) {
						//add to queue
						bfs_queue.push(TecTile{ curr_index.seed , ty * width + tx, dist });
						visited.insert({ ty * width + tx, 0 });
					}

				}

			}
		}
	}

	//utils
	//------------------------------------------------------------------------------
	//check plate origin: returns false if valid org
	bool orgCheck(int org) {
		//check existing origins and lengths
		for (size_t i = 0; i < plates.size(); i++) {
			if (org == plates[i].origin || coordDist(plates[i].origin, org) < 40.0f) {
				return true;
			}
		}
		return false;
	}

	//find plate to which a point belongs
	int getPlate(int index) {
		for (size_t i = 0; i < plates.size(); i++) {
			if (plates[i].indicies.find(index) != plates[i].indicies.end()) {
				return i;
			}
		}
		return -1;
	}

	//get plate type at an index
	int getPlateType(int index) {
		if (plates.size() == 0 || getPlate(index) == -1) {
			return -1;
		}
		return plates[getPlate(index)].type;
	}

	//get plate shelf size
	float getContinentialShelfSize(int index) {
		if (plates.size() == 0 || getPlate(index) == -1) {
			return -1;
		}
		return plates[getPlate(index)].shelf_size;
	}

	//get closest plate to index (unoised)
	int getNearestPlate(int index) {
		float min_dist = 10000000.0f;
		int np = 0;
		for (size_t i = 0; i < plates.size(); i++) {
			if (coordDist(index, plates[i].origin) < min_dist) {
				min_dist = coordDist(index, plates[i].origin);
				np = i;
			}
		}
		return np;
	}

	//get plate motion vector
	std::pair<float, float> getMotionVector(int index) {
		if (plates.size() == 0 || getPlate(index) == -1) {
			return { -1,-1 };
		}
		return { plates[getPlate(index)].vector.x, plates[getPlate(index)].vector.y };
	}

	std::vector<int> getBoundaryList(int index) {
		if (plates.size() == 0 || getPlate(index) == -1) {
			return { -1 };
		}
		auto& plate = plates[getPlate(index)];
		std::vector<int> nlist;
		for (auto& neighbor : plate.neighbors) {
			nlist.push_back(neighbor.second);
		}
	}

	//find boundary point fault type
	BoundaryType getBoundaryType(int boundary_point, int plate) {
		int neighbor_index = (*plates[plate].boundary_points.find(boundary_point)).second;
		return (*plates[plate].neighbors.find(neighbor_index)).second;
	}

	//get angle between motion vectors
	float angle(MotionVector a, MotionVector b) {
		float dot = (a.x * b.x) + (a.y * b.y);
		float mag_a = std::sqrt(pow(a.x, 2) + pow(a.y, 2));
		float mag_b = std::sqrt(pow(b.x, 2) + pow(b.y, 2));

		float pre_cos = dot / (mag_a * mag_b);
		return acos(pre_cos) * (180.0 / 3.141592653589793238463);
	}

	//get boundary type
	BoundaryType assignBoundary(int plate_1, int plate_2) {
		Plate p1 = plates[plate_1];
		Plate p2 = plates[plate_2];

		if (angle(p1.vector, p2.vector) < 90) {
			if ((p1.type == 0 && p2.type == 1) || (p1.type == 1 && p2.type == 0)) {
				return CONVERG_OCNCNT;
			}
			else if (p1.type == 0 && p2.type == 0) {
				return CONVERG_OCN;
			}
			else if (p1.type == 1 && p2.type == 1) {
				return CONVERG_CNT;
			}
		}
		else {
			if (p1.type == 0 && p2.type == 0) {
				return DIVERG_OCN;
			}
			if (p1.type == 1 && p2.type == 1) {
				return DIVERG_CNT;
			}
		}

		return NONE;
	}


	/*
		Erosion section
		------------------------------------------------------------------------------
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

