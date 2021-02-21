#ifndef SPIKE_H
#define SPIKE_H

#include <vector>

class Spike
{
public:
	enum Shape
	{
		SHAPE_SQUARE,
		SHAPE_TRIANGLE,
		SHAPE_GAUSS,
		SHAPE_EXPONENTIAL
	};

	Spike();

	void setSpike(Shape shape, int duration);
	Shape shape() const { return mShape; }
	int duration() const { return mDuration; }
	float potential(int index) const { return mPotentials[index]; }
	void saveSpike(std::ofstream & ofs);
	void loadSpike(std::ifstream & ifs);

private:
	Shape mShape;
	int mDuration;
	std::vector<float> mPotentials;
};

#endif
