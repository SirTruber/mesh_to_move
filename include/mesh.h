#pragma once

#include <memory>
#include <unordered_map>

#include "vec3f.h"

namespace mesh
{
class Facet;

class Vertex
{
public:
    Vec3f pos;
    std::vector<std::weak_ptr<Facet>> f; // Face List

public:
    Vertex(const Vec3f& vec);
    Vertex(float a = 0, float b = 0, float c = 0);
    void mov(const Vec3f &vec);
    void mov(float a, float b, float c);
    void join(std::shared_ptr<Facet> triangle);
};

Vec3f operator - (const Vertex & a, const Vertex & b);
bool operator == (const Vertex & a, const Vertex & b);
bool operator != (const Vertex & a, const Vertex & b);


struct VHash
{
    std::size_t operator()(const mesh::Vertex* v) const noexcept; // Jenkins one-at-a-time hash
};

class Facet
{
public:
    std::weak_ptr<Vertex> v[3]; //Vertex list

public:
    Facet() = delete;
    Facet(std::shared_ptr<Vertex> a, std::shared_ptr<Vertex> b, std::shared_ptr<Vertex> c);
    Facet(const Facet& other);
    Facet& operator =(const Facet& other);

    Vec3f center();
    Vec3f normal();
    float area();
};

class Poly3gon
{
public:
    std::vector <std::shared_ptr<Vertex>> v;
    std::vector <std::shared_ptr<Facet>>  f;

public:
    void push_back(const Vertex& v1, const Vertex& v2, const Vertex& v3);

    bool loadSTLText (const std::string& filename, std::string& text);
    bool loadSTLBin  (const std::string& filename, std::string& text);
    bool saveSTLText (const std::string& filename, const std::string& text);
    bool saveSTLBin  (const std::string& filename, const std::string& text);

    float volume();
    float area();
private:
    std::unordered_map< std::string ,size_t> _unique_v;
};
}


