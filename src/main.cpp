#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <utility>
#include <ctime>
//#include <unordered_set>

#include "../include/mesh.h"

#define MAX_CACHE 1000000000;
typedef std::weak_ptr<mesh::Vertex>  Weak_ver;
typedef std::shared_ptr<mesh::Vertex> Shared_ver;
typedef std::shared_ptr<mesh::Facet> Shared_tr;

template<typename T, typename U>
inline bool equals(const std::weak_ptr<T> t, const std::weak_ptr<U> u)
{
    return !t.owner_before(u) && !u.owner_before(t);
}

template<typename T, typename U>
inline bool equals(const std::weak_ptr<T> t, const std::shared_ptr<U> u)
{
    return !t.owner_before(u) && !u.owner_before(t);
}

struct line
{
    std::queue<float> value;
    std::queue<size_t> column;
};

class Cache_crm
{
public:
    size_t breakage = 0;
    size_t count = 0;
public:
    Cache_crm(size_t size) :_data(size)
    {};

    bool operator ()()
    {
        return !_breakpoint;
    }
    bool push( std::queue<size_t> & distance,std::queue<float> & val)
    {
        if (_breakpoint)
            return false;

        count += val.size();

        if (count > _max_size)
            _breakpoint = true;

        auto it = _data.begin();
        while(!val.empty())
        {
            for(size_t j = 0; j < distance.front(); ++j)
                ++it;

            it->value.push(val.front());
            it->column.push(breakage + distance.front());
            distance.pop();
            val.pop();
        }
        ++breakage;
        return true;
    }
    size_t pop(std::queue<size_t> & move_column, std::queue<float> & move_value)
    {
        move_value = std::move(_data.begin()->value);
        move_column = std::move(_data.begin()->column);
        count -= move_value.size();
        _data.pop_front();
        return breakage;
    }
private:
    bool _breakpoint = false;
    std::list<line> _data;
    static constexpr size_t _max_size = 1000000000 / sizeof(std::pair<size_t, float>);
};
// std::vector<float> distance_map(const mesh::Poly3gon& data)
// {
//     size_t n = data.v.size();
//     std::vector<float> res((n * n - n) / 2);
//
//     for (size_t )
// }

// std::vector<Weak_ver> find_adjacent(Shared_ver& ver)
// {
//     std::vector <Weak_ver> res;
//     for (size_t i = 0; i < ver->f.size();++i)
//     {
//         Shared_tr triangle = ver->f[i].lock();
//
//         if(!triangle)
//             continue;
//
//         for(size_t j = 0; j < 3; ++j)
//         {
//             Weak_ver tmp = triangle->v[j];
//             bool was_find = false;
//
//             for (size_t k = 0; k < res.size(); ++k )
//             {
//                 if(equals(res[k],tmp))
//                     was_find = true;
//             }
//             if(!was_find)
//                 res.push_back(tmp);
//         }
//     }
//     return res;
// }

// mesh::Vec3f compute_displacement(Shared_ver ver, double radius_smoothing)
// {
//     mesh::Vec3f start(ver->pos);
//     mesh::Vec3f end;
//     std::queue<mesh::Vertex *> close_points;
//     std::unordered_set<std::string> points_counter;
//
//     close_points.push(ver.get());
//     std::string hash = std::to_string(start.x) + std::to_string(start.y) + std::to_string(start.z);
//     points_counter.emplace(hash);
//
//     float mass = 0;
//     double r_inv = 1/radius_smoothing;
//     while ( !close_points.empty() )
//     {
//         mesh::Vertex & root_ver = * close_points.front();
//         for (size_t i = 0; i < root_ver.f.size(); ++i)
//         {
//             mesh::Facet & root_tr = *root_ver.f[i].lock();
//             for(size_t j = 0; j < 3; ++j)
//             {
//                 mesh::Vertex & selected_ver = *root_tr.v[j].lock();
//                 hash = std::to_string(selected_ver.pos.x) + std::to_string(selected_ver.pos.y) + std::to_string(selected_ver.pos.z);
//                 if(
//                     (points_counter.find(hash) == points_counter.end()) &&
//                     (norm(selected_ver.pos - start) < radius_smoothing)
//                 )
//                 {
//                     close_points.push(&selected_ver);
//                     points_counter.emplace(hash);
//                 }
//             }
//         }
//         close_points.pop();
//         float m = static_cast<float>(1 - norm(root_ver.pos) * r_inv);
//         end += root_ver.pos * m;
//         mass += m;
//     }
//     end /= mass;
//     return end;
// }

