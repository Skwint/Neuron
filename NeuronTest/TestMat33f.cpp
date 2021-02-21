#include "TestMat33f.h"

#include "Neuron/Mat33f.h"
#include "Neuron/Vec3f.h"


TestMat33f::TestMat33f()
{
}

TestMat33f::~TestMat33f()
{
}

void TestMat33f::run()
{
	Test::run();

	Vec3f v(1.0f, 2.0f, 3.0f);
	Mat33f unit;
	
	TEST_APPROX_EQUAL(v, unit * v);

	Mat33f x90 = Mat33f::rotationAroundX(PIf / 2.0f);
	TEST_APPROX_EQUAL(Vec3f(1.0f, -3.0f, 2.0f), x90 * v);

	Mat33f y90 = Mat33f::rotationAroundY(PIf / 2.0f);
	TEST_APPROX_EQUAL(Vec3f(3.0f, 2.0f, -1.0f), y90 * v);

	Mat33f z90 = Mat33f::rotationAroundZ(PIf / 2.0f);
	TEST_APPROX_EQUAL(Vec3f(-2.0f, 1.0f, 3.0f), z90 * v);

	auto y180 = y90 * y90;
	TEST_APPROX_EQUAL(Vec3f(-1.0f, 2.0f, -3.0f), y180 * v);
}
