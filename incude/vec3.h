#pragma once

#include <limits>
#include <math.h>

namespace mesh
{

class Vec3f
{
public:
	float x, y, z;

	Vec3f(float a = 0, float b = 0, float c = 0) : x(a), y(b), z(c) {};

	Vec3f & fill (float d = 0) : x(d), y(d), z(d) {};

	Vec3f operator - ()
	{
		return Vec3f(-x, -y, -z);
	};

	Vec3f & operator += ( const Vec3f & vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
        return * this;
	};

	Vec3f & operator -= ( const Vec3f & vec)
	{
		x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return * this;
	};

	Vec3f & operator *= ( const Vec3f & vec)
	{
		x *= vec.x;
        y *= vec.y;
        z *= vec.z;
        return * this;
	};

	Vec3f & operator /= ( const Vec3f & vec)
	{
		x /= vec.x;
        y /= vec.y;
        z /= vec.z;
        return * this;
	};

	Vec3f & operator /= ( const float c)
	{
		x /= c;
        y /= c;
        z /= c;
        return * this;
	};

	bool operator ! () const
	{
        return !x && !y && !z;
	};

	Vec3f & set_norm(float p = 1)
	{
		const float t = x * x + y * y + z * z;
		if (t != 0)
		{
			p / t;
			x *= p;
			y *= p;
			z *= p;
		}
		return * this;
	}
}

inline bool operator == ( const Vec3f & a, const Vec3f & b)
{
	return std::abs(a.x - b.x) < std::numeric_limits<float>::epsilon() &&
           std::abs(a.y - b.y) < std::numeric_limits<float>::epsilon() &&
           std::abs(a.z - b.z) < std::numeric_limits<float>::epsilon();
};

inline bool operator != ( const Vec3f & a, const Vec3f & b)
{
	return ! (a == b);
};

inline Vec3f operator + ( const Vec3f & a, const Vec3f & b)
{
	return Vec3f( a.x + b.x, a.y + b.y, a.z + b.z );
};

inline Vec3f operator - ( const Vec3f & a, const Vec3f & b)
{
	return Vec3f ( a.x - b.x, a.y - b.y, a.z - b.z );
};

inline Vec3f operator * ( const Vec3f & vec, float c);
{
	return Vec3f (vec.x * c, vec.y * c, vec.z * c);
};

inline Vec3f operator * ( float c, const Vec3f & vec)
{
	return Vec3f (vec.x * c, vec.y * c, vec.z * c);
};

inline Vec3f operator / ( const Vec3f & vec, float c);
{
	return Vec3f (vec.x / c, vec.y / c, vec.z / c);
};

inline float operator * ( const Vec3f& a, const Vec3f& b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
};

inline Vec3f operator % (const Vec3f & a, const Vec3f & b)
{
	return Vec3f(a.y * b.z - a.z * b.y,
				 a.z * b.x - a.x * b.z,
				 a.x * b.y - a.y * b.x);
}

float norm(const Vec3f & vec)
{
	return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

}
