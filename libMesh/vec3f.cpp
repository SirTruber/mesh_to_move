#include "../include/vec3f.h"

namespace mesh{
Vec3f::Vec3f(float a, float b, float c) : x(a), y(b), z(c)
{};

Vec3f& Vec3f::fill (float d)
{
    x = d;
	y = d;
	z = d;

	return * this;
}

Vec3f Vec3f::operator - ()
{
    return Vec3f(-x, -y, -z);
};

Vec3f& Vec3f::operator += ( const Vec3f & vec)
{
    x += vec.x;
	y += vec.y;
	z += vec.z;
    return * this;
};

Vec3f& Vec3f::operator -= ( const Vec3f & vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return * this;
};

Vec3f& Vec3f::operator *= ( const Vec3f & vec)
{
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;
    return * this;
};

Vec3f& Vec3f::operator /= ( const Vec3f & vec)
{
    x /= vec.x;
    y /= vec.y;
    z /= vec.z;
    return * this;
};

Vec3f& Vec3f::operator *= ( const float c)
{
    x *= c;
    y *= c;
    z *= c;
    return * this;
};

Vec3f& Vec3f::operator /= ( const float c)
{
    x /= c;
    y /= c;
    z /= c;
    return * this;
};

bool Vec3f::operator ! () const
{
	return !x && !y && !z;
};

Vec3f& Vec3f::set_norm(float p)
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
};

bool operator == ( const Vec3f & a, const Vec3f & b)
{
	return std::abs(a.x - b.x) < std::numeric_limits<float>::epsilon() &&
           std::abs(a.y - b.y) < std::numeric_limits<float>::epsilon() &&
           std::abs(a.z - b.z) < std::numeric_limits<float>::epsilon();
};

bool operator != ( const Vec3f & a, const Vec3f & b)
{
	return ! (a == b);
};

Vec3f operator + ( const Vec3f & a, const Vec3f & b)
{
	return Vec3f( a.x + b.x, a.y + b.y, a.z + b.z );
};

Vec3f operator - ( const Vec3f & a, const Vec3f & b)
{
	return Vec3f ( a.x - b.x, a.y - b.y, a.z - b.z );
};

Vec3f operator * ( const Vec3f & vec, float c)
{
	return Vec3f (vec.x * c, vec.y * c, vec.z * c);
};

Vec3f operator * ( float c, const Vec3f & vec)
{
	return Vec3f (vec.x * c, vec.y * c, vec.z * c);
};

Vec3f operator / ( const Vec3f & vec, float c)
{
	return Vec3f (vec.x / c, vec.y / c, vec.z / c);
};

float operator * ( const Vec3f& a, const Vec3f& b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
};

Vec3f operator % (const Vec3f & a, const Vec3f & b)
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
