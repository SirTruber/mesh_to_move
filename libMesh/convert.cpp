#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <string>
#include <limits>

#include "../include/convert.h"
#include "../include/log.h"

using namespace mesh;

void mesh::convert_direct (Poly3gon& data,double radius_smoothing,double smooth_coef)
{
    const size_t size = data.v.size();
    std::vector<Vec3f> shift(size);
    //size_t good_cal = 0;
    for(size_t i = 0; i < size; ++i)
    {
        float mass = 0;
        Vec3f b;

        for(size_t j = 0; j < size; ++j)
        {
            const float distance = norm(data.v[i]->pos - data.v[j]->pos);
            if (distance < radius_smoothing)
            {
                //++good_cal;
                const float m = static_cast<float>(radius_smoothing - distance);
                b += data.v[j]->pos * m;
                mass += m;
            }
        }

        if (mass > 0.0f)
            b /= mass;
        shift[i] = (1 - smooth_coef) * b + smooth_coef * data.v[i]->pos;
    }
    // std::cout << "direct: " <<
    // std::endl <<"cal : " << size * size <<
    // std::endl <<"good_cal : " << good_cal <<" = "<< 100.0f * static_cast<float>(good_cal) / (size*size) <<" % cal" << std::endl << std::endl;
    for(size_t i = 0; i < shift.size(); ++i)
    {
        data.v[i]->mov(shift[i]);
    }
};

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

    // size_t read_counter = 0;
    // size_t cal_counter = 0;
    // size_t good_cal = 0;
    for(size_t i = 0; i < size; ++i)
    {
        float mass = static_cast<float>(radius_smoothing);
        Vec3f b = data.v[i]->pos * mass;

        line computed_value;
        line to_cache_value;

        size_t j = saved_value.pop(i,computed_value) + 1;

        while( !computed_value.empty())
        {
            b += data.v[computed_value.front().first]->pos * computed_value.front().second;
            mass += computed_value.front().second;

            computed_value.pop();
            //++read_counter;
        }

        for(; j < size; ++j)
        {
            const float distance = norm(data.v[i]->pos - data.v[j]->pos);
            //++cal_counter;
            if (distance < radius_smoothing)
            {
                //++good_cal;
                float m = static_cast<float>(radius_smoothing - distance);
                b += data.v[j]->pos * m;
                mass += m;

                if(saved_value())
                    to_cache_value.emplace(j,m);
            }
        }
        saved_value.push(i, to_cache_value);

        if (mass > 0.0f)
            b /= mass;
        shift[i] = (1 - smooth_coef) * b + smooth_coef * data.v[i]->pos;
    }
    // std::cout << "cached:" <<
    // std::endl << "read : " << read_counter <<
    // std::endl << "cal : " << cal_counter << " = " << 100.0f * static_cast<float> (cal_counter) / (size*size) << " % max " <<
    // std::endl << "good_cal : " << good_cal <<" = "<< 100.0f * static_cast<float>(good_cal) / cal_counter <<" % cal" << std::endl << std::endl;
    for(size_t i = 0; i < shift.size(); ++i)
    {
        data.v[i]->mov(shift[i]);
    }
};

Cubic_node::Cubic_node(const int a, const int b, const int c): pos(a,b,c)
{};

Cubic_node::Cubic_node(const Vec3i & vec)
{
    pos = vec;
};

void Cubic_node::add(const size_t ver)
{
    points.push_back(ver);
};

void Cubic_node::clear()
{
    points.clear();
    cached = true;
};

size_t CHash::operator()(const Vec3i & data) const noexcept
{
    std::string hash = std::to_string(data.x) + ";" + std::to_string(data.y) + ";" + std::to_string(data.z);
    return std::hash<std::string>{}(hash);
}

Cubic_view::Cubic_view(const Poly3gon& data, const double radius_smoothing)
{
    const size_t size = data.v.size();
    float max_value = static_cast<float>(std::numeric_limits<int>::max());
    if (radius_smoothing < 1.0f)
        max_value *=radius_smoothing;
    for(size_t i = 0; i < size; ++i)
    {
        Vec3f t = data.v[i]->pos;
        if( std::abs(t.x) > max_value ||
            std::abs(t.y) > max_value ||
            std::abs(t.z) > max_value)
            {
                std::cerr << i << " : int overflow"<<std::endl;
                continue;
            }
        Vec3i cube = t / radius_smoothing;
        auto it = _data.find(cube);
        if(it != _data.end())
            it->second.add(i);
        else
        {
            Cubic_node tmp(cube);
            tmp.add(i);
            _data.emplace(cube,tmp);
        }
    }
    _cur = _data.begin();
};