void convert_cached (mesh::Poly3gon& data,double radius_smoothing = 1,double smooth_coef = 1)
{
    size_t size = data.v.size();
    double r_inv = 1/radius_smoothing;
    Cache_crm saved_value(size + 1);
    std::vector<mesh::Vec3f> shift(size);

    size_t read_counter = 0;
    size_t cal_counter = 0;
    size_t good_cal = 0;
    for(size_t i = 0; i < size; ++i)
    {
        float mass = 0;
        mesh::Vec3f b;

        std::queue<size_t> computed_column;
        std::queue<float> computed_value;
        std::queue<size_t> to_cache_column;
        std::queue<float> to_cache_value;

        size_t j = saved_value.pop(computed_column, computed_value);

        while( !computed_value.empty())
        {
            b += data.v[computed_column.front()]->pos * computed_value.front();
            mass += computed_value.front();

            computed_value.pop();
            computed_column.pop();
            ++read_counter;
        }

        for(; j < size; ++j)
        {
            const float distance = mesh::norm(data.v[i]->pos - data.v[j]->pos);
            ++cal_counter;
            if (distance < radius_smoothing)
            {
                ++good_cal;
                float m = static_cast<float>(1 - distance * r_inv);
                b += data.v[j]->pos * m;
                mass += m;

                if(saved_value())
                {
                    to_cache_column.push(j - i);
                    to_cache_value.push(m);
                }
            }
        }
        saved_value.push(to_cache_column, to_cache_value);

        if (mass > 0.0f)
            b /= mass;
        shift[i] = (1 - smooth_coef) * b + smooth_coef * data.v[i]->pos;
    }
    float tmp = 1.0f /size;
    std::cout << read_counter << ": read. " << cal_counter << ": cal =" << 200.0f * cal_counter * tmp * tmp - 100.0f << " % max " << good_cal << ": good_cal ="<< 100.0f *static_cast<float>(good_cal) / cal_counter <<" %" << std::endl;
    for(size_t i = 0; i < shift.size(); ++i)
    {
        data.v[i]->mov(shift[i]);
    }
}

void convert (mesh::Poly3gon& data,double radius_smoothing = 1,double smooth_coef = 1)
{
    size_t size = data.v.size();
    double r_inv = 1/radius_smoothing;
    std::vector<mesh::Vec3f> shift(size);
    size_t cal_counter = 0;
    for(size_t i = 0; i < size; ++i)
    {
        float mass = 0;
        mesh::Vec3f b;

        for(size_t j = 0; j < size; ++j)
        {
            const float distance = mesh::norm(data.v[i]->pos - data.v[j]->pos);
            ++ cal_counter;
            if (distance < radius_smoothing)
            {
                float m = static_cast<float>(1 - distance * r_inv);
                b += data.v[j]->pos * m;
                mass += m;
            }
        }

        if (mass > 0.0f)
            b /= mass;
        shift[i] = (1 - smooth_coef) * b + smooth_coef * data.v[i]->pos;
    }
    std::cout << cal_counter<< "cal " << std::endl;
    for(size_t i = 0; i < shift.size(); ++i)
    {
        data.v[i]->mov(shift[i]);
    }
}



int main(int argc, char ** argv)
{
    if(argc != 4)
    {
        std::cerr<< "too low argument" << std::endl;
        exit (-2);
    }
    mesh::Poly3gon res;
    mesh::Poly3gon test;
    std::string text;
    std::string file(argv[1]);
    file = file.substr(0,file.size() - 4);
    float start_volume = 0;
    double smooth_coef = std::atof(argv[2]);
    double radius_smoothing = std::atof(argv[3]);

    if(res.loadSTLBin(std::string(file + ".stl"), text) && test.loadSTLBin(std::string(file + ".stl"), text) )
    {
        start_volume = res.volume();
        std::cout << "load suc, tr:" << res.f.size()<< ", points:" << res.v.size() << ", volume:" << start_volume << std::endl;
    }
    else
    {
        std::cerr << "can't load" << std::endl;
        exit (-1);
    }

    time_t t_start = time(NULL);
    convert(res, radius_smoothing, smooth_coef);
    time_t t_1 = time(NULL);
    convert_cached(test, radius_smoothing, smooth_coef);
    time_t t_2 = time(NULL);

    std::cout << "no cache = " << t_1 - t_start << std::endl << "cache = " << t_2 - t_1 << std::endl;

    std::cout << res.volume() << " = " <<100.0 * (start_volume - res.volume()) / start_volume  <<" %" << std::endl;
    res.saveSTLBin(std::string(file + "_work.stl"), text);

    return 0;
}
