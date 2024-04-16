#pragma once

#include <limits>
#include <math.h>

namespace mesh
{

class Vec3f
{
public:
	float x, y, z;

public:
	Vec3f(float a = 0, float b = 0, float c = 0);
	Vec3f & fill (float d = 0);

	Vec3f operator - ();
	Vec3f & operator += ( const Vec3f & vec);
	Vec3f & operator -= ( const Vec3f & vec);
	Vec3f & operator *= ( const Vec3f & vec);
	Vec3f & operator /= ( const Vec3f & vec);
	Vec3f & operator *= ( const float c);
	Vec3f & operator /= ( const float c);
	bool operator ! () const ;

	Vec3f & set_norm(float p = 1);
};

bool operator == ( const Vec3f & a, const Vec3f & b);
bool operator != ( const Vec3f & a, const Vec3f & b);
Vec3f operator + ( const Vec3f & a, const Vec3f & b);
Vec3f operator - ( const Vec3f & a, const Vec3f & b);
Vec3f operator * ( const Vec3f & vec, float c);
Vec3f operator * ( float c, const Vec3f & vec);
Vec3f operator / ( const Vec3f & vec, float c);
float operator * ( const Vec3f& a, const Vec3f& b);
Vec3f operator % (const Vec3f & a, const Vec3f & b);

float norm(const Vec3f & vec);

}
