#include <vector>
#include <algorithm>
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>

#include "../include/mesh.h"

using namespace mesh;


Vertex::Vertex(const Vec3f& vec): pos(vec)
{};

Vertex::Vertex(float a, float b, float c): pos(a,b,c)
{};

void Vertex::mov(const Vec3f &vec)
{
    pos = vec;
};

void Vertex::mov(float a, float b, float c)
{
    pos = Vec3f(a,b,c);
};

void Vertex::join(Facet & triangle)
{
    f.push_back(&triangle);
};

Vec3f mesh::operator - (const Vertex & a, const Vertex & b)
{
    return a.pos - b.pos;
};

inline bool mesh::operator == (const Vertex & a, const Vertex & b)
{
    return a.pos == b.pos;
};

inline bool mesh::operator != (const Vertex & a, const Vertex & b)
{
    return a.pos != b.pos;
};

Facet::Facet(Vertex & a, Vertex & b, Vertex & c)
{
    v[0] = &a;
    v[1] = &b;
    v[2] = &c;
};

Facet::Facet(const Facet& other)
{
    v[0] = other.v[0];
    v[1] = other.v[1];
    v[2] = other.v[2];
};

Facet& Facet::operator =(const Facet& other)
{
    if (this != &other)
    {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
    }
    return * this;
};

inline Vec3f Facet::center()
{
    Vec3f vec = v[0]->pos + v[1]->pos + v[2]->pos;
    return 1.f/3.f * vec;
};

inline Vec3f Facet::normal()
{
    const Vec3f v1 = *v[1] - *v[0];
    const Vec3f v2 = *v[2] - *v[0];

    return  v1 % v2;
};

inline float Facet::area()
{
    return  0.5 * norm(normal());
};

void Poly3gon::push_back(const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
    int v_n[3] = {-1, -1, -1};
    const Vertex * v_old[3] = {&v1, &v2, &v3};
    // for (size_t i = 0; i < v.size(); ++i)
    // {
    //     for (size_t j = 0; j < 3; ++j)
    //         if (v_n[j] == -1 && *v[i] == *v_old[j])
    //             v_n[j] = i;
    // }

    for (size_t i = 0; i < 3; ++i)
    {
        std::string hash = std::to_string(v_old[i]->pos.x) + std::to_string(v_old[i]->pos.y) + std::to_string(v_old[i]->pos.z);
        auto it = _unique_v.find(hash);
        if (it != _unique_v.end())
        {
            v_n[i] = static_cast<int>(it->second);
        }
        else
        {
            v.push_back(std::make_unique<Vertex>(v_old[i]->pos));
            v_n[i] = v.size() - 1;
            _unique_v.emplace(hash, v_n[i]);
        }
    }

    f.push_back(std::make_unique<Facet>( *v[v_n[0]], *v[v_n[1]], *v[v_n[2]] ));
    size_t f_n = f.size() - 1;

    v[v_n[0]]->join( *f[f_n]);
    v[v_n[1]]->join( *f[f_n]);
    v[v_n[2]]->join( *f[f_n]);
};

bool Poly3gon::loadSTLText (const std::string& filename, std::string& text)
{
    std::ifstream input(filename);
    if (!input)
        return false;
    std::string line;

    bool isSolid = false;

    std::getline(input,line, ' ');
    if(line != "solid")
        return false;
    //TODO read a triangle and push to data
    return false;
};

bool Poly3gon::loadSTLBin (const std::string& filename, std::string& text)
{
    std::ifstream input(filename, std::ios::binary);
    if (!input)
        return false;

    char message[80];
    std::uint32_t num;
    float v[12];
    Vertex vec[3];

    if (!input.read(message, 80)) return false;
    text.assign(message, 80);
    if(!input.read(reinterpret_cast<char*>(&num), sizeof(num))) return false;
    for (std::uint32_t i = 0; i < num; ++i)
    {
        if(!input.read( reinterpret_cast<char*>(v), sizeof(v))) return false;
        if(!input.read(message, 2)) return false;

        vec[0].mov(v[3],v[4],v[5]);
        vec[1].mov(v[6],v[7],v[8]);
        vec[2].mov(v[9],v[10],v[11]);

        this->push_back(vec[0],vec[1],vec[2]);
    }
    _unique_v.clear();
    input.close();
    return true;
};

bool Poly3gon::saveSTLText(const std::string& filename, const std::string& text)
{
    std::ofstream output(filename);
    if (!output)
        return false;

    output << "solid " << text << std::endl;

    std::uint32_t num = static_cast<std::uint32_t>(f.size());
    Vec3f vec[4];

    for (std::uint32_t i = 0; i < num; ++i)
    {
        vec[0] = f[i]->normal();
        vec[1] = f[i]->v[0]->pos;
        vec[2] = f[i]->v[1]->pos;
        vec[3] = f[i]->v[2]->pos;

        vec[0] /= norm(vec[0]);
        output << "facet normal " << vec[0].x << " " << vec[0].y << " " << vec[0].z
        << std::endl << "outer loop"
        << std::endl << "vertex " << vec[1].x << " " << vec[1].y << " " << vec[1].z
        << std::endl << "vertex " << vec[2].x << " " << vec[2].y << " " << vec[2].z
        << std::endl << "vertex " << vec[3].x << " " << vec[3].y << " " << vec[3].z
        << std::endl << "endloop" << std::endl << "endfacet" << std::endl;
    }
    output << "endsolid";

    output.close();
    return true;
};

bool Poly3gon::saveSTLBin (const std::string& filename, const std::string& text)
{
    std::ofstream output(filename, std::ios::binary);
    if (!output)
        return false;

    char message[80];
    std::uint32_t num = static_cast<std::uint32_t>(f.size());
    Vec3f vec[4];
    float pos[12];

    for (size_t i = 0; i < 80; ++i)
        message[i] = i < text.size() ? text[i] : '0';

    output.write(message, 80);
    output.write(reinterpret_cast<char*>(&num), sizeof(num));
    message[0] = message[1] = 0;


    for (std::uint32_t i = 0; i < num; ++i)
    {
        vec[0] = f[i]->normal().set_norm();
        vec[1] = f[i]->v[0]->pos;
        vec[2] = f[i]->v[1]->pos;
        vec[3] = f[i]->v[2]->pos;

        for(size_t j = 0 ; j < 4; ++j)
        {
            pos[3 * j    ] = vec[j].x;
            pos[3 * j + 1] = vec[j].y;
            pos[3 * j + 2] = vec[j].z;
        }
        output.write(reinterpret_cast<char*>(pos), sizeof(pos));
        output.write(message, 2);
    }

    output.close();
    return true;
};

float Poly3gon::volume()
{
    float total_volume = 0.0f;
    for (size_t i = 0; i < f.size(); ++i)
        total_volume += f[i]->v[0]->pos * f[i]->normal();

    total_volume *= 1.f/6.f;
    return total_volume;
};

float Poly3gon::area()
{
    float total_area = 0.0f;
    for(size_t i = 0; i < f.size(); ++i)
        total_area += f[i]->area();

    return total_area;
}

/*
std::size_t VHash::operator()(const mesh::Vertex* v) const noexcept
{
    size_t hash;
    const float pos[3] = {v->pos.x,
                          v->pos.y,
                          v->pos.z};
    const unsigned char *key  = reinterpret_cast<const unsigned char *> (pos);

    for (size_t i = 0; i < 3 ; ++i)
    {
        hash += std::hash<float>{}(pos[i]);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}*/
