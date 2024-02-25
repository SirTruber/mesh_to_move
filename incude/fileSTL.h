#pragma once

#include <fstream>
#include <string>

#include "mesh.h"

bool loadSTLText (const std::string filename, std::string& text, mesh::Poly3gone & data)
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
}

bool loadSTLBin (const std::string filename, std::string& text, mesh::Poly3gone & data)
{
    std::ifstream input(filename, std::ios::binary);
    if (!input)
        return false;

    char message[80];
    mesh::Vertex vn, v1, v2, v3;
    float v[4][3];
    unsigned __int32 num;

    if (!input.read(message, 80)) return false;
    text.assign(message, size);

    if(!input.read(&num, 4)) return false;

    for (unsigned __int32 i = 0; i < num; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
            for(size_t k = 0; k < 3; ++k)
                if(!input.read( &v[j][k], 3)) return false;
        if(!input.read(message, 2)) return false;
        v1.mov(v[1][0],v[1][1],v[1][2]);
        v2.mov(v[2][0],v[2][1],v[2][2]);
        v3.mov(v[3][0],v[3][1],v[3][2]);
        data.push_back(v1,v2,v3);
    }
    return true;
}
