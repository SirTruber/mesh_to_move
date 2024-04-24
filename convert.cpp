#include <iostream>
#include <vector>
#include <queue>
#include <list>

#include "../include/convert.h"

using namespace mesh;


Cache_crm::Cache_crm(size_t size) :_data(size)
{};

bool Cache_crm::operator ()()
{
    return !_breakpoint;
};

bool Cache_crm::push( size_t i, line & val)
{
    if (_breakpoint)
        return false;

    count += val.size();

    if (count > _max_size)
        _breakpoint = true;

    while(!val.empty())
    {
        const size_t tmp = val.front().first;

        val.front().first = i;
        _data[tmp].push(val.front());
        val.pop();
    }
    ++breakage;
    return true;
};

size_t Cache_crm::pop(size_t i, line & move_value)
{
    move_value = _data[i];
    count -= move_value.size();
    while(!_data[i].empty()) _data[i].pop();
    return breakage;
};

void mesh::convert_cached (Poly3gon& data,double radius_smoothing,double smooth_coef)
{
    const size_t size = data.v.size();
    Cache_crm saved_value(size);
    std::vector<Vec3f> shift(size);

    size_t read_counter = 0;
    size_t cal_counter = 0;
    size_t good_cal = 0;
    for(size_t i = 0; i < size; ++i)
    {
        float mass = radius_smoothing;
        Vec3f b = data.v[i]->pos * mass;

        line computed_value;
        line to_cache_value;

        size_t j = saved_value.pop(i,computed_value) + 1;

        while( !computed_value.empty())
        {
            b += data.v[computed_value.front().first]->pos * computed_value.front().second;
            mass += computed_value.front().second;

            computed_value.pop();
            ++read_counter;
        }

        for(; j < size; ++j)
        {
            const float distance = norm(data.v[i]->pos - data.v[j]->pos);
            ++cal_counter;
            if (distance < radius_smoothing)
            {
                ++good_cal;
                float m = static_cast<float>(radius_smoothing - distance);
                b += data.v[j]->pos * m;
                mass += m;

                if(saved_value())
                {
                    std::pair<size_t, float> tmp (j,m);
                    to_cache_value.push(tmp);
                }
            }
        }
        saved_value.push(i, to_cache_value);

        if (mass > 0.0f)
            b /= mass;
        shift[i] = (1 - smooth_coef) * b + smooth_coef * data.v[i]->pos;
    }
    std::cout << "cached:" <<
    std::endl << "read : " << read_counter <<
    std::endl << "cal : " << cal_counter << " = " << 100.0f * static_cast<float> (cal_counter) / (size*size) << " % max " <<
    std::endl << "good_cal : " << good_cal <<" = "<< 100.0f * static_cast<float>(good_cal) / cal_counter <<" % cal" << std::endl;
    for(size_t i = 0; i < shift.size(); ++i)
    {
        data.v[i]->mov(shift[i]);
    }
};

void mesh::convert (Poly3gon& data,double radius_smoothing,double smooth_coef)
{
    const size_t size = data.v.size();
    std::vector<Vec3f> shift(size);
    size_t good_cal = 0;
    for(size_t i = 0; i < size; ++i)
    {
        float mass = 0;
        Vec3f b;

        for(size_t j = 0; j < size; ++j)
        {
            const float distance = norm(data.v[i]->pos - data.v[j]->pos);
            if (distance < radius_smoothing)
            {
                ++good_cal;
                const float m = static_cast<float>(radius_smoothing - distance);
                b += data.v[j]->pos * m;
                mass += m;
            }
        }

        if (mass > 0.0f)
            b /= mass;
        shift[i] = (1 - smooth_coef) * b + smooth_coef * data.v[i]->pos;
    }
    std::cout << "naive: " <<
    std::endl <<"cal : " << size * size <<
    std::endl <<"good_cal : " << good_cal <<" = "<< 100.0f * static_cast<float>(good_cal) / (size*size) <<" % cal" << std::endl;;
    for(size_t i = 0; i < shift.size(); ++i)
    {
        data.v[i]->mov(shift[i]);
    }
};
