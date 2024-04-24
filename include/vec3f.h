#pragma once

#include <limits>
#include <math.h>

namespace mesh
{
class Vec3i;

class Vec3f
{
public:
	float x, y, z;

public:
	Vec3f(const float a = 0.0f, const float b = 0.0f, const float c = 0.0f);
	Vec3f(const Vec3i & vec);
	Vec3f & fill (const float d = 0.0f);

	Vec3f operator - ();
	Vec3f & operator += ( const Vec3f & vec);
	Vec3f & operator -= ( const Vec3f & vec);
	Vec3f & operator *= ( const Vec3f & vec);
	Vec3f & operator /= ( const Vec3f & vec);
	Vec3f & operator *= ( const float c);
	Vec3f & operator /= ( const float c);
	bool operator ! () const ;

	Vec3f & set_norm(float p = 1.0f);
};

bool operator == ( const Vec3f & a, const Vec3f & b);
bool operator != ( const Vec3f & a, const Vec3f & b);
Vec3f operator + ( const Vec3f & a, const Vec3f & b);
Vec3f operator - ( const Vec3f & a, const Vec3f & b);
Vec3f operator * ( const Vec3f & vec, const  float c);
Vec3f operator * ( const float c, const Vec3f & vec);
Vec3f operator / ( const Vec3f & vec, const float c);
float operator * ( const Vec3f& a, const Vec3f& b);
Vec3f operator % (const Vec3f & a, const Vec3f & b);

float norm(const Vec3f & vec);

class Vec3i
{
public:
	int x, y, z;

public:
	Vec3i(const int a = 0, const int b = 0, const int c = 0);
	Vec3i(const Vec3f & vec);
	Vec3i & fill (const int d = 0);

	Vec3i operator - ();
	Vec3i & operator += ( const Vec3i & vec);
	Vec3i & operator -= ( const Vec3i & vec);
	Vec3i & operator *= ( const Vec3i & vec);
	Vec3i & operator /= ( const Vec3i & vec);
	Vec3i & operator *= ( const int c);
	Vec3i & operator /= ( const int c);
	bool operator ! () const ;
};

bool operator == ( const Vec3i & a, const Vec3i & b);
bool operator != ( const Vec3i & a, const Vec3i & b);
Vec3i operator + ( const Vec3i & a, const Vec3i & b);
Vec3i operator - ( const Vec3i & a, const Vec3i & b);
Vec3i operator * ( const Vec3i & vec, const  int c);
Vec3i operator * ( const int c, const Vec3i & vec);
Vec3i operator / ( const Vec3i & vec, const int c);
int operator * ( const Vec3i & a, const Vec3i & b);
Vec3i operator % (const Vec3i & a, const Vec3i & b);

int norm(const Vec3i & vec);

}
