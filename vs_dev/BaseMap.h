#pragma once
#include <iostream>
#include <random>
#include <cmath>

class BaseMap {
protected:
	int height = 0;
	int width = 0;

	//check bounds
	bool coordCheck(int x, int y) {
		return x >= 0 && x < width && y >= 0 && y < height;
	}
	bool coordCheck(int index) {
		return coordCheck(index % width, index / width);
	}

	//euclidean distance between flat indices
	float coordDist(int i1, int i2) {
		return std::sqrt(pow(i1 % width - i2 % width, 2) + pow(i1 / width - i2 / width, 2));
	}
public:
	int getHeight() { return height; }
	int getWidth() { return width; }
	int getSize() { return height * width; }

	virtual void initialise(int w, int h) {
		width = w; height = h;
	}
};