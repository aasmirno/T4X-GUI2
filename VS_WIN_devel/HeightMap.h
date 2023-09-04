#pragma once
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <FastNoise/FastNoise.h>
#include "FastNoiseLite.h"

class HeightMap : public BaseMap
{
private:
	//enums
	enum ElevationType : uint16_t { L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11, L12, L13, L14, L15, L16 };
	enum Direction { L, R, U, D };
	enum BoundaryType { CONVERG_CNT, DIVERG_CNT, CONVERG_OCN, DIVERG_OCN, CONVERG_OCNCNT, NONE };

	//heightmap metdata
	std::vector<uint16_t> height_map_ids;
	std::vector<float> height_map;
	float max_height = 0.0f;
	float min_height = 100000000.0f;

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
		int origin;			//flat index origin
		float base_height = 0.0f;	//base height
		int type = 0;		//plate type
		int shelf_size = 1;
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
			else if (height_map[index] < 2.0f / 16.0f) {
				height_map_ids[index] = L2;
			}
			else if (height_map[index] < 3.0f / 16.0f) {
				height_map_ids[index] = L3;
			}
			else if (height_map[index] < 4.0f / 16.0f) {
				height_map_ids[index] = L4;
			}
			else if (height_map[index] < 5.0f / 16.0f) {
				height_map_ids[index] = L5;
			}
			else if (height_map[index] < 6.0f / 16.0f) {
				height_map_ids[index] = L6;
			}
			else if (height_map[index] < 7.0f / 16.0f) {
				height_map_ids[index] = L7;
			}
			else if (height_map[index] < 8.0f / 16.0f) {
				height_map_ids[index] = L8;
			}
			else if (height_map[index] < 9.0f / 16.0f) {
				height_map_ids[index] = L9;
			}
			else if (height_map[index] < 10.0f / 16.0f) {
				height_map_ids[index] = L10;
			}
			else if (height_map[index] < 11.0f / 16.0f) {
				height_map_ids[index] = L11;
			}
			else if (height_map[index] < 12.0f / 16.0f) {
				height_map_ids[index] = L12;
			}
			else if (height_map[index] < 13.0f / 16.0f) {
				height_map_ids[index] = L13;
			}
			else if (height_map[index] < 14.0f / 16.0f) {
				height_map_ids[index] = L14;
			}
			else if (height_map[index] < 15.0f / 16.0f) {
				height_map_ids[index] = L15;
			}
			else {
				height_map_ids[index] = L16;
			}

		}

		for (int i = 0; i < plates.size(); i++) {
			//draw plate origin in white
			height_map_ids[plates[i].origin] = L16;
			for (auto& bpoint : plates[i].boundary_points) {
				height_map_ids[bpoint.first] = L16;
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
	void initialise(int w, int h) {
		BaseMap::initialise(w, h);
		refresh();
	}

	//refresh the height map to 0.0f
	void refresh() {
		max_height = 0;
		height_map.clear();
		height_map.resize(height * width);

		height_map_ids.clear();
		height_map_ids.resize(height * width);

		plates.clear();
		voronoi_ids.clear();
	}

	/*
	* Utility methods
	* ------------------------------------------------------------------------------
	*/
	//add noise to current height map
	void addNoise(FastNoiseLite& noise, float octave_weight) {
		for (size_t index = 0; index < height_map.size(); index++)
		{
			height_map[index] += ((noise.GetNoise((float)(index % width), (float)(index / height)) + 1.0) * octave_weight);
		}
	}

	//condition values: 0 = no condition, 1 = plate check, 2 = above sea level
	int gradientMax(uint8_t condition, int time_out) {
		int start = getRand(2);
		int startX = start % width;
		int startY = start / width;
		bool moved = true;

		while (moved) {
			float curr_max = height_map[startY * width + startX];
			moved = false;
			int xoff[] = { -1, 1, 0, 0 };
			int yoff[] = { 0, 0, -1, 1 };
			int nx = startX;
			int ny = startY;

			for (int i = 0; i < 4; i++) {
				if (coordCheck(startY + yoff[i], startX + xoff[i]) && height_map[(startY + yoff[i]) * width + (startX + xoff[i])] > curr_max) {
					curr_max = height_map[(startY + yoff[i]) * width];
					moved = true;
					nx = startX + xoff[i];
					ny = startY + yoff[i];
				}
			}
			startX = nx;
			startY = ny;
		}

		return startY * width + startX;
	}

	//subtract noise from current height map
	void subNoise(FastNoiseLite& noise, float octave_weight) {
		for (size_t index = 0; index < height_map.size(); index++)
		{
			height_map[index] -= ((noise.GetNoise((float)(index % width), (float)(index / height)) + 1.0) * octave_weight);

			//limit at 0.0f
			if (height_map[index] < 0) {
				height_map[index] = 0;
			}
		}
	}

	//get random point on map given a condition and a time out value
	//condition values: 0 = no condition, 1 = plate check, 2 = above sea level
	int getRand(uint8_t condition, int time_out) {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<> coordx(0, width - 3);
		std::uniform_real_distribution<> coordy(0, height - 3);
		int search_x = (int)coordx(mt);
		int search_y = (int)coordy(mt);
		int iterations = 0;

		while (!orgCheck(search_y * width + search_x)) {
			search_x = (int)coordx(mt);
			search_y = (int)coordy(mt);
			iterations++;
			if (iterations > 30) {
				return -1;
			}
		}

		return search_y * width + search_x;
	}

	int getRand(int g) {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<> coordx(0, width - 3);
		std::uniform_real_distribution<> coordy(0, height - 3);
		int search_x = (int)coordx(mt);
		int search_y = (int)coordy(mt);

		return search_y * width + search_x;
	}

	//get angle between motion vectors
	float angle(MotionVector a, MotionVector b) {
		float dot = (a.x * b.x) + (a.y * b.y);
		float mag_a = std::sqrt(pow(a.x, 2) + pow(a.y, 2));
		float mag_b = std::sqrt(pow(b.x, 2) + pow(b.y, 2));

		float pre_cos = dot / (mag_a * mag_b);
		return acos(pre_cos) * (180.0 / 3.141592653589793238463);
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
			height_map[i] /= height_range;
		}

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
				FAULT_PROFILE_CVG_OC_O[i] = -0.02 * (-i + 50);
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
			}
			else if (i < 32) {
				FAULT_PROFILE_CVG_C[i] = 0.005 * (i - 20);
				FAULT_PROFILE_CVG_OC_C[i] = 0.005 * (i - 20);
			}
			else if (i < 37) {
				FAULT_PROFILE_CVG_C[i] = 0.1;
				FAULT_PROFILE_CVG_OC_C[i] = 0.1;
			}
			else if (i < 50) {
				FAULT_PROFILE_CVG_C[i] = 0.005 * ((-i) + 50);
				FAULT_PROFILE_CVG_OC_C[i] = 0.005 * ((-i) + 50);
			}
		}

		//noise inits
		FastNoiseLite noise;	//simplex
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(0.006);
		auto fnPerlin = FastNoise::New<FastNoise::OpenSimplex2S>();

		std::mt19937 mt(rd());	//rand
		std::uniform_real_distribution<> rand(-1.0, 1.0);

		printf("	Plate 0\n");
		//create n plates
		int num_plates = 15;
		int plate_org = 0;
		plates.clear();

		//generate origins
		printf("creating plates:\n");
		for (int i = 0; i < num_plates; i++) {
			printf("	Plate %d\n", i);
			int plate_org = getRand();
			if (plate_org == -1) {
				break;
			}

			Plate new_plate = Plate{ plate_org };
			new_plate.type = rand(mt) > 0.2 ? 1 : 0;	//assign type
			plates.push_back(new_plate);
		}

		//assign heights
		for (auto& plate : plates) {
			float plate_height = noise.GetNoise((float)(plate.origin % width), (float)(plate.origin / width));
			if (plate.type == 1) {	//limit continential plates 0.45 to 0.5
				plate.base_height = plate_height > 0.5f ? 0.5 : (plate_height < 0.45 ? 0.45 : plate_height);
				height_map[plate.origin] = plate.base_height;
			}
			else {	//limit continential plates 0.1 to 0.3
				plate.base_height = plate_height > 0.3 ? 0.3 : (plate_height < 0.1 ? 0.1 : plate_height);
				height_map[plate.origin] = plate.base_height;
			}
		}

		//assign plates with voronoi method
		printf("assigning points:\n");
		for (int index = 0; index < height_map.size(); index++) {
			int nearest_plate = getNearestPlate(index);
			plates[nearest_plate].indicies.insert({ index, 0 });
		}

		//run relaxation algorithm
		for (int i = 0; i < 3; i++) {
			lloydRelax();
		}

		////reassign indices
		for (int index = 0; index < plates.size(); index++) {
			plates[index].indicies.clear();
		}

		noise.SetFrequency(0.005);
		for (size_t index = 0; index < height_map.size(); index++) {
			int closest_plate_index = getNearestPlateNoise(index, noise);
			//assign point to plate
			plates[closest_plate_index].indicies.insert({ index,0 });
			height_map[index] = plates[closest_plate_index].base_height;

		}

		//assign plate direction vectors
		printf("assigning motion vectors\n");
		for (size_t i = 0; i < plates.size(); i++) {
			plates[i].vector.x = rand(mt);
			plates[i].vector.y = rand(mt);
		}
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
			//get avg of indices
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
		printf("assigning neighbors\n");
		//iterate each plate
		for (int i = 0; i < plates.size(); i++) {
			printf("	on plate: %d\n", i);
			for (auto& index : plates[i].indicies) {
				int cx = index.first % width;
				int cy = index.first / width;
				int x_off[4] = { -1,1,0,0 };
				int y_off[4] = { 0,0,-1,1 };
				for (int off = 0; off < 4; off++) {
					int tx = cx + x_off[off];
					int ty = cy + y_off[off];
					if (coordCheck(tx, ty) && !(plates[i].indicies.find(ty * width + tx) != plates[i].indicies.end())) {
						plates[i].neighbors[getPlate(ty * width + tx)];
						plates[i].boundary_points.insert({ index.first, getPlate(ty * width + tx) });
					}
				}
			}
		}
	}

	//assign plate boundary types
	void assignBoundaries() {
		printf("assigning boundaries\n");
		for (size_t i = 0; i < plates.size(); i++) {
			printf("	on plate: %d\n", i);
			for (auto& neighbor_index : plates[i].neighbors) {
				//calculate boundary type with neighbor
				plates[i].neighbors[neighbor_index.first] = assignBoundary(i, neighbor_index.first);
			}
		}
	}

	//set shelf size for continents
	void assignShelfSize() {
		FastNoiseLite noise;	//simplex	
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(0.05);

		for (int plate_index = 0; plate_index < plates.size(); plate_index++) {
			if (plates[plate_index].type == 1) {
				//generate random shelf size
				float shelf_size = (std::abs(noise.GetNoise((float)(plates[plate_index].origin % width), (float)(plates[plate_index].origin / width))) * 50.0f);	//randomly choose shelf size
				shelf_size = std::max(25.0f, shelf_size);
				plates[plate_index].shelf_size = shelf_size;

				//set neighboring continent shelf size;
				for (auto neighbor_index : plates[plate_index].neighbors) {
					if (plates[neighbor_index.first].type == 1) {
						plates[neighbor_index.first].shelf_size = shelf_size;
					}
				}

			}
		}
	}

	//create continential shelf
	void createShelf() {
		FastNoiseLite noise;	//simplex	
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(0.005);

		for (size_t plate_index = 0; plate_index < plates.size(); plate_index++) {	//for all plates
			if (plates[plate_index].type == 1) {	//for all continential plates
				float height_origin = height_map[plates[plate_index].origin];	//get current plate height

				std::queue<std::pair<int, float>> search_queue; //<index, closest ocean plate height>
				std::unordered_map<int, int> visited;
				//seed bfs queue with all ocean boundary tiles
				for (auto& point : plates[plate_index].boundary_points) {
					if (plates[point.second].type == 0) {
						search_queue.push({ point.first, height_map[plates[point.second].origin] });
					}
				}

				//run bfs from ocean boundary
				while (!search_queue.empty()) {
					auto curr_index = search_queue.front();
					search_queue.pop();
					int cx = curr_index.first % width;
					int cy = curr_index.first / width;
					//assign height based on shelf formula
					float dist_to_bound = distToOcean(curr_index.first, plate_index).first + (noise.GetNoise((float)(cx % width), (float)(cy / width)) * 5);
					height_map[curr_index.first] = std::max((((height_origin - curr_index.second) / plates[plate_index].shelf_size) * dist_to_bound) + curr_index.second, curr_index.second);

					int x_off[4] = { -1,1,0,0 };
					int y_off[4] = { 0,0,-1,1 };
					for (int off = 0; off < 4; off++) {

						int tx = cx + x_off[off];
						int ty = cy + y_off[off];
						auto dist_pair = distToOcean(ty * width + tx, plate_index);
						if (coordCheck(tx, ty)
							&& plates[plate_index].indicies.find(ty * width + tx) != plates[plate_index].indicies.end()
							&& dist_pair.first < plates[plate_index].shelf_size
							&& visited.find(ty * width + tx) == visited.end()
							) {

							//add to queue
							search_queue.push({ ty * width + tx, dist_pair.second });
							visited.insert({ ty * width + tx, 0 });
						}

					}
				}
			}
		}
	}

	//get closest ocean boundary point <distance, ocean plate height>
	std::pair<float, float> distToOcean(int index, int plate) {
		if (!coordCheck(index) || !coordCheck(plate)) {
			return { -1,-1 };
		}
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
					int dist = (int)coordDist(ty * width + tx, curr_index.seed) - (abs(noise.GetNoise((float)tx, (float)ty)) * 5);

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

	//apply a base noise profile
	void applyNoiseProfile(float ocean_level) {
		FastNoiseLite noise;	//simplex
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
		std::random_device rd;
		noise.SetSeed(rd());

		//apply general terrain
		noise.SetFractalType(FastNoiseLite::FractalType_None);
		noise.SetFrequency(0.01);
		for (int index = 0; index < height_map.size(); index++) {
			if (height_map[index] > ocean_level) {
				height_map[index] += std::abs(noise.GetNoise((float)(index % width), (float)(index / width))) * 0.2;
			}
		}

		//apply mountain profile
		noise.SetFrequency(0.01);
		noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
		for (int index = 0; index < height_map.size(); index++) {
			if (height_map[index] > 0.6) {
				height_map[index] += std::abs(noise.GetNoise((float)(index % width), (float)(index / width))) * 0.2;
			}
		}

		//apply fbm fractal
		noise.SetFrequency(0.2);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		for (int index = 0; index < height_map.size(); index++) {
			if (height_map[index] > ocean_level) {
				height_map[index] += std::abs(noise.GetNoise((float)(index % width), (float)(index / width))) * 0.05;
			}
		}
	}

	//utils
	//------------------------------------------------------------------------------
	//check plate origin: returns true if valid origin
	bool orgCheck(int org) {
		//check existing origins and lengths
		for (size_t i = 0; i < plates.size(); i++) {
			if (org == plates[i].origin || coordDist(plates[i].origin, org) < 40.0f) {
				return false;
			}
		}
		return true;
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

	//get closest plate to index (noised)
	int getNearestPlateNoise(int index, FastNoiseLite& noise) {
		int closest_plate_index = 0;
		float dist = 10000000000.0f;
		//find closest plate
		for (size_t i = 0; i < plates.size(); i++) {
			if (coordDist(index, plates[i].origin) + ((noise.GetNoise((float)(index % width), (float)(index / width))) * 50) < dist) {
				closest_plate_index = i;
				dist = coordDist(index, plates[i].origin);
			}
		}
		return closest_plate_index;
	}

	//get plate motion vector
	std::pair<float, float> getMotionVector(int index) {
		if (plates.size() == 0 || getPlate(index) == -1) {
			return { -1,-1 };
		}
		return { plates[getPlate(index)].vector.x, plates[getPlate(index)].vector.y };
	}

	std::vector<int> getBoundaryList(int index) {
		if (plates.size() == 0 || getPlate(index) == -1 || !coordCheck(index)) {
			return { -1 };
		}

		auto& plate = plates[getPlate(index)];
		std::vector<int> nlist;
		for (auto& neighbor : plate.neighbors) {
			nlist.push_back(neighbor.first);
		}
		return nlist;
	}

	//find boundary point fault type
	BoundaryType getBoundaryType(int boundary_point, int plate) {
		int neighbor_index = (*plates[plate].boundary_points.find(boundary_point)).second;
		return (*plates[plate].neighbors.find(neighbor_index)).second;
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

	void applyGaussian(int sigma, int kernel_size) {
		//create convolutional filter
		std::vector<std::pair<int, float>> gaussian;

		//ensure kernel size is acceptable
		kernel_size = (kernel_size % 2 == 1) ? kernel_size : kernel_size + 1;

		//assign offset and weight
		for (int i = 0; i < kernel_size; i++) {
			float gauss_exp = exp(-(pow(i - (kernel_size / 2), 2) / (2 * pow(sigma, 2))));
			float gauss_c = 1 / (std::sqrt(2 * 3.1415926 * pow(sigma, 2)));
			gaussian.push_back({ i - (kernel_size / 2), gauss_c * gauss_exp });
		}


		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				//apply gaussian to points
				int num_points = kernel_size;
				float convolved_value = 0.0f;
				for (auto off : gaussian) {
					if (coordCheck(x + off.first, y)) {
						convolved_value += off.second * height_map[y * width + (x + off.first)];
					}
					else {
						num_points--;
					}
				}
				height_map[y * width + x] = convolved_value;
			}
		}

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				//apply gaussian to points
				int num_points = kernel_size;
				float convolved_value = 0.0f;
				for (auto off : gaussian) {
					if (coordCheck(y + off.first, x)) {
						convolved_value += off.second * height_map[(y + off.first) * width + x];
					}
					else {
						num_points--;
					}
				}
				height_map[y * width + x] = convolved_value;
			}
		}

	}

	void createRivers() {
		for (int i = 0; i < 4; i++) {
			printf("max at: %d\n", gradientMax());
		}
	}
};

