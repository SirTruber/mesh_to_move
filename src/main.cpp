#include <iostream>
#include <vector>

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

std::vector<Weak_ver> find_adjacent(Shared_ver& ver)
{
    std::vector <Weak_ver> res;
    for (size_t i = 0; i < ver->f.size();++i)
    {
        Shared_tr triangle = ver->f[i].lock();

        if(!triangle)
            continue;

        for(size_t j = 0; j < 3; ++j)
        {
            Weak_ver tmp = triangle->v[j];
            bool was_find = false;

            for (size_t k = 0; k < res.size(); ++k )
            {
                if(equals(res[k],tmp))
                    was_find = true;
            }
            if(!was_find)
                res.push_back(tmp);
        }
    }
    return res;
}

mesh::Vec3f compute_displacement(Shared_ver& ver, double radius_smoothing)
{
    mesh::Vec3f start(ver->pos);
    mesh::Vec3f end ;
    std::vector<Shared_ver> close_points;

    close_points.push_back(ver);

    for (size_t i = 0; i < close_points.size(); ++i)
    {
        std::vector<Weak_ver> tmp (find_adjacent(close_points[i]));

        // for(size_t j = 0; j < tmp.size(); ++j)
        // {
        //     Shared_ver t = tmp[j].lock();
        //
        //     if(!t)
        //         continue;
        //
        //     mesh::Vec3f shift = t->pos - start;
        //     float distance = norm(shift);
        //     bool was_find = false;
        //
        //     if(distance >= radius_smoothing)
        //         break;
        //
        //     for(size_t k = 0; k < close_points.size(); ++k)
        //     {
        //         if(t == close_points[k])
        //             was_find = true;
        //     }
        //
        //     if(!was_find )
        //     {
        //         close_points.push_back(t);
        //         shift*= static_cast<float>(1.0 - distance/radius_smoothing);
        //
        //         end += shift;
        //     }
        // }
        for(size_t i = 0; i < tmp.size(); ++i)
        {
            end += tmp[i].lock()->pos;
        }
        end /= tmp.size();
    }
    return end;
}

void convert (mesh::Poly3gon& data,double radius_smoothing = 1)
{
    mesh::Poly3gon& new_data(data);

    for(size_t i = 0; i < new_data.v.size(); ++i)
    {
        if(data.v[i])
        {
            mesh::Vec3f b(compute_displacement(new_data.v[i], radius_smoothing));

            mesh::Vec3f old = new_data.v[i]->pos;
            mesh::Vec3f shift = old - b;
            data.v[i]->mov(b + radius_smoothing * shift);
        }
    }
    data = new_data;
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
    if(argc != 3)
    {
        std::cerr<< "too low argument" << std::endl;
        exit (-2);
    }
    mesh::Poly3gon res;
    std::string text;
    std::string file(argv[1]);
    file = file.substr(0,file.size() - 4);
    float start_volume = 0;
    double argument = std::atof(argv[2]);

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

    convert(res, argument);
        //std::cout << "volume:" << get_volume(res)<< std::endl;

    std::cout << res.volume() << " = " <<100.0 * (start_volume - res.volume()) / start_volume  <<" %" << std::endl;
    res.saveSTLBin(std::string(file + "_work.stl"), text);

    return 0;
}
