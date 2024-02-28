#pragma once

#include <memory>
#include <vector>
#include <algorithm>

#include "vec3f.h"

namespace mesh
{
class Facet;

class Vertex
{
public:
    Vec3f pos;
    std::vector<std::weak_ptr<Facet>> f; // Face List

    Vertex(const Vec3f& vec): pos(vec){};
    Vertex(float a = 0, float b = 0, float c = 0): pos(a,b,c){};

    void mov(const Vec3f &vec){ pos = vec; }

    void mov(float a, float b, float c) { pos = Vec3f(a,b,c); }

    void join(std::shared_ptr<Facet> triangle)
    {
        f.push_back(triangle);
    }

    void separate(std::weak_ptr<Facet> triangle)
    {
        //auto it = std::find(f.begin(),f.end(), triangle);
        //if (it != std::end(f))
        //    f.erase(it);
    }
};

Vec3f operator - (const Vertex & a, const Vertex & b)
{
    return a.pos - b.pos;
}

inline bool operator == (const Vertex & a, const Vertex & b)
{
    return a.pos == b.pos;
}

inline bool operator != (const Vertex & a, const Vertex & b)
{
    return a.pos != b.pos;
}

class Facet
{
public:
    std::weak_ptr<Vertex> v[3]; //Vertex list

    Facet () = delete;

    Facet (std::shared_ptr<Vertex> a, std::shared_ptr<Vertex> b, std::shared_ptr<Vertex> c )
    {
        v[0] = a;
        v[1] = b;
        v[2] = c;
    }

    Facet (const Facet& other)
    {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
    }

    Facet & operator = (const Facet& other)
    {
        if (this != &other)
        {
            v[0] = other.v[0];
            v[1] = other.v[1];
            v[2] = other.v[2];
        }
        return * this;
    }
};

inline Vec3f get_center(const Facet& triangle)
{
    Vec3f v = triangle.v[0].lock()->pos + triangle.v[1].lock()->pos + triangle.v[2].lock()->pos;
    return 1.f/3.f * v;
}

inline Vec3f get_normal(const Facet& triangle)
{
    const Vec3f v1 = *triangle.v[1].lock() - *triangle.v[0].lock();
    const Vec3f v2 = *triangle.v[2].lock() - *triangle.v[0].lock();

    return  v1 % v2;
}

inline float get_area(const Facet& triangle)
{
    return  0.5 * norm(get_normal(triangle));
}

class Poly3gon
{
public:
    std::vector <std::shared_ptr<Vertex>> v;
    std::vector <std::shared_ptr<Facet>>  f;

    void push_back(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        int v_n[3] = {-1, -1, -1};
        const Vertex * v_old[3] = {&v1, &v2, &v3};

        for (size_t i = 0; i < v.size(); ++i)
        {
            for (size_t j = 0; j < 3; ++j)
                if (v_n[j] == -1 && *v[i] == *v_old[j])
                    v_n[j] = i;
        }

        for (size_t j = 0; j < 3; ++j)
        {
            if (v_n[j] == -1)
            {
                v.push_back(std::make_shared<Vertex>(v_old[j]->pos));
                v_n[j] = v.size() - 1;
            }
        }

        f.push_back(std::make_shared<Facet>( v[v_n[0]], v[v_n[1]], v[v_n[2]] ));
        size_t f_n = f.size() - 1;

        v[v_n[0]]->join( f[f_n]);
        v[v_n[1]]->join( f[f_n]);
        v[v_n[2]]->join( f[f_n]);

    }
};
float get_volume(const Poly3gon& m)
{
    float total_volume = 0.f;
    for (size_t i = 0; i < m.f.size(); ++i)
    {
        total_volume += m.f[i]->v[0].lock()->pos * get_normal(*m.f[i]);
    }

    total_volume *= 1.f/6.f;
    return total_volume;
}
}