std::list<size_t> Cubic_view::get_current()
{
    return _cur->second.points;
};

std::list<size_t> Cubic_view::get_neighbours()
{
    std::list<size_t> res;
    int x0 = _cur->first.x;
    int y0 = _cur->first.y;
    int z0 = _cur->first.z;
    for(int i = - 1; i < 2; ++i)
    {
        for(int j = -1; j < 2; ++j)
        {
            for(int k = -1; k < 2; ++k)
            {
                if(i == 0 && j == 0 && k == 0) continue;
                Vec3i t(x0 + i, y0 + j, z0 + k);
                auto it = _data.find(t);
                if(it != _data.end() && !it->second.cached)
                    for(auto im :  it->second.points)
                        res.push_back(im);
            }
        }
    }
    return res;
};

void Cubic_view::mark_cached()
{
    _cur->second.clear();
};

bool Cubic_view::step()
{
    ++_cur;
    return _cur != _data.end();
};

void mesh::convert_cubic (Poly3gon& data,double radius_smoothing,double smooth_coef)
{
    const size_t size = data.v.size();
    // size_t read_counter = 0;
    // size_t cal_counter = 0;
    // size_t good_cal = 0;
    if (std::abs(1.0 - smooth_coef) < std::numeric_limits<double>::epsilon())
        return;

    if(std::abs(radius_smoothing) < std::numeric_limits<double>::epsilon())
    {
        for(size_t i = 0; i < size; ++i)
            data.v[i]->mov(smooth_coef * data.v[i]->pos);
        return;
    }

    Cache_crm saved_value(size);
    Cubic_view cv(data,radius_smoothing);
    do
    {
        std::queue<std::pair<size_t, Vec3f>> shift;

        bool cache_flag = true;
        std::list<size_t> root_cube = cv.get_current();
        std::list<size_t> neighbours_cube = cv.get_neighbours();
        for(auto it = root_cube.rbegin(); it != root_cube.rend(); ++it)
            neighbours_cube.push_front(*it);

        for(auto slow = root_cube.begin(); slow != root_cube.end(); ++slow)
        {
            float mass = static_cast<float>(radius_smoothing);
            Vec3f b = data.v[*slow]->pos * mass;

            line computed_value;
            line to_cache_value;

            saved_value.pop(*slow,computed_value);

            while( !computed_value.empty())
            {
                b += data.v[computed_value.front().first]->pos * computed_value.front().second;
                mass += computed_value.front().second;

                computed_value.pop();

                //++read_counter;
            }
            for(auto fast = next(neighbours_cube.begin()); fast != neighbours_cube.end(); ++fast)
            {
                const float distance = norm(data.v[*slow]->pos - data.v[*fast]->pos);
                //++cal_counter;

                if (distance < radius_smoothing)
                {

                    float m = static_cast<float>(radius_smoothing - distance);
                    b += data.v[*fast]->pos * m;
                    mass += m;

                    //++good_cal;
                    if(saved_value())
                        to_cache_value.emplace(*fast,m);
                }
            }
            if (mass > 0.0f)
                b /= mass;
            Vec3f p = (1 - smooth_coef) * b + smooth_coef * data.v[*slow]->pos;
            shift.emplace(*slow, p);

            if(saved_value.push(*slow, to_cache_value))
                neighbours_cube.pop_front();
            else
                cache_flag = false;
        }
        while(!shift.empty())
        {
            data.v[shift.front().first]->mov(shift.front().second);

            shift.pop();
        }

        if( cache_flag) cv.mark_cached();
    }
    while(cv.step());

    // std::cerr << "test";
    // std::cout << "cubic: " <<
    // std::endl << "read :" << read_counter <<
    // std::endl << "cal : " << cal_counter << " = " << 100.0f * static_cast<float> (cal_counter) / (size*size) << " % max" <<
    // std::endl << "good_cal : " << good_cal <<" = "<< 100.0f * static_cast<float> (good_cal) / (cal_counter) << " % cal" << std::endl << std::endl;
};
