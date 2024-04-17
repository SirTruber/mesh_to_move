#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>

#include "../include/mesh.h"

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

void convert (mesh::Poly3gon& data,double radius_smoothing = 1,double smooth_coef = 1)
{
    size_t size = data.v.size();
    double r_inv = 1/radius_smoothing;
    //std::vector<float> distance_map(size * (size - 1) / 2 );
    std::vector<mesh::Vec3f> shift(size);
    for(size_t i = 0; i < size; ++i)
    {
        float mass = 0;
        mesh::Vec3f b;
        for(size_t j = 0; j < size; ++j)
        {
            const float distance = mesh::norm(data.v[i]->pos - data.v[j]->pos);
            //distance_map.push_back(distance);
            if (distance < radius_smoothing)
            {
                const float m = static_cast<float>(1 - distance * r_inv);
                b += data.v[j]->pos * m;
                mass += m;
            }
        }

        // for(size_t j = 0 ; j < i; ++j)
        // {
        //     const float distance = distance_map[i + j * size];
        //     if(distance < radius_smoothing)
        //     {
        //         const float m = static_cast<float>(1 - distance * r_inv);
        //         b += data.v[j]->pos * m;
        //         mass += m;
        //     }
        // }
        if (mass > 0.0f)
            b /= mass;
        shift[i] = (1 - smooth_coef) * b + smooth_coef * data.v[i]->pos;
    }

    for(size_t i = 0; i < shift.size(); ++i)
    {
        data.v[i]->mov(shift[i]);
    }
}
// void convert( mesh::Poly3gon& data)
// {
//     std::vector<mesh::Vec3f> to_move(data.v.size());
//
//     for(size_t i = 0; i < data.v.size(); ++i)
//     {
//         mesh::Vec3f sum_normal;
//         mesh::Vec3f normal;
//         //to_move[i] = data.v[i]->pos;
//         for(size_t j = 0; j < data.v[i]->f.size(); ++j)
//         {
//             mesh::Facet triangle = *data.v[i]->f[j].lock();
//             normal = mesh::get_normal(triangle);
//             sum_normal += normal;
//             to_move[i] += mesh::norm(normal) * mesh::get_center(triangle);
//         }
//
//     //to_move[i] += 3 * data.v[i]->pos;
//
//     if (sum_normal.x > std::numeric_limits<float>::epsilon()
//     &&  sum_normal.y > std::numeric_limits<float>::epsilon()
//     &&  sum_normal.z > std::numeric_limits<float>::epsilon())
//             to_move[i] /= sum_normal;
//     }
//     for(size_t i = 0; i < data.v.size(); ++i)
//     {
//         data.v[i]->mov(to_move[i]);
//     }
// }


int main(int argc, char ** argv)
{
    if(argc != 4)
    {
        std::cerr<< "too low argument" << std::endl;
        exit (-2);
    }
    mesh::Poly3gon res;
    std::string text;
    std::string file(argv[1]);
    file = file.substr(0,file.size() - 4);
    float start_volume = 0;
    double smooth_coef = std::atof(argv[2]);
    double radius_smoothing = std::atof(argv[3]);

    if(res.loadSTLBin(std::string(file + ".stl"), text))
    {
        start_volume = res.volume();
        std::cout << "load suc, tr:" << res.f.size()<< ", points:" << res.v.size() << ", volume:" << start_volume << std::endl;
    }
    else
    {
        std::cerr << "can't load" << std::endl;
        exit (-1);
    }
    //saveSTLBin(std::string(file + "_save.stl"), text, res);

    convert(res, radius_smoothing, smooth_coef);
        //std::cout << "volume:" << get_volume(res)<< std::endl;

    std::cout << res.volume() << " = " <<100.0 * (start_volume - res.volume()) / start_volume  <<" %" << std::endl;
    res.saveSTLBin(std::string(file + "_work.stl"), text);

    return 0;
}
