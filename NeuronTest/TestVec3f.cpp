#include "TestVec3f.h"

TestVec3f::TestVec3f()
{
}

TestVec3f::~TestVec3f()
{
}

// This test performs a number of == operations on floating point numbers.
// This is intentional and is correct provided that the calculation path is the same
// for both sides of the equality.
void TestVec3f::run()
{
	Test::run();

	Vec3f zero;
	TEST(zero.x == 0.0f && zero.y == 0.0f && zero.z == 0.0f);

	float AX = 1.0f;
	float AY = 2.0f;
	float AZ = 3.0f;
	Vec3f a(AX, AY, AZ);
	Vec3f b(-AY, AX, AZ);

	TEST(a.x == a[0]);
	TEST(a.y == a[1]);
	TEST(a.z == a[2]);

	Vec3f sum(AX - AY, AX + AY, AZ + AZ);
	TEST((a + b).x == sum.x);
	TEST((a + b).y == sum.y);
	TEST((a + b).z == sum.z);

	Vec3f sub(AX + AY, AY - AX, 0.0f);
	TEST((a - b).x == sub.x);
	TEST((a - b).y == sub.y);
	TEST((a - b).z == sub.z);

	float dotproduct = AX * -AY + AY * AX + AZ * AZ;
	TEST(dot(a, b) == dotproduct);

	Vec3f crossproduct(AY * AZ - AX * AZ, AZ * -AY - AX * AZ, AX * AX + AY * AY);
	TEST(cross(a, b).x == crossproduct.x);
	TEST(cross(a, b).y == crossproduct.y);
	TEST(cross(a, b).z == crossproduct.z);

	Vec3f v(a);
	TEST(v == a);
	v = b;
	TEST(v == b);
	v = b; v += a;
	TEST(v == b + a);
	v = b; v -= a;
	TEST(v == b - a);
	v = b; v *= 2.0f;
	TEST(v == b * 2.0f);
	v = b; v /= 2.0f;
	TEST(v == b / 2.0f);

	TEST(fabs(a.lengthSq() - 14) < TINY_VALUE);
	TEST(fabs(a.length() - sqrtf(14)) < TINY_VALUE);

	Vec3f n = b.normal();
	v = b;
	v.normalize();
	TEST(approxEqual(v, n));
	TEST((n.length() - 1.0f) < TINY_VALUE);
}
