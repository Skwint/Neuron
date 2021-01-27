#ifndef EUCLID_VEC2F_H
#define EUCLID_VEC2F_H

#include <ostream>
#include <math.h>

#include "EuclidNamespace.h"

EUCLID_NAMESPACE_BEGIN

class Vec2f
{
public:
    inline Vec2f()						{ e[0] = 0; e[1] = 0; }
	inline Vec2f(float ax, float ay)	{ e[0] = ax; e[1] = ay; }
    inline ~Vec2f() { };
    inline void Set(float ax, float ay) { e[0] = ax; e[1] = ay; }
    inline       float & operator[](int idx)       { return e[idx]; }
    inline const float & operator[](int idx) const { return e[idx]; }
	static inline Vec2f & Alias(float * f)              { return(*(reinterpret_cast<Vec2f *>(f))); }
	static inline const Vec2f & Alias(const float * f)  { return(*(reinterpret_cast<const Vec2f *>(f))); }    
public:
	union{
		struct{
			float x;
			float y;
		};
		float e[2];
	};
};
    
inline bool  operator==(const Vec2f & a,const Vec2f & b) { return((a[0] == b[0]) && (a[1] == b[1])); }
inline bool  operator!=(const Vec2f & a,const Vec2f & b) { return((a[0] != b[0]) || (a[1] != b[1])); }
inline float vectorSum (const Vec2f & a                ) { return(a[0] + a[1]); }
inline float vectorProd(const Vec2f & a                ) { return(a[0] * a[1]); }
inline Vec2f operator- (const Vec2f & a                ) { return(Vec2f(-a[0], -a[1])); }
inline Vec2f operator+ (const Vec2f & a,const Vec2f & b) { return(Vec2f(a[0] + b[0], a[1] + b[1])); }
inline Vec2f operator- (const Vec2f & a,const Vec2f & b) { return(Vec2f(a[0] - b[0], a[1] - b[1])); }
inline Vec2f operator* (const float   a,const Vec2f & b) { return(Vec2f(a    * b[0], a    * b[1])); }
inline Vec2f operator* (const Vec2f & a,const float   b) { return(Vec2f(a[0] * b   , a[1] * b   )); }
inline Vec2f operator/ (const Vec2f & a,const float   b) { return(Vec2f(a[0] / b   , a[1] / b   )); }
inline Vec2f operator+=(      Vec2f & a,const Vec2f & b) { a[0] += b[0]; a[1] += b[1]; return(a); }
inline Vec2f operator-=(      Vec2f & a,const Vec2f & b) { a[0] -= b[0]; a[1] -= b[1]; return(a); }
inline Vec2f operator*=(      Vec2f & a,const float   b) { a[0] *= b; a[1] *= b; return(a); }
inline Vec2f operator/=(      Vec2f & a,const float   b) { a[0] /= b; a[1] /= b; return(a); }
        
inline float dot(const Vec2f& a,const Vec2f& b)
{
	return (a[0] * b[0] + a[1] * b[1]);
}

inline float lengthSq(const Vec2f& a)
{
    return a[0]*a[0] + a[1]*a[1];
}

inline float length(const Vec2f& a)
{
    return sqrt(lengthSq(a));
}

inline Vec2f normal(const Vec2f& a)
{
    return a / length(a);
}

inline Vec2f safenormal(const Vec2f& a)
{
    float m = length(a);
    if(m < 0.00001)
    {
        return(Vec2f(0.0, 1.0));
    }
    else
    {
        return(a / m);
    }
}

inline void normalize(Vec2f & a)
{
    float m = length(a);
    a /= m;
}
    
inline Vec2f conj(const Vec2f& a)
{
    return Vec2f(-a[0], -a[1]);
}

inline Vec2f inv(const Vec2f& a)
{
    return 1.f / lengthSq(a) * conj(a);
}

inline Vec2f slerp(const Vec2f & a, const Vec2f & b, float weight)
{
    return(((1.0f - weight) * a) + (weight * b));
}

inline std::ostream & operator<<(std::ostream & os, const Vec2f & v)
{
    os << "[" << v[0] << "," << v[1] << "]";
    return(os);
}

EUCLID_NAMESPACE_END

#endif //Vec2f_H
