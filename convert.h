#pragma once

#include <queue>
#include <vector>
#include <list>
#include <unordered_map>

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

// struct Cubic_node
// {
//     Vec3i pos;
//
//     std::list<size_t> point;
// }
//
// class Cubic_view
// {
// public:
//     Cubic_view(const Poly3gon& data, const double radius_smoothing);
// private:
//     unordered_map<std::string, Cubic_node> _data
// };

void convert_cached (mesh::Poly3gon& data,double radius_smoothing = 1,double smooth_coef = 1);

void convert (mesh::Poly3gon& data,double radius_smoothing = 1,double smooth_coef = 1);
}
