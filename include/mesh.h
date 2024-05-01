#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "vec3f.h"

using namespace LA;

namespace mesh
{
class Facet;

class Vertex
{
public:
    Vec3f pos;
    std::vector<Facet*> f; // Face List

public:
    Vertex(const Vec3f& vec);
    Vertex(float a = 0, float b = 0, float c = 0);
    void mov(const Vec3f & vec);
    void mov(float a, float b, float c);
    void join(Facet & triangle);
};

Vec3f operator - (const Vertex & a, const Vertex & b);
bool operator == (const Vertex & a, const Vertex & b);
bool operator != (const Vertex & a, const Vertex & b);

class Facet
{
public:
    Vertex* v[3]; //Vertex list

public:
    Facet() = delete;
    Facet(Vertex & a, Vertex & b, Vertex & c);
    Facet(const Facet& other);
    Facet& operator =(const Facet& other);

    Vec3f center() const;
    Vec3f normal() const;
    float area()   const;
};

class Poly3gon
{
public:
    std::vector <std::unique_ptr<Vertex>> v;
    std::vector <std::unique_ptr<Facet >> f;

public:
    bool loadSTLText (const std::string& filename, std::string& text);
    bool loadSTLBin  (const std::string& filename, std::string& text);
    bool saveSTLText (const std::string& filename, const std::string& text);
    bool saveSTLBin  (const std::string& filename, const std::string& text);

    float volume() const;
    float area()   const;
private:
    std::unordered_map< std::string ,size_t> _unique_v;
private:
    void push_back(const Vertex& v1, const Vertex& v2, const Vertex& v3);
};
}
