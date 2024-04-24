#include <iostream>
#include <utility>
#include <ctime>

#include<unistd.h>
int getopt(int argc, char ** argv, const char* optsring);
extern char* optarg;
extern int optind, opterr, optopt;

#include "../include/convert.h"

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
    std::cout << test.volume() << " = " <<100.0 * (start_volume - test.volume()) / start_volume  <<" %" << std::endl;
    res.saveSTLBin(std::string(file + "_work.stl"), text);
    test.saveSTLBin(std::string(file + "_test.stl"), text);

    return 0;
}
