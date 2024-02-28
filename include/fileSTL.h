#pragma once

#include <fstream>
#include <string>
#include <ctime>
#include <cstdint>

#include "mesh.h"

bool loadSTLText (const std::string filename, std::string& text, mesh::Poly3gon& data)
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
}

bool loadSTLBin (const std::string& filename, std::string& text, mesh::Poly3gon& data)
{
    std::ifstream input(filename, std::ios::binary);
    if (!input)
        return false;

    char message[80];
    std::uint32_t num;
    float v[12];
    mesh::Vertex vec[3];

    if (!input.read(message, 80)) return false;
    text.assign(message, 80);
    if(!input.read(reinterpret_cast<char*>(&num), sizeof(num))) return false;
    std::clock_t t0 = std::clock();
    std::clock_t t = std::clock();
    for (std::uint32_t i = 0; i < num; ++i)
    {
        if(!input.read( reinterpret_cast<char*>(v), sizeof(v))) return false;
        if(!input.read(message, 2)) return false;

        vec[0].mov(v[3],v[4],v[5]);
        vec[1].mov(v[6],v[7],v[8]);
        vec[2].mov(v[9],v[10],v[11]);

        data.push_back(vec[0],vec[1],vec[2]);
        if (i % 10000 == 0)
        {
            std::cout << (std::clock() - t)/CLOCKS_PER_SEC << " seconds from last"
            << std::endl << (std::clock() - t0)/CLOCKS_PER_SEC << " seconds from start" << std::endl;
            t = std::clock();
        }
    }
    input.close();
    return true;
}

bool saveSTLText(const std::string& filename, const std::string& text, const mesh::Poly3gon& data)
{
    std::ofstream output(filename);
    if (!output)
        return false;

    output << "solid " << text << std::endl;

    std::uint32_t num = static_cast<std::uint32_t>(data.f.size());
    mesh::Vec3f vec[4];

    for (std::uint32_t i = 0; i < num; ++i)
    {
        vec[0] = mesh::get_normal(*data.f[i]);
        vec[1] = data.f[i]->v[0].lock()->pos;
        vec[2] = data.f[i]->v[1].lock()->pos;
        vec[3] = data.f[i]->v[2].lock()->pos;

        vec[0] /= mesh::norm(vec[0]);
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
}


bool saveSTLBin (const std::string& filename, const std::string& text, const mesh::Poly3gon& data)
{
    std::ofstream output(filename, std::ios::binary);
    if (!output)
        return false;

    char message[80];
    std::uint32_t num = static_cast<std::uint32_t>(data.f.size());
    mesh::Vec3f vec[4];
    float pos[12];

    for (size_t i = 0; i < 80; ++i)
        message[i] = i < text.size() ? text[i] : '0';

    output.write(message, 80);
    output.write(reinterpret_cast<char*>(&num), sizeof(num));
    message[0] = message[1] = 0;


    for (std::uint32_t i = 0; i < num; ++i)
    {
        vec[0] = mesh::get_normal(*data.f[i]).set_norm();
        vec[1] = data.f[i]->v[0].lock()->pos;
        vec[2] = data.f[i]->v[1].lock()->pos;
        vec[3] = data.f[i]->v[2].lock()->pos;

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
}
