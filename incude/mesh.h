#pragma once

#include "vec3f.h"
#include <vector>
#include <algorithm>

namespace mesh
{
class Facet;

class Vertex
{
public:
    Vec3f pos;
    std::vector<Facet*> f; // Face List

    Vertex(const Vec3f &vec): pos(vec){};
    Vertex(float a, float b, float c): pos(a,b,c){};

    void mov(const Vec3f &vec): pos(vec){};
    void mov(float a, float b, float c): pos(a,b,c){};

    void join(const Facet* triangle)
    {
        f.push_back(triangle);
    }

    void separate(const Facet* triangle)
    {
        auto it = std::find(f.begin(),f.end(), triangle);
        if (it != std::end(f))
            f.erase(it);
    }
}

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
    Vertex* v[3]; //Vertex list

    Facet (const Vertex* a, const Vertex* b, const Vertex* c )
    {
        v[0] = a;
        v[1] = b;
        v[2] = c;
    }

    Facet (const Facet & other)
    {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
    }

    Facet & operator = (const Facet & other)
    {
        if (this != &other)
        {
            v[0] = other.v[0];
            v[1] = other.v[1];
            v[2] = other.v[2];
        }
        return * this;
    }
}

float get_area(const Facet & triangle)
{
    const Vec3f v1 = triangle.v[1] - triangle.v[0];
    const Vec3f v2 = triangle.v[2] - triangle.v[0];

    return  0.5 * norm(v1 % v2);
}

Vec3f get_center(const Facet & triangle)
{
    Vec3f v = triangle.v[0]->pos + triangle.v[1]->pos + triangle.v[2]->pos;
    return 1.f/3.f * v;
}

Vec3f get_norm(const Facet & triangle)
{
    const Vec3f v1 = triangle.v[1] - triangle.v[0];
    const Vec3f v2 = triangle.v[2] - triangle.v[0];

    return  v1 % v2;
}

class Poly3gon
{
public:
    std::vector <Vertex> v;
    std::vector <Facet> f;

    bool push_back(const Vertex & v1, const Vertex & v2, const Vertex & v3)
    {
        int v_n[3] = [-1, -1, -1];
        Vertex & v_old[3] = [v1, v2, v3];

        for (size_t i = 0; i < v.size(); ++i)
        {
            for (size_t j = 0; j < 3; ++j)
                if (v_n[j] == -1 && v[i] == v_old[j])
                    v_n[j] = i;
        }

        if (v_n[0] != -1 && v_n[1] != -1 && v_n[2] != -1) //existing triangle do not add
            return false;

        for (size_t j = 0; j < 3; ++j)
        {
            if (v_n[j] == -1)
            {
                v.push_back(v_old[j]);
                v_n[j] = v.size() - 1;
            }
        }

        Facet f_new( &v[v_n[0]], &v[v_n[1]], &v[v_n[2]]);
        f.push_back(f_new);
        size_t f_n = f.size() - 1;

        v[v_n[0]].join(& f[f_n]);
        v[v_n[1]].join(& f[f_n]);
        v[v_n[2]].join(& f[f_n]);

        return true;
    }
}
float volume(const Poly3gon& m)
{
    float total_volume = 0.f;
    for (size_t i = 0; i < m.f.size(); ++i)
    {
        total_volume += m.f[i].v[0]->pos * get_norm(m);
    }

    total_volume *= 1.f/6.f;
    return total_volume;
}
}
}

