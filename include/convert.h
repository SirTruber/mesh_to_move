#pragma once

#include <queue>
#include <vector>
#include <list>
#include <unordered_set>

#include "mesh.h"
namespace mesh
{
using line = std::queue<std::pair<size_t, float>>;

class Cache_crm
{
public:
    size_t breakage = 0;
    size_t count = 0;
public:
    Cache_crm(size_t size);

    bool operator ()();
    bool push( size_t i, line & val);
    size_t pop( size_t i, line & move_value);
private:
    bool _breakpoint = false;
    std::vector<line> _data;
    static constexpr size_t _max_size = 4000000000 / sizeof(std::pair<size_t, float>); // 4 GiB
};

struct Cubic_node
{
    Vec3i pos;
    bool cached = false;
    std::list<size_t> points;

    Cubic_node(const int a = 0, const int b = 0, const int c = 0);
    Cubic_node(const Vec3i& vec);

    void add(const size_t ver);
    void clear();
};

struct CHash
{
    size_t operator()(const Vec3i & data) const noexcept;
};

class Cubic_view
{
public:
    Cubic_view() = delete;
    Cubic_view(const Poly3gon& data, const double radius_smoothing = 1);

    std::list<size_t> get_current();
    std::list<size_t> get_neighbours();
    void mark_cached();
    bool step();
private:
    std::unordered_map<Vec3i, Cubic_node, CHash> _data;
    std::unordered_map<Vec3i, Cubic_node, CHash>::iterator _cur;
};

void convert_direct (mesh::Poly3gon& data,double radius_smoothing = 1,double smooth_coef = 1);

void convert_cached (mesh::Poly3gon& data,double radius_smoothing = 1,double smooth_coef = 1);

void convert_cubic (Poly3gon& data,double radius_smoothing = 1,double smooth_coef = 1);

}
