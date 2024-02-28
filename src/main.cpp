#include <iostream>

#include "../include/fileSTL.h"


void convert( mesh::Poly3gon& data)
{
    std::vector<mesh::Vec3f> to_move(data.v.size());

    for(size_t i = 0; i < data.v.size(); ++i)
    {
        mesh::Vec3f sum_normal;
        mesh::Vec3f normal;
        //to_move[i] = data.v[i]->pos;
        for(size_t j = 0; j < data.v[i]->f.size(); ++j)
        {
            mesh::Facet triangle = *data.v[i]->f[j].lock();
            normal = mesh::get_normal(triangle);
            sum_normal += normal;
            to_move[i] += mesh::norm(normal) * mesh::get_center(triangle);
        }

    //to_move[i] += 3 * data.v[i]->pos;

    if (sum_normal.x > std::numeric_limits<float>::epsilon()
    &&  sum_normal.y > std::numeric_limits<float>::epsilon()
    &&  sum_normal.z > std::numeric_limits<float>::epsilon())
            to_move[i] /= sum_normal;
    }
    for(size_t i = 0; i < data.v.size(); ++i)
    {
        data.v[i]->mov(to_move[i]);
    }
}
int main(int argc, char ** argv)
{
    mesh::Poly3gon res;
    std::string text;

    if(loadSTLBin("teapot.stl", text, res))
        std::cout << "load suc, tr:" << res.f.size()<< ", points:" << res.v.size() << ", volume:" << get_volume(res) << std::endl;
    saveSTLText("teapot_save.stl", text, res);

    convert(res);
        std::cout << "volume:" << get_volume(res)<< std::endl;
    saveSTLText("teapot_work.stl", text, res);

    return 0;
}
