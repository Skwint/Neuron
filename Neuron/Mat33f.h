#ifndef MAT33F_H
#define MAT33F_H

#include <cmath>

#include "NeuronSim/Constants.h"
#include "Vec3f.h"

class Mat33f
{
public:
	Mat33f() { v[0] = v[4] = v[8] = 1.0f; v[1] = v[2] = v[3] = v[5] = v[6] = v[7] = 0.0f; }

	inline static Mat33f rotationAroundX(float angle)
	{
		Mat33f rot;
		float ca = cos(angle);
		float sa = sin(angle);
		rot[4] = ca;
		rot[5] = -sa;
		rot[7] = sa;
		rot[8] = ca;
		return rot;
	}

	inline static Mat33f rotationAroundY(float angle)
	{
		Mat33f rot;
		float ca = cos(angle);
		float sa = sin(angle);
		rot[0] = ca;
		rot[2] = sa;
		rot[6] = -sa;
		rot[8] = ca;
		return rot;
	}

	inline static Mat33f rotationAroundZ(float angle)
	{
		Mat33f rot;
		float ca = cos(angle);
		float sa = sin(angle);
		rot[0] = ca;
		rot[1] = -sa;
		rot[3] = sa;
		rot[4] = ca;
		return rot;
	}

	inline float & operator[](int index) { return v[index]; }
	inline const float & operator[](int index) const { return v[index]; }


public:
	float v[9];
};

inline Vec3f operator*(const Mat33f & mat, const Vec3f & vec)
{
	Vec3f res;
	for (int rr = 0; rr < 3; ++rr)
	{
		for (int cc = 0; cc < 3; ++cc)
		{
			res[rr] += mat[rr * 3 + cc] * vec[cc];
		}
	}
	return res;
}

inline Mat33f operator*(const Mat33f & mat1, const Mat33f & mat2)
{
	Mat33f res;
	for (int rr = 0; rr < 3; ++rr)
	{
		for (int cc = 0; cc < 3; ++cc)
		{
			float val = 0.0f;
			for (int ii = 0; ii < 3; ++ii)
			{
				 val += mat1[rr * 3 + ii] * mat2[ii * 3 + cc];
			}
			res[rr * 3 + cc] = val;
		}
	}
	return res;
}

#endif
